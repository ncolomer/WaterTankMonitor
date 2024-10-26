#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

#include <Arduino.h>
#include <NeoSWSerial.h>
#include <CircularBuffer.h>

class Sensor {
private:
    NeoSWSerial serial;
    CircularBuffer buffer;
    uint32_t nextReadTimestamp = 0;
    float rawDistanceMm = 0;
public:
    Sensor(void);
    bool begin(void);
    float readDistanceCm(void);
};

#endif
