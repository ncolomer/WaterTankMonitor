#ifndef SRC_CIRCULAR_BUFFER_H_
#define SRC_CIRCULAR_BUFFER_H_

#include <Arduino.h>

class CircularBuffer {
private:
  uint8_t bufferSize;
  uint8_t* buffer;
  uint8_t size = 0;
  uint8_t ptr = 0;
  uint8_t idx(uint8_t index);
public:
  CircularBuffer(uint8_t size);
  void clear(void);
  void push(uint8_t word);
  void shift(void);
  uint8_t at(uint8_t index);
  void toString(Print *str);
};

#endif
