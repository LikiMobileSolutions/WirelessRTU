#include <Arduino.h>
#include "LoRaWan-Arduino.h" //http://librarymanager/All#SX126x
#include <SPI.h>
#include <stdarg.h>
#include <stdio.h>
#include <ArduinoRS485.h> // Biblio do RS-a jako sender
#include "mbed.h"
#include "rtos.h"

using namespace std::chrono_literals;
using namespace std::chrono;

// Function declarations
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnRxTimeout(void);
void OnRxError(void);
void RS(void);
// Definicje do RS-a
#define RS485_TX_PIN 0
#define RS485_DE_PIN 6
#define RS485_RE_PIN 1


// Define LoRa parameters
#define RF_FREQUENCY 868300000  // Hz
#define TX_OUTPUT_POWER 22      // dBm
#define LORA_BANDWIDTH 0        // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR 7 // [SF7..SF12]
#define LORA_CODINGRATE 1       // [1: 4/5, 2: 4/6,  3: 4/7,  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8  // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0   // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE 3000
#define TX_TIMEOUT_VALUE 3000
static uint8_t adress[1];
//static uint8_t received_adress[1];
static RadioEvents_t RadioEvents;
static uint8_t RcvBuffer[64];
static uint8_t RxBuffer[64];
int idx = 0;
static uint16_t size_1;
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
  Serial.println("LoRaP2P Rx Test");
  Serial.println("=====================================");

  // Initialize the Radio callbacks
  RadioEvents.TxDone = NULL;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = NULL;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;
  RadioEvents.CadDone = NULL;

  // Initialize the Radio
  Radio.Init(&RadioEvents);

  // Set Radio channel
  Radio.SetChannel(RF_FREQUENCY);
  // Set Radio TX configuration
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  for (int i = 0; i <= 5; i++)
  {
    Radio.Send(adress, 1);
  }

  // Set Radio RX configuration
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  // Start LoRa
  Serial.println("Starting Radio.Rx");
  Radio.Rx(RX_TIMEOUT_VALUE);
  // radio.Read(received_adress[0]);

  // Konf do RS-a
// Część do obsługi RS-a
	RS485.setPins(RS485_TX_PIN, RS485_DE_PIN, RS485_RE_PIN);
	RS485.begin(9600);


}
void loop()
{

//  for (int cnt=0; cnt<=64; cnt++)
//{  	RS485.print(RxBuffer[cnt]);
//}
  
 }

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{

  delay(10);
  memcpy(RcvBuffer, payload, size); // copy do destination (destinatio,source,size)
  size_1=size;
  // Serial.printf("RssiValue=%d dBm, SnrValue=%d\n", rssi, snr);
  String str = (char *)RcvBuffer;
  //Serial.println("porownanie");
  //Serial.println(RcvBuffer[0] - 48);
  //Serial.println(adress[0] - 48);
  if ((RcvBuffer[0]) == adress[0])
  {
    for (int idx = 0; idx < size; idx++)
    {
      Serial.println(RcvBuffer[idx] - 48);
      RxBuffer[idx]=RcvBuffer[idx]-48;
    }
    idx = 0;
    Radio.Rx(RX_TIMEOUT_VALUE);
    
  }
  else
  {
    Serial.println("wrong node adress");
  }
  RS();
}

void RS(void)
{
  RS485.beginTransmission();
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  int id=0;
   for (id = 1; id < size_1;id++)
    {
       RS485.write(RcvBuffer[id]);
       RS485.flush();
    }
    id= 0;
  RS485.endTransmission();
  delay(1000);
}

/**@brief Function to be executed on Radio Rx Timeout event
 */
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