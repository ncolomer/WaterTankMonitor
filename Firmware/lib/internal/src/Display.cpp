#include <Display.h>

#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Display::Display(void)
{
    this->ssd1306 = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
}

bool Display::begin(void)
{
    bool initialized = ssd1306.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    if (!initialized) return false;
    ssd1306.setRotation(1);
    ssd1306.dim(true);
    displayDefaultView();
    return true;
}

void Display::setDisplay(bool isOn)
{
  if (isOn) {
    ssd1306.ssd1306_command(SSD1306_DISPLAYON);
  } else {
    ssd1306.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}

/// @brief display the default 'unknown' view
void Display::displayDefaultView(void)
{
  // setup
  ssd1306.clearDisplay();
  // border
  ssd1306.drawRect(0, 0, ssd1306.width(), ssd1306.height(), SSD1306_WHITE);
  // text cursor
  ssd1306.setTextSize(1);
  ssd1306.setTextColor(SSD1306_INVERSE);
  uint8_t offset = 14;
  ssd1306.setCursor(offset, ssd1306.height()/2);
  // display
  ssd1306.print(F("?"));
  // flush
  ssd1306.display();
}

/// @brief display a percentage bar with value
/// @param percentage percentage in per cent, value will be constrained to [0;100]
void Display::displayPercentage(uint8_t percentage)
{
  if (isnan(percentage))
  {
    displayDefaultView();
    return;
  }
  constrain(percentage, 0, 100);
  // setup
  ssd1306.clearDisplay();
  // border
  ssd1306.drawRect(0, 0, ssd1306.width(), ssd1306.height(), SSD1306_WHITE);
  // text cursor
  ssd1306.setTextSize(1);
  ssd1306.setTextColor(SSD1306_INVERSE);
  uint8_t offset;
  if (percentage < 10) offset = 11;
  else if (percentage < 100) offset = 8;
  else offset = 4;
  ssd1306.setCursor(offset, ssd1306.height()/2);
  // draw bar
  percentage = constrain(percentage, 0, 100);
  uint8_t pixels = map(percentage, 0, 100, 0, ssd1306.height()-4);
  ssd1306.fillRect(2, 2 + (ssd1306.height()-4) - pixels, ssd1306.width()-4, pixels, SSD1306_WHITE);
  // draw text
  ssd1306.print(percentage);
  ssd1306.print(F("%"));
  // flush
  ssd1306.display();
}
