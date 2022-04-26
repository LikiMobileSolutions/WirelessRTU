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
static uint8_t RcvBuffer[64];
static char RcvBuffer_RS_send[64];
int sizemessage = 0;
long long int czas = 0;
long long int czast = 0;
int n =1;
boolean RS_mode = 0;
boolean lora_received =0;

void setup()
{
	adress[0] = '8';
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
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;
	RadioEvents.CadDone = NULL;

	// Initialize the Radio
	Radio.Init(&RadioEvents);

	// Set Radio channel
	Radio.SetChannel(RF_FREQUENCY);
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

	//SET LORA TO RX

	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

	Radio.Rx(RX_TIMEOUT_VALUE);

}
void send()
{

	Radio.Send(TxdBuffer, sizemessage);
	memset(TxdBuffer,'\0',64);

}



void loop()
{
	if(lora_received == 1)
	{
		RS_send();
		lora_received =0;
	}
	else
	{
		// Obsługa RS-a
		if (RS485.available())
		{

			czas = millis();
			TxdBuffer[0] = adress[0];
				switch(RS485.read())
				{
					case '0':
						Serial.print("0");
						TxdBuffer[n] = '0';

						break;
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
			if ((czas+2 < czast) && (czast < czas +4))
			{
				Serial.println(" ");
				sizemessage = n-1;
				n=1;
				Lora_send_mode();

			}
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
	/* IO2 HIGH  3V3_S ON */
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(30); //300
	/* IO2 HIGH  3V3_S ON */

	RS485.write(RcvBuffer_RS_send);
	Serial.print("wysylam po RS: ");
	Serial.println(RcvBuffer_RS_send);
	RS485.endTransmission();
	memset(RcvBuffer_RS_send,'\0',64);
	
	
	//SET RS TO RECIVER
	/* IO2 LOW  3V3_S OFF */
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);
	delay(30); //
	/* IO2 LOW  3V3_S OFF */
	Serial.println("uart send is done");

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(50); // 500
	/* IO2 HIGH  3V3_S ON */

	// enable reception, can be disabled with: RS485.noReceive();
	RS485.receive();
	Serial.println("RS ready to receive");

}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{

  //Serial.println("OnRxDone");
  delay(10);
  memcpy(RcvBuffer, payload, size); // copy do destination (destinatio,source,size)

  // Serial.printf("RssiValue=%d dBm, SnrValue=%d\n", rssi, snr);
  String str = (char *)RcvBuffer;
  //Serial.println("parownanie");
  //Serial.println(RcvBuffer[0] - 48);
  //Serial.println(adress[0] - 48);
  if ((RcvBuffer[0] - 48) == (adress[0] - 48))
  {
    for (int idx = 0; idx < size-1; idx++)
    {

      //Serial.println(RcvBuffer[idx] - 48);
	  RcvBuffer_RS_send[idx] = RcvBuffer[idx+1];
	  
    }
	memset(RcvBuffer,'\0',64);

    Radio.Rx(RX_TIMEOUT_VALUE);
	lora_received = 1;
  }
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

	send();

	delay(100);

	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
 	Radio.Rx(RX_TIMEOUT_VALUE);

}