#include <Arduino.h>
#include <RadioLib.h>

#include "utils.hpp"

void receiverLoop(SX1262& lora) {
    if (!packetInRcvBuff()) {
        return;
    }

    String receivedStr = recvData(lora);
    
    if (receivedStr.length() == 0) {
        lora.startReceive();
        return;
    }

    if (receivedStr == "Hi") {
        Serial.print('.');
        printModulePacketMetadata(lora);

        delay(100);
        if (!send(lora, "Bye")) {
            Serial.println("Failed to send response");
        }
    } else {
        Serial.print('#');
    }

    lora.startReceive();
}
