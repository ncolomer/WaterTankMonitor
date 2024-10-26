#include <CircularBuffer.h>

CircularBuffer::CircularBuffer(uint8_t size)
  : bufferSize{size}
  , buffer{new uint8_t[size]{0x00}}
{
}

uint8_t CircularBuffer::idx(uint8_t index)
{
  return (ptr + bufferSize + index) % bufferSize;
}

void CircularBuffer::clear(void)
{
  for (int i = 0; i < bufferSize; i++) buffer[i] = 0;
  size = 0;
}

/// @brief push a new byte on the right-most part of the buffer, shifting existing element to the left
/// @param word the value to push
void CircularBuffer::push(uint8_t word)
{
  buffer[ptr] = word; // write at current position
  ptr = (ptr + 1) % bufferSize; // move the pointer
  size = min(size + 1, bufferSize); // update size
}

void CircularBuffer::shift(void)
{
  ptr = (ptr + 1) % bufferSize; // move the pointer
}

/// @brief return the value at index (zero-based)
/// @param index the index to lookup
/// @return the stored value
uint8_t CircularBuffer::at(uint8_t index)
{
  return buffer[idx(index)];
}

void CircularBuffer::toString(Print *str)
{
  str->print(F("["));
  for (int i = 0; i < bufferSize; i++)
  {
    str->printf("%02x", at(i));
    if (i < bufferSize - 1) str->print(F(","));
  }
  str->print(F("]"));
}
