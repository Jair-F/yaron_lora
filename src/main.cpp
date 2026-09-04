#include <Arduino.h>
#include <RadioLib.h>

// #define IS_SENDER

#ifdef IS_SENDER
#include "sender.hpp"
#else
#include "receiver.hpp"
#endif

const String authKey = "k��,�ˍI�";

// ============================================================================
// ROLE & TIMING SELECTION 
// ============================================================================
const bool IS_SENDER_NODE = true;

// --- CONFIG 1: Arduino Uno / Nano ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// SX1262 lora = new Module(10, 2, 9, 3);

// --- CONFIG 2: Arduino Mega 2560 ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
#ifdef IS_SENDER
SX1262 lora = new Module(10, 2, 9, 3);
#else

// --- CONFIG 3: Teensy 4.1 ---
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
SX1262 lora = new Module(10, 2, 9, 3);
#endif


void setup() {
    Serial.begin(9600);
    // while (!Serial);

    Serial.print(F("Device Role: "));
    Serial.println(IS_SENDER_NODE ? F("SENDER (Periodic)") : F("RECEIVER (Reactive)"));
    setupLora(lora);
}

void loop() {
    #ifdef IS_SENDER
    senderLoop(lora);
    #else
    receiverLoop(lora);
    #endif
}
