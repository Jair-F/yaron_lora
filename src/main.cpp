#include <Arduino.h>
#include <RadioLib.h>

// ============================================================================
// ROLE & TIMING SELECTION 
// ============================================================================
const bool IS_SENDER_NODE = true;

// The fixed interval (in milliseconds) at which the Sender drops a "Hi" message
const unsigned long PERIOD_MS = 3000; 

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

const int PIN_DIO1 = 2; 

// ============================================================================

unsigned long lastActionTime = 0;
bool gotReplyThisCycle = false;

void setupLora() {
    pinMode(PIN_DIO1, INPUT);
    
    Serial.println(F("\n--- DX-PJ27 LoRa Periodic Handshake Node ---"));
    Serial.print(F("Device Role: "));
    Serial.println(IS_SENDER_NODE ? F("SENDER (Periodic)") : F("RECEIVER (Reactive)"));
    
    Serial.print(F("[SX1262] Initializing ... "));
    int state = lora.begin(433.0, 10.4, 8, 7, 0x12, 22, 8, false);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("Initialization successful!"));
    } else {
        Serial.print(F("Initialization failed, code "));
        Serial.println(state);
        while (true);
    }
    
    lora.startReceive();
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
    if (receivedStr == "Bye") {
        gotReplyThisCycle = true;
        Serial.print('.'); // Connected indicator
    } else {
        Serial.print('#'); // Unexpected packet payload
    }
}

void handleReceiver(String& receivedStr) {
    if (receivedStr == "Hi") {
        Serial.println(F("\n[Receiver] Status: Connected! Received 'Hi'."));
        printModulePacketMetadata(lora);
        
        delay(100); // Tiny turn-around window offset
        Serial.println(F("[Receiver] Sending reply: Bye"));
        send("Bye");
    } else {
        Serial.print('#');
    }
}

String recvData() {
    String receivedStr;
    int state = lora.readData(receivedStr);

    if (state != RADIOLIB_ERR_NONE) {
        return "";
    }

    if (receivedStr.length() > 0) {
        return receivedStr;
    }
    return "";
}

void sendAndRcvCycle() {
    String receivedStr = recvData();

    if (receivedStr.length() > 0) {
        if (IS_SENDER_NODE) {
            handleSender(receivedStr);
        } else {
            handleReceiver(receivedStr);
        }
    }
    lora.startReceive();
}

void setup() {
    Serial.begin(9600);
    // while (!Serial);

    setupLora();
    lastActionTime = millis();
}

void loop() {
    // --- SENDER PERIODIC TRANSMISSION LOGIC ---
    if (IS_SENDER_NODE && (millis() - lastActionTime >= PERIOD_MS)) {
        // Evaluate the previous cycle results before sending the next one
        if (lastActionTime > 0) { 
            if (gotReplyThisCycle) {
                Serial.println(F("\n[Sender] Status: Connected! (Receiver acknowledged)"));
            } else {
                Serial.println(F("\n[Sender] Status: Disconnected! (No reply received)"));
            }
        }

        Serial.println(F("[Sender] Period elapsed. Transmitting: Hi"));
        send("Hi");
        
        gotReplyThisCycle = false; // Reset flag for the new cycle
        lora.startReceive();       // Return to listen mode to collect any reactive response
        lastActionTime = millis();
    }

    // --- PACKET PROCESSING GUARD ---
    if (!packetInRcvBuff()) {
        return;
    }

    sendAndRcvCycle();
}
