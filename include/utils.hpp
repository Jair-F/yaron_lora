#include <Arduino.h>
#include <RadioLib.h>

const int PIN_DIO1 = 2; 

bool packetInRcvBuff() {
    return digitalRead(PIN_DIO1) == HIGH;
}

bool send(SX1262& lora, const String& data_str) {
    return lora.transmit(data_str.c_str()) == RADIOLIB_ERR_NONE;
}

String recvData(SX1262& lora) {
    String receivedStr;
    int state = lora.readData(receivedStr);

    if (state != RADIOLIB_ERR_NONE) {
        return "";
    }

    if (receivedStr.length() > 0) {
        return receivedStr;
    }
    return "";
}

void printModulePacketMetadata(SX1262& lora) {
    Serial.print(F("    -> | RSSI: "));
    Serial.print(lora.getRSSI());
    Serial.print(F(" dBm | SNR: "));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));
}

void setupLora(SX1262& lora) {
    pinMode(PIN_DIO1, INPUT);
    
    Serial.println(F("\n--- DX-PJ27 LoRa Periodic Handshake Node ---"));
    
    Serial.print(F("[SX1262] Initializing ... "));
    int state = lora.begin(433.0, 10.4, 8, 7, 0x12, 22, 8, false);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("Initialization successful!"));
    } else {
        Serial.print(F("Initialization failed, code "));
        Serial.println(state);
        while (true);
    }
    
    lora.startReceive();
}
