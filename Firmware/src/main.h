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

/// @brief structure holding all ptvo tracked variables
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

/// @brief eeprom-saved structure
struct Settings
{
  float minDistanceValue;
  float maxDistanceValue;
};
