#include <Arduino.h>
#include <RadioLib.h>

#define IS_SENDER

#ifdef IS_SENDER
#include "sender.hpp"
#else
#include "receiver.hpp"
#endif

const bool IS_SENDER_NODE = true;

// Uno / Nano
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
SX1262 lora = new Module(10, 2, 9, 3);

// Mega 2560
// NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// #ifdef IS_SENDER
// SX1262 lora = new Module(10, 2, 9, 3);
// #else

// // Teensy 4.1
// // NSS: 10, DIO1: 2, NRST: 9, BUSY: 3
// SX1262 lora = new Module(10, 2, 9, 3);
// #endif


void setup() {
    Serial.begin(9600);

    pinMode(trigger_out_pin, OUTPUT);
    digitalWrite(trigger_out_pin, LOW);
    pinMode(btn_pin, INPUT_PULLUP);
    pinMode(connection_status_led, OUTPUT);
    digitalWrite(connection_status_led, LOW);

    #ifdef IS_SENDER
    for(uint8_t i = 0; i < 2; ++i) {
        digitalWrite(connection_status_led, HIGH);
        delay(1000);
        digitalWrite(connection_status_led, LOW);
        delay(1000);
    }
    #endif

    Serial.print(F("Device Role: "));
    Serial.println(IS_SENDER_NODE ? F("SENDER (Periodic)") : F("RECEIVER (Reactive)"));
    setup_lora(lora);
}

void loop() {
    bool connection_status = false;

    #ifdef IS_SENDER
    connection_status = senderLoop(lora);
    #else
    if (!receiver_loop(lora)) {
        Serial.print(F("#"));
        delay(3000);
        connection_status = true;
    }
    #endif

    if (connection_status) {
        digitalWrite(connection_status_led, HIGH);
    }
    else {
        digitalWrite(connection_status_led, LOW);
    }
}
