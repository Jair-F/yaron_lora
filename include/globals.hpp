#pragma once
#include <Arduino.h>
#include "latch_btn.hpp"

constexpr uint8_t connection_status_led = 7;
constexpr uint8_t btn_pin = 5;
constexpr uint8_t fire_pin = 4;
constexpr uint8_t PIN_DIO1 = 2;
const String sender_auth_key = "XO*@#*SJ9";
const String receiver_auth_key = "WOK*($@43^";
const String fire_cmd = "_trig";
const String confirm_fired = "_fireConf";
const String confirm_released = "_relConf";

LatchButton btn_state;
