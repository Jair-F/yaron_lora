#include <Arduino.h>
#include <RadioLib.h>

// ============================================================================
// ROLE SELECTION 
// ============================================================================
// Set to true for Device 1 (Sends "Hi", waits for "Bye")
// Set to false for Device 2 (Waits for "Hi", replies with "Bye")
const bool IS_SENDER_NODE = false; 

// ============================================================================
// BOARD SELECTION (Uncomment ONLY ONE matching your current target board)
// ============================================================================

// --- CONFIG 1: Arduino Uno / Nano ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// SX1262 lora = new Module(10, 2, 9, 3);

// --- CONFIG 2: Arduino Mega 2560 ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// SX1262 lora = new Module(10, 2, 9, 3);

// --- CONFIG 3: Teensy 4.1 ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
SX1262 lora = new Module(10, 2, 9, 3);

// Active pin definition tracking for the noise guard logic
const int PIN_DIO1 = 2; 

// ============================================================================

unsigned long lastActionTime = 0;
// Increased timeout to allow long flight-time packets room to breathe
const unsigned long TIMEOUT_MS = 12000; 

void setupLora() {
    pinMode(PIN_DIO1, INPUT);
    
    Serial.println(F("\n--- DX-PJ27 LoRa Hi/Bye Communication Node ---"));
    Serial.print(F("Device Role: "));
    Serial.println(IS_SENDER_NODE ? F("SENDER (Sends 'Hi')") : F("RECEIVER (Replies with 'Bye')"));
    
    Serial.print(F("[SX1262] Initializing ... "));
    
    // Initialize LoRa at 433.0 MHz (Change to 868.0 or 915.0 if needed)
    int state = lora.begin(433.0, 10.4, 8, 7, 0x12, 22, 8, false);
    
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
}

bool packetInRcvBuff() {
    return digitalRead(PIN_DIO1) == HIGH;
}

void printModulePacketMetadata(SX1262& lora) {
    Serial.print(F("    -> | RSSI: "));
    Serial.print(lora.getRSSI());
    Serial.print(F(" dBm | SNR: "));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));
}

bool send(const String& data_str) {
    return lora.transmit(data_str.c_str()) == RADIOLIB_ERR_NONE;
}

void handleSender(String& receivedStr) {
    Serial.println(F("\n[Sender] Sending next: Hi"));
    lastActionTime = millis();
    if (!send("Hi")) {
        Serial.println(F("Transmission failed"));
    }

    // FIX: Evaluate received data first before modifying transceiver state!
    if (receivedStr == "Bye") {
        Serial.print('.'); // Success indicator
    } else {
        Serial.print('#'); // Malformed/Unexpected text indicator
    }
}

void handleReceiver(String& receivedStr) {
    if (!send("Bye")) {
        Serial.println(F("Transmission failed"));
    }
    lastActionTime = millis();

    if (receivedStr == "Hi") {
        Serial.print('.'); // Success indicator
    } else {
        Serial.print('#');
    }
}

void resetLoraIfTimedOut() {
    if (IS_SENDER_NODE && (millis() - lastActionTime > TIMEOUT_MS)) {
        Serial.println(F("\n[Timeout] Initiating fresh handshake..."));
        if (!send("Hi")) {
            Serial.print(F("Kickstart transmit failed."));
        }
        lora.startReceive();
        lastActionTime = millis();
    }
}

String recvData() {
    if (!packetInRcvBuff()) {
        return "";
    }
    String receivedStr;
    int state = lora.readData(receivedStr);

    if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        return "";
    }

    if (state != RADIOLIB_ERR_NONE) {
        Serial.print(F("Receive logic encountered error: "));
        Serial.println(state);
        return "";
    }

    if (receivedStr.length() > 0) {
        Serial.println("\nData recvd: \"" + receivedStr + "\"");
        printModulePacketMetadata(lora);
        return receivedStr;
    }

    return "";
}

void sendAndRcvCycle() {
    String receivedStr = recvData();

    // Check string length to bypass empty noise glitches safely
    if (receivedStr.length() > 0) {
    }
    if (IS_SENDER_NODE) {
        handleSender(receivedStr);
    } else {
        handleReceiver(receivedStr);
    }
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    setupLora();
    lastActionTime = millis();
}

void loop() {
    resetLoraIfTimedOut();

    sendAndRcvCycle();

    // put back in recv mode to listen for packets
    lora.startReceive();
}
