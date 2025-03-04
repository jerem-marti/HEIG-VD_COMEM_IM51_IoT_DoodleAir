#ifndef IMU_MANAGER_H
#define IMU_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "ICM_20948.h"  // Bibliothèque du capteur IMU

class IMUManager {
public:
    IMUManager();
    void begin();
    String readData();

private:
    ICM_20948_I2C myICM;
    float filter(float* buffer, float newValue);
    float accXBuffer[5], accYBuffer[5], accZBuffer[5];
    float gyrXBuffer[5], gyrYBuffer[5], gyrZBuffer[5];
    int bufferIndex;
};

#endif
