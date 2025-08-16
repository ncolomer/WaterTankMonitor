#include <Arduino.h>
#include <Preferences.h>

#include "Display.h"
#include "Sensor.h"
#include "ZigbeeManager.h"
#include "main.h"

// Global objects
Display display;
Sensor sensor;
ZigbeeManager zigbee;
Preferences preferences;

// Global variables
Settings settings;
Variables variables;

// Timing variables
uint32_t turnOffLedTimestamp = 0;
uint32_t turnOffDisplayTimestamp = 0;
uint32_t lastSensorReadTime = 0;
uint32_t lastZigbeeReportTime = 0;

// Button handling
bool lastButtonState = HIGH;
bool buttonPressed = false;

void IRAM_ATTR buttonISR() {
    buttonPressed = true;
}

void blinkLed() {
    digitalWrite(LED_PIN, HIGH);
    turnOffLedTimestamp = millis() + BLINK_DURATION;
}

void turnOnDisplay() {
    turnOffDisplayTimestamp = millis() + DISPLAY_ON_DURATION;
    variables.displayState = 1;
    display.setDisplay(true);
    zigbee.setDisplayState(true);
    blinkLed();
}

void turnOffDisplay() {
    variables.displayState = 0;
    display.setDisplay(false);
    zigbee.setDisplayState(false);
}

void handleButton() {
    if (buttonPressed) {
        buttonPressed = false;
        turnOnDisplay();
    }
}

void updateWaterLevel() {
    float distance = sensor.readDistanceCm();
    
    if (!isnan(distance) && distance != variables.rawDistanceValue) {
        variables.rawDistanceValue = distance;
        
        // Calculate water level percentage
        if (variables.maxDistanceValue > variables.minDistanceValue) {
            variables.waterLevelValue = (variables.maxDistanceValue - variables.rawDistanceValue) / 
                                      (variables.maxDistanceValue - variables.minDistanceValue) * 100.0;
            variables.waterLevelValue = constrain(variables.waterLevelValue, 0, 100);
        } else {
            variables.waterLevelValue = 0;
        }
        
        // Update display
        display.displayPercentage((uint8_t)variables.waterLevelValue);
        
        // Update Zigbee attributes
        zigbee.setRawDistance(variables.rawDistanceValue);
        zigbee.setWaterLevel(variables.waterLevelValue);
        
        Serial.printf("Distance: %.1f cm, Water Level: %.1f%%\n", 
                     variables.rawDistanceValue, variables.waterLevelValue);
    }
}

void loadSettings() {
    // Load settings from preferences
    settings.minDistanceValue = preferences.getFloat("min_dist", 20.0);
    settings.maxDistanceValue = preferences.getFloat("max_dist", 200.0);
    
    // Apply to variables
    variables.minDistanceValue = settings.minDistanceValue;
    variables.maxDistanceValue = settings.maxDistanceValue;
    
    SETTINGS_DEBUG(settings);
}

void saveSettings() {
    preferences.putFloat("min_dist", settings.minDistanceValue);
    preferences.putFloat("max_dist", settings.maxDistanceValue);
}

void setup() {
    // Initialize serial
    Serial.begin(115200);
    delay(1000); // Give time for serial monitor
    
    Serial.println(F("Water Tank Monitor - ESP32C6 Version"));
    Serial.println(F("===================================="));
    
    // Initialize preferences
    if (!preferences.begin("water-tank", false)) {
        Serial.println(F("Failed to initialize preferences"));
    }
    
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    // Attach button interrupt
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    
    // Initialize components
    if (!display.begin()) {
        Serial.println(F("Failed to initialize display"));
    }
    
    if (!sensor.begin()) {
        Serial.println(F("Failed to initialize sensor"));
    }
    
    if (!zigbee.begin()) {
        Serial.println(F("Failed to initialize Zigbee"));
    }
    
    // Load settings
    loadSettings();
    
    // Turn on display initially
    turnOnDisplay();
    
    Serial.println(F("Setup complete"));
}

void loop() {
    // Handle LED status
    if (digitalRead(LED_PIN) && millis() >= turnOffLedTimestamp) {
        digitalWrite(LED_PIN, LOW);
    }
    
    // Handle button press
    handleButton();
    
    // Handle display timeout
    if (variables.displayState && millis() >= turnOffDisplayTimestamp) {
        turnOffDisplay();
    }
    
    // Handle sensor reading
    if (millis() - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
        updateWaterLevel();
        lastSensorReadTime = millis();
    }
    
    // Handle Zigbee processing
    zigbee.process();
    
    // Handle Zigbee reporting
    if (millis() - lastZigbeeReportTime >= ZIGBEE_REPORT_INTERVAL) {
        zigbee.reportAllValues();
        lastZigbeeReportTime = millis();
    }
    
    // Small delay to prevent watchdog issues
    delay(10);
}
