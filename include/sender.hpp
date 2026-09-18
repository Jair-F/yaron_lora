#pragma once
#include <Arduino.h>
#include <RadioLib.h>
# include "utils.hpp"

const unsigned long TIMEOUT_MS = 5000;

void blink_to_confirm_fire() {
    for(uint8_t i = 0; i < 4; i++) {
        digitalWrite(connection_status_led, LOW);
        delay(250);
        digitalWrite(connection_status_led, HIGH);
        delay(250);
    }
}

void blink_to_confirm_release() {
    for(uint8_t i = 0; i < 4; i++) {
        digitalWrite(connection_status_led, LOW);
        delay(250);
        digitalWrite(connection_status_led, HIGH);
        delay(250);
    }
}

bool senderLoop(SX1262& lora) {
    auto start = millis();

    if (!send(lora, btn_state.triggered() ? sender_auth_key + fire_cmd : sender_auth_key)) {
        Serial.println("Failed to send Hi");
    }
    delay(100);
    lora.startReceive();

    while (!packet_in_rcv_buff() && (millis() - start) < TIMEOUT_MS) {
        continue;
    }

    String received_str = recv_data(lora);
    // Serial.println("received msg: \"" + received_str + "\"");
    // print_module_packet_metadata(lora);


    if (received_str.startsWith(receiver_auth_key)) {
        Serial.print('.');

        if(received_str.endsWith(confirm_fired)) {
            blink_to_confirm_fire();
        }
        else if(received_str.endsWith(confirm_released)) {
            blink_to_confirm_release();
        }

        btn_state.reset();

    } else {
        Serial.print('#');
        return false;
    }

    return true;
}
