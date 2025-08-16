#ifndef MAIN_H_
#define MAIN_H_

#include <Arduino.h>

#ifdef DEBUG
  #define PTVO_CALLBACK_DEBUG(endpoint, isWrite, value) ({\
    Serial.print(F(endpoint));\
    Serial.print(isWrite ? F("(write)") : F("(read)"));\
    Serial.println(value);\
  })
  #define SETTINGS_DEBUG(settings) ({\
    Serial.print(F("Settings("));\
    Serial.print(F("minDistanceValue: "));\
    Serial.print(settings.minDistanceValue);\
    Serial.print(F(", maxDistanceValue: "));\
    Serial.print(settings.maxDistanceValue);\
    Serial.println(F(")"));\
  })
#else
  #define PTVO_CALLBACK_DEBUG(endpoint, isWrite, value)
  #define SETTINGS_DEBUG(settings)
#endif

// Pin definitions for XIAO ESP32C6
#define LED_PIN                     21  // Built-in LED on XIAO ESP32C6
#define BUTTON_PIN                  0   // Boot button
#define I2C_SDA_PIN                 4   // I2C SDA
#define I2C_SCL_PIN                 5   // I2C SCL
#define UART_RX_PIN                 20  // UART RX for ultrasonic sensor
#define UART_TX_PIN                 21  // UART TX for ultrasonic sensor

// Zigbee endpoint definitions (matching PTVO configuration)
#define ENDPOINT_DISPLAY_STATE      3
#define ENDPOINT_WATER_LEVEL_VALUE  4
#define ENDPOINT_RAW_DISTANCE_VALUE 5
#define ENDPOINT_MIN_DISTANCE_VALUE 6
#define ENDPOINT_MAX_DISTANCE_VALUE 7

// Timing constants
#define BLINK_DURATION              100   // ms
#define DISPLAY_ON_DURATION         30000 // ms
#define SENSOR_READ_INTERVAL        1000  // ms
#define ZIGBEE_REPORT_INTERVAL      5000  // ms

/// @brief structure holding all zigbee tracked variables
struct Variables
{
  // measured water level (in percentage)
  float waterLevelValue;
  // measured raw distance (in cm)
  float rawDistanceValue;
  // min measurable distance (in cm), corresponding to when water level is at max (tank full)
  float minDistanceValue;
  // max measurable distance (in cm), corresponding to when water level is at min (tank empty)
  float maxDistanceValue;
  // control if display is on or off
  uint8_t displayState;
};

/// @brief preferences-saved structure (ESP32 uses Preferences instead of EEPROM)
struct Settings
{
  float minDistanceValue;
  float maxDistanceValue;
};

#endif // MAIN_H_
