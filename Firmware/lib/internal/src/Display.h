#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include <Adafruit_SSD1306.h>

class Display {
private:
    Adafruit_SSD1306 ssd1306;
    void displayDefaultView(void);
public:
    Display(void);
    bool begin(void);
    void setDisplay(bool isOn);
    void displayPercentage(uint8_t percentage);
};

#endif
