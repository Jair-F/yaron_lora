#include <Arduino.h>
#include <RadioLib.h>

// ============================================================================
// ROLE SELECTION 
// ============================================================================
// Set to true for Device 1 (Sends "Hi", waits for "Bye")
// Set to false for Device 2 (Waits for "Hi", replies with "Bye")
const bool IS_SENDER_NODE = true; 

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

// Active pin definition tracking for the noise guard logic
const int PIN_DIO1 = 2; 

// ============================================================================

unsigned long lastActionTime = 0;
const unsigned long TIMEOUT_MS = 6000; 

void setupLora() {
    // Configure the DIO1 hardware pin as an input
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

bool loarReadyToInteract() {
    return digitalRead(PIN_DIO1) == HIGH;
}

void printModulePacketMetadata(SX1262& lora) {
    Serial.print(F("\" | RSSI: "));
    Serial.print(lora.getRSSI());
    Serial.print(F(" dBm | SNR: "));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));
}

void handleSender(String& receivedStr) {
    if (receivedStr == "Bye") {
        Serial.println(F("[Success] Handshake completed successfully!"));
        delay(2000); // Wait 2 seconds before starting the message cycle over
        
        Serial.println(F("[SX1262] Transmitting: Hi"));
        int txState = lora.transmit("Hi");
        if (txState != RADIOLIB_ERR_NONE) {
            Serial.print(F("Transmission failed, code "));
            Serial.println(txState);
        }
        lastActionTime = millis();
    }
}

void handleReceiver(String& receivedStr) {
    if (receivedStr == "Hi") {
        delay(500); // Brief delay to let the sender node open its listening window
        Serial.println(F("[SX1262] Transmitting response: Bye"));
        
        int txState = lora.transmit("Bye");
        if (txState != RADIOLIB_ERR_NONE) {
            Serial.print(F("Transmission failed, code "));
            Serial.println(txState);
        }
        lastActionTime = millis();
    }
}

void resetLoraIfTimedOut() {
    // --- TIMEOUT KICKSTART LOGIC ---
    if (IS_SENDER_NODE && (millis() - lastActionTime > TIMEOUT_MS)) {
        Serial.println(F("[Timeout] Silence detected. Initiating handshake..."));
        
        Serial.println(F("[SX1262] Transmitting: Hi"));
        int txState = lora.transmit("Hi");
        if (txState == RADIOLIB_ERR_NONE) {
            Serial.println(F("[SX1262] Initial 'Hi' sent successfully."));
        } else {
            Serial.print(F("Kickstart transmit failed, code "));
            Serial.println(txState);
        }
        
        lora.startReceive();
        lastActionTime = millis();
    }
}

void sendAndRcvCycle() {
    String receivedStr;

    // Check and read packet details from internal hardware buffer
    int state = lora.readData(receivedStr);

    if (state == RADIOLIB_ERR_NONE) {
        // Secondary filter to ensure no empty/corrupted noise slices make it past
        if (receivedStr.length() > 0) {
            Serial.print(F("[SX1262] Valid packet received! Data: \""));
            Serial.print(receivedStr);
            printModulePacketMetadata(lora);
            
            if (IS_SENDER_NODE) {
                handleSender(receivedStr);
            } 
            else {
                handleReceiver(receivedStr);
            }
        }
    } 
    else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
        Serial.print(F("Receive logic encountered error: "));
        Serial.println(state);
    }
    
    // Always reset the receiver window state and clear register interrupt bits
    lora.startReceive();
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    setupLora();
    lastActionTime = millis();
}

void loop() {
    resetLoraIfTimedOut();
    
    // SPAM PREVENTER GUARD: Only interact with the library if DIO1 is actively driven HIGH
    if (!loarReadyToInteract()) {
        return;
    }

    sendAndRcvCycle();
}
