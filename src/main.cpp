#include <Arduino.h>
#include "LoRaWan-Arduino.h" //http://librarymanager/All#SX126x
#include <SPI.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <ArduinoRS485.h> // Biblioteka RS
#include "mbed.h"
#include "rtos.h"

using namespace std::chrono_literals;
using namespace std::chrono;

// Function declarations
void OnTxDone(void);
void OnTxTimeout(void);
void send(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnRxTimeout(void);
void OnRxError(void);
void RS_send(void);
void Lora_send_mode(void);
void RS_onetime_send(void);

// Definicje do RS-a
#define RS485_TX_PIN 0
#define RS485_DE_PIN 6
#define RS485_RE_PIN 1

// Define LoRa parameters
#define RF_FREQUENCY 868300000	// Hz
#define TX_OUTPUT_POWER 22		// dBm
#define LORA_BANDWIDTH 0		// [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR 7 // [SF7..SF12]
#define LORA_CODINGRATE 1		// [1: 4/5, 2: 4/6,  3: 4/7,  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8	// Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0	// Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE 3000
#define TX_TIMEOUT_VALUE 3000
static uint8_t adress[1];
static RadioEvents_t RadioEvents;
static uint8_t RcvBuffer[64];
boolean lora_received =0;
boolean RS_received = 0;
uint8_t rxBuff[50];
uint8_t rsBuff[50];
int licznik =0;
int RS_send_index = 0;
boolean RS_ready = true;

void setup()
{
	adress[0] = 0x08; // klucz identyfikacyjny urzadzenie
	time_t timeout = millis();
	Serial.begin(115200);
	while (!Serial)
	{
		if ((millis() - timeout) < 5000)
		{
			delay(100);
		}
		else
		{
			break;
		}
	}
	// Initialize LoRa chip.
	lora_rak11300_init();

	// Initialize the Radio callbacks
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;
	RadioEvents.CadDone = NULL;

	// Initialize the Radio
	Radio.Init(&RadioEvents);
	Radio.SetChannel(RF_FREQUENCY);

	// Init RS
	RS485.setPins(RS485_TX_PIN, RS485_DE_PIN, RS485_RE_PIN);
	RS485.begin(9600);
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(500);
	RS485.receive();

	//SETUP LORA MODE
	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);			
	Radio.Rx(RX_TIMEOUT_VALUE);


}
void send()
{
	licznik++;
	Radio.Send(rxBuff, licznik);
}


void loop()
{
	if(lora_received == 1)
	{	
		if(RS_ready == true)
		{RS_send();}
		lora_received =0;
	}
	else
	{
		// Obsługa RS-a
		if (RS485.available())
		{
			
			rxBuff[licznik] = RS485.read();
			licznik++;
			RS_received =1; 
			delay(5);
		}
		else
		{
			if (RS_received == 1)
			{
				rxBuff[0] = 0x08; // TU WRÓC 
				Lora_send_mode();
				licznik = 1;
			}
			RS_received = 0;
		}
	}
}



/**@brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void)
{
	//Serial.println("OnTxDone");
	
}

/**@brief Function to be executed on Radio Tx Timeout event
 */
void OnTxTimeout(void)
{
	Serial.println("OnTxTimeout");
}


void RS_send(void)
{
	RS485.beginTransmission();
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	for(int i=0; i<RS_send_index-1; i++)
	{
		RS485.write(rsBuff[i]);
	}

	RS485.endTransmission();

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	RS485.receive();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
	RS_ready = false;
	Serial.println("OnRxDone");
	delay(10);
	memcpy(RcvBuffer, payload, size);
	if ((RcvBuffer[0]) == (adress[0]))
  	{
		RS_send_index=0;
		for (int idx = 0; idx < size-1; idx++)
		{
		rsBuff[idx] = RcvBuffer[idx+1];
		RS_send_index++;
		}
		Radio.Rx(RX_TIMEOUT_VALUE);
		lora_received = 1;
  	}
  	RS_ready = true;
}

void OnRxTimeout(void)
{
  Serial.println("OnRxTimeout");
  Radio.Rx(RX_TIMEOUT_VALUE);
}

/**@brief Function to be executed on Radio Rx Error event
 */
void OnRxError(void)
{
  Serial.println("OnRxError");
  Radio.Rx(RX_TIMEOUT_VALUE);
}

void Lora_send_mode(void)
{
	Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);
	delay(45);
	send();

	delay(45);

	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
 	Radio.Rx(RX_TIMEOUT_VALUE);

}