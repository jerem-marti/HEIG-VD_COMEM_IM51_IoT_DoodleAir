#ifndef IMU_MANAGER_H
#define IMU_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "ICM_20948.h"

class IMUManager {
public:
    IMUManager();
    void begin();
    String readMotion();

private:
    ICM_20948_I2C myICM;
    
    float rollOffset, pitchOffset, yawOffset;
    float lastAccelY, lastAccelZ;
    float velocityX, velocityY;
    float previousTime;

    void calibrate();
    float integrateAcceleration(float accel, float lastAccel, float& velocity);
};

#endif
