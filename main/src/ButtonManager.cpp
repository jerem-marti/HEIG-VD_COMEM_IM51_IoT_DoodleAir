#include "ButtonManager.h"

ButtonManager::ButtonManager(int pColor, int pErase)
    : pinColor(pColor), pinErase(pErase), lastStateColor(HIGH), lastStateErase(HIGH) {}

void ButtonManager::begin() {
    pinMode(pinColor, INPUT);
    pinMode(pinErase, INPUT);
}

void ButtonManager::update(WSClient& wsClient) {
    bool stateColor = digitalRead(pinColor);
    bool stateErase = digitalRead(pinErase);

    if (stateColor != lastStateColor) {
        wsClient.send("{\"button\": \"color\", \"state\": \"" + String(stateColor ? "released" : "pressed") + "\"}");
        lastStateColor = stateColor;
    }
    if (stateErase != lastStateErase) {
        wsClient.send("{\"button\": \"erase\", \"state\": \"" + String(stateErase ? "released" : "pressed") + "\"}");
        lastStateErase = stateErase;
    }
}
