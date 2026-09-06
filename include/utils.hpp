#include <Arduino.h>
#include <RadioLib.h>

constexpr uint8_t connection_status_led = 13;
constexpr uint8_t btn_pin = 5;
constexpr uint8_t trigger_out_pin = 4;
constexpr uint8_t PIN_DIO1 = 2;
const String sender_auth_key = "XO*@#*SJ9";
const String receiver_auth_key = "WOK*($@43^";
const String fire_cmd = "_fire";

bool button_pressed() {
    bool state = digitalRead(btn_pin) == LOW;
    if (state) {
        Serial.println("triggered btn");
    }
    return state;
}

void fire() {
    Serial.println("firing...");
    digitalWrite(trigger_out_pin, HIGH);
    delay(1000);
    digitalWrite(trigger_out_pin, LOW);
}

bool packet_in_rcv_buff() {
    return digitalRead(PIN_DIO1) == HIGH;
}

bool send(SX1262& lora, const String& data_str) {
    return lora.transmit(data_str.c_str()) == RADIOLIB_ERR_NONE;
}

String recv_data(SX1262& lora) {
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

void print_module_packet_metadata(SX1262& lora) {
    Serial.print(F("    -> | RSSI: "));
    Serial.print(lora.getRSSI());
    Serial.print(F(" dBm | SNR: "));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));
}

void setup_lora(SX1262& lora) {
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
