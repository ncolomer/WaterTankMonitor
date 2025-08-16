#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

#include <Arduino.h>
#include <HardwareSerial.h>

class Sensor {
private:
    HardwareSerial* serial;
    uint32_t nextReadTimestamp = 0;
    float rawDistanceMm = 0;
    bool isInitialized = false;
    
public:
    Sensor(void);
    bool begin(void);
    float readDistanceCm(void);
    bool isReady(void) { return isInitialized; }
};

#endif
