#include "Display.h"

// Pin definitions for XIAO ESP32C6
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

Display::Display(void) : ssd1306(128, 32, &Wire, -1), isDisplayOn(false)
{
}

bool Display::begin(void)
{
    // Configure I2C pins for XIAO ESP32C6
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    if (!ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    ssd1306.clearDisplay();
    ssd1306.setTextSize(1);
    ssd1306.setTextColor(SSD1306_WHITE);
    ssd1306.cp437(true);
    
    Serial.println(F("SSD1306 initialized successfully"));
    return true;
}

void Display::setDisplay(bool isOn)
{
    isDisplayOn = isOn;
    if (isOn) {
        ssd1306.ssd1306_command(SSD1306_DISPLAYON);
        displayDefaultView();
    } else {
        ssd1306.ssd1306_command(SSD1306_DISPLAYOFF);
    }
}

void Display::displayPercentage(uint8_t percentage)
{
    if (!isDisplayOn) return;
    
    ssd1306.clearDisplay();
    
    // Display title
    ssd1306.setCursor(0, 0);
    ssd1306.print(F("Water Level"));
    
    // Display percentage
    ssd1306.setCursor(0, 12);
    ssd1306.setTextSize(2);
    ssd1306.print(percentage);
    ssd1306.print(F("%"));
    
    // Display bar
    int barWidth = map(percentage, 0, 100, 0, 120);
    ssd1306.drawRect(0, 28, 120, 4, SSD1306_WHITE);
    if (barWidth > 0) {
        ssd1306.fillRect(0, 28, barWidth, 4, SSD1306_WHITE);
    }
    
    ssd1306.display();
}

void Display::displayDefaultView(void)
{
    ssd1306.clearDisplay();
    ssd1306.setCursor(0, 0);
    ssd1306.setTextSize(1);
    ssd1306.print(F("Water Tank Monitor"));
    ssd1306.setCursor(0, 12);
    ssd1306.print(F("Ready"));
    ssd1306.display();
}
