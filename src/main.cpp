#include <Arduino.h>
#include <RadioLib.h>

// ============================================================================
// BOARD SELECTION (Uncomment ONLY ONE matching your current target board)
// ============================================================================

// --- CONFIG 1: Arduino Uno / Nano ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// SX1262 lora = new Module(10, 2, 9, 3);

// --- CONFIG 2: Arduino Mega 2560 ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
SX1262 lora = new Module(10, 2, 9, 3);

// --- CONFIG 3: Teensy 4.1 ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// SX1262 lora = new Module(10, 2, 9, 3);

// Active hardware constructor line
// SX1262 lora = new Module(10, 2, 9, 3);

// ============================================================================

// Timeout tracker to jumpstart the ping-pong if communication stalls
unsigned long lastActionTime = 0;
const unsigned long TIMEOUT_MS = 6000; 

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("\n--- DX-PJ27 LoRa Ping-Pong Node ---"));
  Serial.print(F("[SX1262] Initializing ... "));
  
  // Initialize LoRa at 433.0 MHz (Change to 868.0 or 915.0 if needed)
  int state = lora.begin(433.0, 125.0, 7, 5, 0x12, 10, 8, false);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Initialization successful!"));
  } else {
    Serial.print(F("Initialization failed, code "));
    Serial.println(state);
    while (true);
  }

  // Put module into continuous receive mode
  state = lora.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print(F("Failed to start receive mode, code "));
    Serial.println(state);
    while (true);
  }

  lastActionTime = millis();
}

void loop() {
  String receivedStr;
  
  // Check if a packet has been received
  int state = lora.readData(receivedStr);

  if (state == RADIOLIB_ERR_NONE) {
    // Packet successfully received!
    Serial.print(F("[SX1262] Received packet! Data: \""));
    Serial.print(receivedStr);
    Serial.print(F("\" | RSSI: "));
    Serial.print(lora.getRSSI());
    Serial.print(F(" dBm | SNR: "));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));

    // Formulate a reply
    String replyStr = "";
    if (receivedStr == "Ping") {
      replyStr = "Pong";
    } else if (receivedStr == "Pong") {
      replyStr = "Ping";
    } else {
      replyStr = "Ping"; // Default fallback
    }

    // Small delay so the other device has time to switch back to listening mode
    delay(500); 

    // Transmit the response
    Serial.print(F("[SX1262] Transmitting response: "));
    Serial.println(replyStr);
    
    int txState = lora.transmit(replyStr);
    if (txState == RADIOLIB_ERR_NONE) {
      Serial.println(F("[SX1262] Transmission success. Returning to listen mode."));
    } else {
      Serial.print(F("[SX1262] Transmission failed, code "));
      Serial.println(txState);
    }

    // Always re-enable receive mode after transmitting
    lora.startReceive();
    lastActionTime = millis(); // Reset timeout tracker
  } 
  // FIX applied here: Changed to filter out RADIOLIB_ERR_RX_TIMEOUT
  else if (state != RADIOLIB_ERR_NONE && state != RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.print(F("Receive logic encountered error: "));
    Serial.println(state);
  }

  // TIMEOUT KICKSTART LOOP: 
  // If neither device talks for 6 seconds, this node will transmit a "Ping" 
  if (millis() - lastActionTime > TIMEOUT_MS) {
    Serial.println(F("[Timeout] Silence detected. Kickstarting sequence..."));
    
    int txState = lora.transmit("Ping");
    if (txState == RADIOLIB_ERR_NONE) {
      Serial.println(F("[SX1262] Kickstart Ping sent successfully."));
    }
    
    lora.startReceive();
    lastActionTime = millis();
  }
}
