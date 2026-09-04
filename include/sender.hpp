#include <Arduino.h>
#include <RadioLib.h>

# include "utils.hpp"

// Send msg and wait for this much time for an response.
const unsigned long PERIOD_MS = 5000;

bool senderLoop(SX1262& lora) {
    auto start = millis();

    if (!send(lora, button_pressed() ? sender_auth_key + fire_cmd : sender_auth_key)) {
        Serial.println("Failed to send Hi");
    }
    delay(100);
    lora.startReceive();

    while (!packet_in_rcv_buff() && (millis() - start) < PERIOD_MS) {
        continue;
    }

    String received_str = recv_data(lora);
    // Serial.println("received msg: \"" + received_str + "\"");
    // print_module_packet_metadata(lora);


    if (received_str.startsWith(receiver_auth_key)) {
        Serial.print('.');

        if(received_str.endsWith(fire_cmd)) {
            fire();
        }
    } else {
        Serial.print('#');
        return false;
    }

    return true;
}
