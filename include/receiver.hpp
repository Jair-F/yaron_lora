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

    // Serial.println("received msg: \"" + received_str + "\"");
    // print_module_packet_metadata(lora);
    bool ret = false;
    bool fired_this_loop = false;

    if (received_str.startsWith(sender_auth_key)) {
        ret = true;
        Serial.print('.');

        if (received_str.endsWith(fire_cmd)) {
            fire();
            fired_this_loop = true;
        }

        delay(100);
        String confrim_cmd = digitalRead(trigger_out_pin) ? confirm_fired : confirm_released;
        String send_cmd = fired_this_loop ? receiver_auth_key + confrim_cmd  : receiver_auth_key;

        if (!send(lora, send_cmd)) {
            Serial.println("Failed to send response");
        }
    } else {
        Serial.print('#');
    }

    lora.startReceive();
    return ret;
}
