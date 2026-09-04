#include <Arduino.h>
#include <RadioLib.h>

#include "utils.hpp"

bool receiverLoop(SX1262& lora) {
    if (!packetInRcvBuff()) {
        return false;
    }

    String receivedStr = recvData(lora);
    
    if (receivedStr.length() == 0) {
        lora.startReceive();
        return false;
    }

    Serial.print("received msg: \"");
    Serial.println(receivedStr + '\"');

    bool ret = false;

    if (receivedStr == sender_auth_key) {
        ret = true;
        Serial.print('.');
        printModulePacketMetadata(lora);

        delay(100);
        if (!send(lora, receiver_auth_key)) {
            Serial.println("Failed to send response");
        }
    } else {
        Serial.print('#');
    }

    lora.startReceive();
    return ret;
}
