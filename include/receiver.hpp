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

    if (received_str.startsWith(sender_auth_key)) {
        ret = true;
        Serial.print('.');

        if (received_str.endsWith(fire_cmd)) {
            fire();
        }

        delay(100);
        if (!send(lora, button_pressed() ? receiver_auth_key + fire_cmd  : receiver_auth_key)) {
            Serial.println("Failed to send response");
        }
    } else {
        Serial.print('#');
    }

    lora.startReceive();
    return ret;
}
