#include <Arduino.h>
#include <RadioLib.h>

# include "utils.hpp"

// The fixed interval (in milliseconds) at which the Sender drops a "Hi" message
const unsigned long PERIOD_MS = 3000;

void senderLoop(SX1262& lora) {
    auto start = millis();

    if (!send(lora, "Hi")) {
        Serial.println("Failed to send Hi");
    }
    
    // --- PACKET PROCESSING GUARD ---
    while (!packetInRcvBuff() && (millis() - start) < PERIOD_MS) {
        continue;
    }


    String receivedStr = recvData(lora);
    Serial.print("received packet: \"");
    Serial.println(receivedStr + "\"");

    if (receivedStr.length() > 0) {
        if (receivedStr == "Bye") {
            Serial.print('.');
            printModulePacketMetadata(lora);
        } else {
            Serial.print('#');
        }
    }

    lora.startReceive(); // Return to listen mode to collect any reactive response

    auto remaining_sleep = millis() - start;
    delay(max(0, remaining_sleep));
}
