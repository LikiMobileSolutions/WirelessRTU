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
static uint8_t TxdBuffer[64];
static uint8_t Txwiadomosc[64];
//uint8_t *wiadomosc = (uint8_t *)malloc(sizeof(uint8_t));
char wiadomosc[0];
static uint16_t received_adress[1];
int sizemessage = 0;
long long int czas = 0;
long long int czast = 0;
int n =1;

void setup()
{
	adress[0] = '9';
	// Initialize Serial for debug output
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

	Serial.println("=====================================");
	Serial.println("LoRap2p Tx Test");
	Serial.println("=====================================");

	// Initialize the Radio callbacks
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = NULL;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = NULL;
	RadioEvents.RxError = NULL;
	RadioEvents.CadDone = NULL;

	// Initialize the Radio
	Radio.Init(&RadioEvents);

	// Set Radio channel
	Radio.SetChannel(RF_FREQUENCY);

	// Set Radio RX configuration
	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
					  LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
					  LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
					  0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

	if (received_adress[0] = !adress[0])
	{
		Serial.println("Wrong node adress");
	}
	else
	{
		Serial.println("Node adress OK!");
		// Set Radio TX configuration
		Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
						  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
						  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
						  true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);
	}

	// Część do obsługi RS-a
	RS485.setPins(RS485_TX_PIN, RS485_DE_PIN, RS485_RE_PIN);
	RS485.begin(9600);

	/* IO2 HIGH  3V3_S ON */
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(500);
	/* IO2 HIGH  3V3_S ON */

	// enable reception, can be disabled with: RS485.noReceive();
	RS485.receive();
	Serial.println("RAK5802 RS485 test");
}
void send()
{

	//sizemessage = 0;

	//sizemessage = sizeof(wiadomosc);


	//Radio.Send(TxdBuffer, sizemessage);
	


	/*
	TxdBuffer[0] = '5';
	TxdBuffer[1] = '2';
	TxdBuffer[2] = '3';
	TxdBuffer[3] = '4';
	TxdBuffer[4] = adress[0];
	Radio.Send(TxdBuffer, 5);
	*/
	Radio.Send(TxdBuffer, sizemessage);
	
	//Radio.Send((uint8_t *)wiadomosc, 64);
}

void loop()
{
	
	//send();
	sizemessage = sizeof(wiadomosc);
	// Obsługa RS-a
	if (RS485.available())
	{

		czas = millis();
		//Serial.print(czas);



		TxdBuffer[0] = adress[0];
			switch(RS485.read())
			{
				case '1':
					Serial.print("1");
					TxdBuffer[n] = '1';

					break;
				
				case '2':
					Serial.print("2");
					TxdBuffer[n] = '2';
					break;
				
				case '3':
					Serial.print("3");
					TxdBuffer[n] = '3';
					break;
				
				case '4':
					Serial.print("4");
					TxdBuffer[n] = '4';
					break;
				
				case '5':
					Serial.print("5");
					TxdBuffer[n] = '5';
					break;
				
				case '6':
					Serial.print("6");
					TxdBuffer[n] = '6';
					break;
				
				case '7':
					Serial.print("7");
					TxdBuffer[n] = '7';
					break;
				
				case '8':
					Serial.print("8");
					TxdBuffer[n] = '8';
					break;
				
				case '9':
					Serial.print("9");
					TxdBuffer[n] = '9';
					break;
				
				

			}
			
			
		n++;
		
	
	}
	else
	{
		czast = millis();
		if ((czas+2 < czast) && (czast< czas +4))
		{
			Serial.println(" ");
			sizemessage = n-1;
			n=1;
			send();

		}
	}

}

/**@brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void)
{
	Serial.println("OnTxDone");
	
}

/**@brief Function to be executed on Radio Tx Timeout event
 */
void OnTxTimeout(void)
{
	Serial.println("OnTxTimeout");
}
