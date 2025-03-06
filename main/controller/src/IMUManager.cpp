#include "IMUManager.h"

IMUManager::IMUManager() : rollOffset(0), pitchOffset(0), velocityX(0), velocityY(0), previousTime(0) {}

void IMUManager::begin() {
    Wire.begin();
    delay(100);

    myICM.begin(Wire, 0x68);
    if (myICM.status != ICM_20948_Stat_Ok) {
        Serial.println("⚠️ IMU non détecté !");
        return;
    }

    Serial.println("✅ IMU détecté !");
    calibrate();
}

void IMUManager::calibrate() {
    Serial.println("🔄 Calibration IMU...");
    rollOffset = 0;
    pitchOffset = 0;
    for (int i = 0; i < 100; i++) {
        myICM.getAGMT();
        rollOffset += atan2(myICM.accY(), myICM.accZ()) * RAD_TO_DEG;
        pitchOffset += atan2(myICM.accX(), myICM.accZ()) * RAD_TO_DEG;
        yawOffset += atan2(myICM.accX(), myICM.accY()) * RAD_TO_DEG;
        delay(10);
    }
    rollOffset /= 100;
    pitchOffset /= 100;
    Serial.println("🔄 Roll offset : " + String(rollOffset));
    Serial.println("🔄 Pitch offset : " + String(pitchOffset));
    Serial.println("🔄 Yaw offset : " + String(yawOffset));
    Serial.println("✅ Calibration terminée !");
}

float IMUManager::integrateAcceleration(float accel, float lastAccel, float& velocity) {
    float dt = (millis() - previousTime) / 1000.0;  // Temps écoulé en secondes
    previousTime = millis();

    float accelAvg = (accel + lastAccel) / 2.0;  // Moyenne des deux valeurs
    velocity += accelAvg * dt;  // Intégration de l'accélération
    return velocity * dt;  // Conversion en déplacement
}

String IMUManager::readMotion() {
    if (myICM.dataReady()) {
        myICM.getAGMT();

        float accX = myICM.accX();
        float accY = myICM.accY();
        float accZ = myICM.accZ();
        Serial.println("🚀 Accélération X : " + String(accX) + " m/s²");
        Serial.println("🚀 Accélération Y : " + String(accY) + " m/s²");
        Serial.println("🚀 Accélération Z : " + String(accZ) + " m/s²");

        float roll = atan2(myICM.accY(), myICM.accZ()) * RAD_TO_DEG;
        float pitch = atan2(myICM.accX(), myICM.accZ()) * RAD_TO_DEG;
        float yaw = atan2(myICM.accX(), myICM.accY()) * RAD_TO_DEG;
        Serial.println("📐 Roll : " + String(roll) + "°");
        Serial.println("📐 Pitch : " + String(pitch) + "°");
        Serial.println("📐 Yaw : " + String(yaw) + "°");

        float deltaX = integrateAcceleration(accY, lastAccelY, velocityX);
        float deltaY = integrateAcceleration(accZ, lastAccelZ, velocityY);

        lastAccelY = accY;
        lastAccelZ = accZ;

        return "{\"accX\":" + String(accX) + ",\"accY\":" + String(accY) + ",\"accZ\":" + String(accZ) + ",\"roll\":" + String(roll) + ",\"pitch\":" + String(pitch) + ",\"yaw\":" + String(yaw) + ",\"deltaX\":" + String(deltaX) + ",\"deltaY\":" + String(deltaY) + "}";
    }
    return "";
}
