#include <Arduino.h>
#include <RadioLib.h>

// SX1262 pin configuration matching the wiring above
// NSS pin: 10, DIO1 pin: 2, NRST pin: 9, BUSY pin: 3
// SX1262 lora = new Module(10, 2, 9, 3);

// Mega 2560
SX1262 lora = new Module(10, 2, 9, 3); // NSS: 10, DIO1: 2, NRST: 9, BUSY: 3


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.print(F("[SX1262] Initializing ... "));
  
  // Initialize LoRa at 433.0 MHz (Change to 868.0 or 915.0 depending on your exact version)
  // Carrier Frequency, Bandwidth (kHz), Spreading Factor, Coding Rate, Sync Word, Preamble Length
  int state = lora.begin(433.0, 125.0, 7, 5, 0x12, 10, 22, false);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Initialization successful!"));
  } else {
    Serial.print(F("Initialization failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print(F("[SX1262] Transmitting packet ... "));

  // Send data over LoRa
  int state = lora.transmit("Hello from DX-PJ27!");

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Success!"));
  } else {
    Serial.print(F("Failed, code "));
    Serial.println(state);
  }

  // Wait 5 seconds before transmitting again
  delay(5000);
}
