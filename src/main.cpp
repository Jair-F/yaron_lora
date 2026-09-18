#include <Arduino.h>
#include <RadioLib.h>

#define IS_SENDER

#ifdef IS_SENDER
#include "sender.hpp"
#else
#include "receiver.hpp"
#endif

const bool IS_SENDER_NODE = true;


SX1262 lora = new Module(10, 2, 9, 3);

void btn_pressed() {
    btn_state.trigger();
    Serial.println(F("triggered btn"));
}

void setup() {
    Serial.begin(9600);

    pinMode(connection_status_led, OUTPUT);
    digitalWrite(connection_status_led, LOW);

    #ifdef IS_SENDER
    pinMode(btn_pin, INPUT_PULLUP);
    attachInterrupt(btn_pin, btn_pressed, FALLING);

    for(uint8_t i = 0; i < 2; ++i) {
        digitalWrite(connection_status_led, HIGH);
        delay(1000);
        digitalWrite(connection_status_led, LOW);
        delay(1000);
    }
    #else
    pinMode(fire_pin, OUTPUT);
    digitalWrite(fire_pin, LOW);
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
        Serial.print('#');
        delay(100);
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
