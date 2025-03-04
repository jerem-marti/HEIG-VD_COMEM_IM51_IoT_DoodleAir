#include "IMUManager.h"

IMUManager::IMUManager() : bufferIndex(0) {}

void IMUManager::begin() {
    Wire.begin();
    delay(100);  // Délai pour stabiliser l'I2C

    int maxAttempts = 5;  // Nombre maximum de tentatives
    int attempt = 0;

    while (attempt < maxAttempts) {
        myICM.begin(Wire, 0x68);  // Tente d'initialiser l'IMU
        
        if (myICM.status == ICM_20948_Stat_Ok) {
            Serial.println("✅ IMU détecté à l'adresse 0x68 !");
            return;
        } 

        Serial.println(F("⚠️ IMU non détecté, nouvelle tentative..."));
        delay(500);  // Pause entre chaque tentative
        attempt++;
    }

    Serial.println("❌ Échec : IMU non détecté après plusieurs tentatives !");
}

String IMUManager::readData() {
    if (myICM.dataReady()) {
        myICM.getAGMT();
        
        float accX = filter(accXBuffer, myICM.accX());
        float accY = filter(accYBuffer, myICM.accY());
        float accZ = filter(accZBuffer, myICM.accZ());
        float gyrX = filter(gyrXBuffer, myICM.gyrX());
        float gyrY = filter(gyrYBuffer, myICM.gyrY());
        float gyrZ = filter(gyrZBuffer, myICM.gyrZ());

        return "{\"accelX\":" + String(accX) +
               ",\"accelY\":" + String(accY) +
               ",\"accelZ\":" + String(accZ) +
               ",\"gyroX\":" + String(gyrX) +
               ",\"gyroY\":" + String(gyrY) +
               ",\"gyroZ\":" + String(gyrZ) + "}";
    }
    return "";
}

float IMUManager::filter(float* buffer, float newValue) {
    buffer[bufferIndex] = newValue;
    bufferIndex = (bufferIndex + 1) % 5;

    float sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += buffer[i];
    }
    return sum / 5;
}
