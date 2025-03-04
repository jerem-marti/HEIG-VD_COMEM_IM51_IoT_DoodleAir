#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "WSClient.h"

class ButtonManager {
public:
    ButtonManager(int pinColor, int pinErase);
    void begin();
    void update(WSClient& wsClient);

private:
    int pinColor, pinErase;
    bool lastStateColor, lastStateErase;
};

#endif
