#include <Arduino.h>
#include <EEPROM.h>

#include <EnableInterrupt.h>
#include <extensions/PtvoUartSensor.h>

#include <Display.h>
#include <Sensor.h>
#include <main.h>

#define LED_PIN                     9 // PB1
#define INT_PIN                     14 // PC0 / PCINT8

#define SETTINGS_EEPROM_ADDRESS     0

#define BLINK_DURATION              100 // ms
#define DISPLAY_ON_DURATION         30000 // ms

#define ENDPOINT_DISPLAY_STATE      ENDPOINT_3
#define ENDPOINT_WATER_LEVEL_VALUE  ENDPOINT_4
#define ENDPOINT_RAW_DISTANCE_VALUE ENDPOINT_5
#define ENDPOINT_MIN_DISTANCE_VALUE ENDPOINT_6
#define ENDPOINT_MAX_DISTANCE_VALUE ENDPOINT_7

PDTL::PtvoUartSensor<5, 5> ptvo;
Display display;
Sensor sensor;

Settings settings;
Variables variables;

uint32_t turnOffLedTimestamp = 0;

uint32_t turnOffDisplayTimestamp = 0;
bool displayStateChanged = false;

void BlinkLed()
{
  digitalWrite(LED_PIN, HIGH);
  turnOffLedTimestamp = millis() + BLINK_DURATION;
}

void TurnOnDisplay()
{
  turnOffDisplayTimestamp = millis() + DISPLAY_ON_DURATION;
  variables.displayState = 1;
  displayStateChanged = true;
}

void PtvoCallback(PDTL::VariableData &variableInfo, bool isWrite)
{
  BlinkLed();
  switch (variableInfo.m_uId)
  {
  case ENDPOINT_WATER_LEVEL_VALUE:
    PTVO_CALLBACK_DEBUG("ENDPOINT_WATER_LEVEL_VALUE", isWrite, variables.waterLevelValue);
    break;
  case ENDPOINT_RAW_DISTANCE_VALUE:
    PTVO_CALLBACK_DEBUG("ENDPOINT_RAW_DISTANCE_VALUE", isWrite, variables.rawDistanceValue);
    break;
  case ENDPOINT_MIN_DISTANCE_VALUE:
    PTVO_CALLBACK_DEBUG("ENDPOINT_MIN_DISTANCE_VALUE", isWrite, variables.minDistanceValue);
    if (isWrite && settings.minDistanceValue != variables.minDistanceValue)
    {
      settings.minDistanceValue = variables.minDistanceValue;
      EEPROM.put(SETTINGS_EEPROM_ADDRESS, settings);
    }
    break;
  case ENDPOINT_MAX_DISTANCE_VALUE:
    PTVO_CALLBACK_DEBUG("ENDPOINT_MAX_DISTANCE_VALUE", isWrite, variables.maxDistanceValue);
    if (isWrite && settings.maxDistanceValue != variables.maxDistanceValue)
    {
      settings.maxDistanceValue = variables.maxDistanceValue;
      EEPROM.put(SETTINGS_EEPROM_ADDRESS, settings);
    }
    break;
  case ENDPOINT_DISPLAY_STATE:
    PTVO_CALLBACK_DEBUG("ENDPOINT_DISPLAY_STATE", isWrite, variables.displayState);
    // max z2m "trigger" duration is DISPLAY_ON_DURATION
    // see https://ptvo.info/zigbee-configurable-firmware-features/gpio/
    if (isWrite) TurnOnDisplay();
    break;
  }
}

void ButtonCallback(void) {
  if (displayStateChanged) return; // not yet processed, ignore
  BlinkLed();
  TurnOnDisplay();
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  BlinkLed();

  pinMode(INT_PIN, INPUT_PULLUP);
  enableInterrupt(INT_PIN, ButtonCallback, FALLING);

  // Initialize serial
  Serial.begin(115200);
  Serial.println(F("WaterTankMonitor"));
  Serial.println(F("================"));

  // Initialize display
  display.begin();
  TurnOnDisplay();

  // Initialize sensor
  sensor.begin();

  // Configure ptvo data tag link
  ptvo.AddVariable(ENDPOINT_DISPLAY_STATE, variables.displayState, PtvoCallback);
  ptvo.AddVariable(ENDPOINT_WATER_LEVEL_VALUE, variables.waterLevelValue, PtvoCallback);
  ptvo.AddVariable(ENDPOINT_RAW_DISTANCE_VALUE, variables.rawDistanceValue, PtvoCallback);
  ptvo.AddVariable(ENDPOINT_MIN_DISTANCE_VALUE, variables.minDistanceValue, PtvoCallback);
  ptvo.AddVariable(ENDPOINT_MAX_DISTANCE_VALUE, variables.maxDistanceValue, PtvoCallback);

  // Load store settings from eeprom
  EEPROM.get(SETTINGS_EEPROM_ADDRESS, settings);
  SETTINGS_DEBUG(settings);
  variables.minDistanceValue = settings.minDistanceValue;
  variables.maxDistanceValue = settings.maxDistanceValue;

  // Send settings to firmware
  ptvo.SendVariable(ENDPOINT_DISPLAY_STATE);
  ptvo.SendVariable(ENDPOINT_MIN_DISTANCE_VALUE);
  ptvo.SendVariable(ENDPOINT_MAX_DISTANCE_VALUE);
}

void loop()
{
  // handle led status
  if (digitalRead(LED_PIN) && millis() >= turnOffLedTimestamp)
  { // should turn off
    digitalWrite(LED_PIN, LOW);
  }

  // handle display status
  if (variables.displayState && millis() >= turnOffDisplayTimestamp)
  { // should turn off
    variables.displayState = 0;
    displayStateChanged = true;
  }

  if (displayStateChanged)
  { // report state changed
    display.setDisplay(variables.displayState);
    ptvo.SendVariable(ENDPOINT_DISPLAY_STATE);
    displayStateChanged = false;
  }

  // handle ptvo comms
  ptvo.Process();

  // sensor reading
  float distance = sensor.readDistanceCm();
  if (distance != variables.rawDistanceValue)
  {
    variables.rawDistanceValue = distance;
    variables.waterLevelValue = (variables.maxDistanceValue - variables.rawDistanceValue) / (variables.maxDistanceValue - variables.minDistanceValue) * 100.0;
    variables.waterLevelValue = constrain(variables.waterLevelValue, 0, 100);
    display.displayPercentage(variables.waterLevelValue);
  }
}
