#include <Arduino.h>
#include <RadioLib.h>

# include "utils.hpp"

// Send msg and wait for this much time for an response.
const unsigned long PERIOD_MS = 5000;

bool senderLoop(SX1262& lora) {
    auto start = millis();

    String send_msg = "";
    if (button_pressed()) {
        send_msg = sender_auth_key + fire_cmd;
    }
    else {
        send_msg = sender_auth_key;
    }

    Serial.print("sending: ");
    Serial.println(send_msg);
    if (!send(lora, send_msg)) {
        Serial.println("Failed to send Hi");
    }
    delay(100);
    lora.startReceive();

    while (!packet_in_rcv_buff() && (millis() - start) < PERIOD_MS) {
        continue;
    }

    String received_str = recv_data(lora);

    if (received_str.startsWith(receiver_auth_key)) {
        Serial.print('.');
        print_module_packet_metadata(lora);

        Serial.print("received: ");
        Serial.println(received_str);
        if(received_str.endsWith(fire_cmd)) {
            fire();
        }
    } else {
        Serial.print('#');
    }

    // long remaining_sleep = PERIOD_MS - (millis() - start);
    // delay(max(0, remaining_sleep));

    return true;
}
