#ifndef SRC_ZIGBEE_MANAGER_H_
#define SRC_ZIGBEE_MANAGER_H_

#include <Arduino.h>
#include <Preferences.h>

class ZigbeeManager {
private:
    Preferences preferences;
    bool isInitialized;
    uint32_t lastReportTime;
    
    // Zigbee cluster and attribute IDs
    static const uint16_t CLUSTER_BASIC = 0x0000;
    static const uint16_t CLUSTER_POWER_CONFIG = 0x0001;
    static const uint16_t CLUSTER_ANALOG_INPUT = 0x000C;
    static const uint16_t CLUSTER_BINARY_INPUT = 0x000F;
    
    // Attribute IDs
    static const uint16_t ATTR_MODEL_ID = 0x0005;
    static const uint16_t ATTR_MANUFACTURER_NAME = 0x0004;
    static const uint16_t ATTR_APPLICATION_VERSION = 0x0001;
    
public:
    ZigbeeManager(void);
    bool begin(void);
    void process(void);
    
    // Variable management (replacing PTVO functionality)
    void setWaterLevel(float percentage);
    void setRawDistance(float distance);
    void setDisplayState(bool state);
    void setMinDistance(float distance);
    void setMaxDistance(float distance);
    
    // Configuration
    void setMinDistanceConfig(float distance);
    void setMaxDistanceConfig(float distance);
    float getMinDistanceConfig(void);
    float getMaxDistanceConfig(void);
    
    // Status
    bool isConnected(void);
    void reportAllValues(void);
};

#endif
