#include <Arduino.h>
#include "IMUManager.h"
#include "WSClient.h"
#include "ButtonManager.h"
#include "PotManager.h"

// 🔹 Objets globaux
IMUManager imu;
WSClient wsClient("192.168.15.13", 8080);
ButtonManager buttons(26, 36);
PotManager potentiometer(34);

void setup() {
    Serial.begin(115200);

    // 🔹 Initialisation des composants
    wsClient.begin("J", "DEUX * deux = 4");
    imu.begin();
    buttons.begin();
    potentiometer.begin();

    Serial.println("✅ ESP32 prêt !");
}

void loop() {
    wsClient.checkConnection();
    wsClient.poll();

    // 🔹 IMU : Lecture et envoi toutes les 500ms
    static unsigned long lastIMUSendTime = 0;
    if (millis() - lastIMUSendTime > 100) {
        lastIMUSendTime = millis();
        String imuData = imu.readMotion();
        wsClient.send(imuData);
    }

    // 🔹 Potentiomètre : Lecture et envoi toutes les 500ms
    static unsigned long lastPotSendTime = 0;
    if (millis() - lastPotSendTime > 100) { // 🔥 Envoi 10x par seconde
        lastPotSendTime = millis();
        String potData = potentiometer.readData();
        wsClient.send(potData);
    }

    // 🔹 Boutons : Vérification et envoi en cas de changement
    buttons.update(wsClient);
    
    delay(50);
}
