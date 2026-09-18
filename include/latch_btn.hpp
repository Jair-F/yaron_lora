#pragma once
#include <Arduino.h>


class LatchButton {
private:
    bool _state = false;

public:
    void reset() {
        this->_state = false;
    }

    void trigger() {
        this->_state = true;
    }

    bool triggered() const {
        return this->_state;
    }
};

