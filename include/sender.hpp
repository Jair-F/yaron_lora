#include <Arduino.h>
#include <RadioLib.h>

# include "utils.hpp"

// Send msg and wait for this much time for an response.
const unsigned long PERIOD_MS = 5000;

bool senderLoop(SX1262& lora) {
    auto start = millis();

    if (!send(lora, sender_auth_key)) {
        Serial.println("Failed to send Hi");
    }
    delay(100);
    lora.startReceive();

    while (!packetInRcvBuff() && (millis() - start) < PERIOD_MS) {
        continue;
    }

    String receivedStr = recvData(lora);

    if (receivedStr == receiver_auth_key) {
        Serial.print('.');
        printModulePacketMetadata(lora);
    } else {
        Serial.print('#');
    }

    // long remaining_sleep = PERIOD_MS - (millis() - start);
    // delay(max(0, remaining_sleep));

    return true;
}
