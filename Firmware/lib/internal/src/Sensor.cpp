#include "Sensor.h"

// Pin definitions for XIAO ESP32C6
#define UART_RX_PIN 20
#define UART_TX_PIN 21

Sensor::Sensor(void) : serial(nullptr), nextReadTimestamp(0), rawDistanceMm(0), isInitialized(false)
{
}

bool Sensor::begin(void)
{
    // Use UART1 for the ultrasonic sensor
    serial = &Serial1;
    serial->begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    
    if (serial) {
        isInitialized = true;
        Serial.println(F("Ultrasonic sensor UART initialized"));
        return true;
    }
    
    Serial.println(F("Failed to initialize ultrasonic sensor UART"));
    return false;
}

float Sensor::readDistanceCm(void)
{
    if (!isInitialized || !serial) {
        return NAN;
    }
    
    // Check if it's time to read
    if (millis() < nextReadTimestamp) {
        return rawDistanceMm / 10.0; // Return last reading in cm
    }
    
    // Send measurement command to A02 sensor
    // The A02 sensor expects a specific command format
    uint8_t command[] = {0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    serial->write(command, sizeof(command));
    
    // Wait for response
    delay(100);
    
    if (serial->available() >= 4) {
        // Read response header
        uint8_t header[4];
        serial->readBytes(header, 4);
        
        // Check if response is valid (0xFF 0xFC followed by data)
        if (header[0] == 0xFF && header[1] == 0xFC) {
            // Read distance data (2 bytes, big endian)
            uint8_t data[2];
            if (serial->readBytes(data, 2) == 2) {
                uint16_t distance = (data[0] << 8) | data[1];
                
                // Validate distance (A02 range is typically 20-6000mm)
                if (distance >= 20 && distance <= 6000) {
                    rawDistanceMm = distance;
                    nextReadTimestamp = millis() + 1000; // Read every second
                    return distance / 10.0; // Convert mm to cm
                }
            }
        }
    }
    
    // If reading failed, return last valid reading or NAN
    if (rawDistanceMm > 0) {
        return rawDistanceMm / 10.0;
    }
    
    return NAN;
}
