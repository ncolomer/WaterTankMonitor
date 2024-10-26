#include <Sensor.h>

#define LIBCALL_ENABLEINTERRUPT // see https://github.com/GreyGnome/EnableInterrupt/issues/20#issuecomment-145151367
#include <EnableInterrupt.h>

#define RX_PIN 3 // PD3
#define TX_PIN 4 // PD4
#define READ_INTERVAL 500 // ms

void serialCallback(void)
{
  NeoSWSerial::rxISR(PIND);
}

Sensor::Sensor(void) : serial(RX_PIN, TX_PIN), buffer(4)
{
}

bool Sensor::begin(void)
{
  serial.begin(9600);
  enableInterrupt(RX_PIN, serialCallback, CHANGE);
  digitalWrite(TX_PIN, HIGH); // enable processed (more stable) values, see datasheet
  return true;
}

/// @brief read distance from sensor and refresh cached value if valid
/// @return the (maybe cached) measured distance in centimeter
/// See // https://wiki.dfrobot.com/_A02YYUW_Waterproof_Ultrasonic_Sensor_SKU_SEN0311
float Sensor::readDistanceCm(void)
{
  if (millis() > nextReadTimestamp) {
    // time for refresh!
    // issue true read
    while (serial.available()) serial.read(); // flush rx
    delay(100); // wait before reading
    for (int i = 0; i < 4; i++) buffer.push(serial.read()); // read data
    for (int i = 1; i <= 3; i++) if (buffer.at(0) != 0xFF) buffer.shift(); else break; // reorder if needed

    uint8_t error = 0;
    if (buffer.at(0) == 0xFF) { // verify frame start
      if (((buffer.at(0) + buffer.at(1) + buffer.at(2)) & 0x00FF) == buffer.at(3)) { // verify checksum
        float decoded = (buffer.at(1) << 8) + buffer.at(2); // decode data
        if (decoded > 30) {
          rawDistanceMm = decoded; // update cache
        } else error = 1; // below lower limit
      } else error = 2; // checksum error
    } else error = 3; // invalid frame

    #ifdef DEBUG
    buffer.toString(&Serial);
    Serial.print(F(" > "));
    switch (error) {
      case 0: Serial.print(rawDistanceMm); break;
      case 1: Serial.print(F("error: below lower limit")); break;
      case 2: Serial.print(F("error: checksum error")); break;
      case 3: Serial.print(F("error: invalid frame")); break;
      default: break;
    }
    Serial.println();
    #endif

    nextReadTimestamp = millis() + READ_INTERVAL;
  }
  return rawDistanceMm / 10;
}
