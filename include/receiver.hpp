#pragma once
#include <Arduino.h>
#include <RadioLib.h>
#include "utils.hpp"

bool receiver_loop(SX1262& lora) {
    if (!packet_in_rcv_buff()) {
        return false;
    }

    String received_str = recv_data(lora);
    
    if (received_str.length() == 0) {
        lora.startReceive();
        return false;
    }

    // Serial.println("received msg: \"" + received_str + F("\""));
    // print_module_packet_metadata(lora);
    bool connected = false;

    if (received_str.startsWith(sender_auth_key)) {
        connected = true;
        Serial.print('.');

        bool fired_this_loop = false;

        if (received_str.endsWith(fire_cmd)) {
            fire();
            fired_this_loop = true;
        }

        delay(100);
        String response = receiver_auth_key;
        if (fired_this_loop) {
            String confrim_fired_state = digitalRead(fire_pin) ? confirm_fired : confirm_released;
            response += confrim_fired_state;
        }

        if (!send(lora, response)) {
            Serial.println(F("Failed to send response"));
        }
    }
    else {
        Serial.print('#');
    }

    lora.startReceive();
    return connected;
}
