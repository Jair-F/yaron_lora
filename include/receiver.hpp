#include <Arduino.h>
#include <RadioLib.h>

#include "utils.hpp"

bool receiver_loop(SX1262& lora) {
    if (!packet_in_rcv_buff()) {
        return false;
    }

    String receivedStr = recv_data(lora);
    
    if (receivedStr.length() == 0) {
        lora.startReceive();
        return false;
    }

    Serial.print("received msg: \"");
    Serial.println(receivedStr + '\"');

    bool ret = false;

    if (receivedStr.startsWith(sender_auth_key)) {
        ret = true;
        Serial.print('.');
        print_module_packet_metadata(lora);

        if (receivedStr.endsWith(fire_cmd)) {
            fire();
        }

        delay(100);
        String send_msg = "";
        if (button_pressed()) {
            send_msg = receiver_auth_key + fire_cmd;
        }
        else {
            send_msg = sender_auth_key;
        }
        if (!send(lora, send_msg)) {
            Serial.println("Failed to send response");
        }
    } else {
        Serial.print('#');
    }

    lora.startReceive();
    return ret;
}
