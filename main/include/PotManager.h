#ifndef POT_MANAGER_H
#define POT_MANAGER_H

#include <Arduino.h>

class PotManager {
public:
    PotManager(int pin);
    void begin();
    String readData();

private:
    int pin;
    float alpha = 0.8;  // Facteur de lissage (0.1 = très lisse, 0.5 = réactif)
    float filteredValue = 0;
};

#endif
