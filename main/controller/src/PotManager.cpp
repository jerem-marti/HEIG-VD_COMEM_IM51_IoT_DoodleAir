#include "PotManager.h"

PotManager::PotManager(int p) : pin(p), filteredValue(analogRead(p)) {}

void PotManager::begin() {
    pinMode(pin, INPUT);
}

// 🔹 Filtrage exponentiel pour une réponse rapide avec un minimum de bruit
String PotManager::readData() {
    int rawValue = analogRead(pin);
    filteredValue = alpha * rawValue + (1 - alpha) * filteredValue;

    return "{\"potentiometer\": " + String(filteredValue, 2) + "}"; // 2 décimales
}
