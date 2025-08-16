/*
  Basic Test Sketch for Water Tank Monitor - ESP32C6 Version
  
  This sketch tests the basic functionality of the ESP32C6 components:
  - OLED Display
  - Ultrasonic Sensor
  - Button handling
  - Basic timing
  
  Use this to verify hardware connections before implementing Zigbee functionality.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Pin definitions
#define LED_PIN 21
#define BUTTON_PIN 0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define UART_RX_PIN 20
#define UART_TX_PIN 21

// Display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Test variables
bool displayOn = true;
uint32_t lastToggle = 0;
uint32_t lastSensorRead = 0;
float lastDistance = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Water Tank Monitor - Basic Test");
  Serial.println("===============================");
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
  } else {
    Serial.println("Display initialized successfully");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
  }
  
  // Initialize UART for sensor
  Serial1.begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  Serial.println("UART initialized for sensor");
  
  // Initial LED blink
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Setup complete - starting test loop");
}

void loop() {
  // Handle button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    displayOn = !displayOn;
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    
    if (displayOn) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      Serial.println("Display turned ON");
    } else {
      display.ssd1306_command(SSD1306_DISPLAYOFF);
      Serial.println("Display turned OFF");
    }
    
    // Debounce
    delay(200);
  }
  
  // Toggle display every 5 seconds for testing
  if (millis() - lastToggle > 5000) {
    displayOn = !displayOn;
    if (displayOn) {
      display.ssd1306_command(SSD1306_DISPLAYON);
    } else {
      display.ssd1306_command(SSD1306_DISPLAYOFF);
    }
    lastToggle = millis();
    Serial.printf("Display toggled: %s\n", displayOn ? "ON" : "OFF");
  }
  
  // Read sensor every 2 seconds
  if (millis() - lastSensorRead > 2000) {
    // Simulate sensor reading for now
    float distance = 50.0 + random(-10, 10); // Simulate 50cm ± 10cm
    
    if (displayOn) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Water Tank Monitor");
      display.setCursor(0, 12);
      display.setTextSize(2);
      display.printf("%.0f cm", distance);
      display.display();
    }
    
    Serial.printf("Simulated distance: %.1f cm\n", distance);
    lastDistance = distance;
    lastSensorRead = millis();
  }
  
  // Small delay
  delay(100);
}
