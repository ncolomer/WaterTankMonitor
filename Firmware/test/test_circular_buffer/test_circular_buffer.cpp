
#include <Arduino.h>
#include <CircularBuffer.h>
#include <unity.h>

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_push_1b(void)
{
  CircularBuffer buffer(4);
  buffer.push(0x80);
  TEST_MESSAGE(buffer.toString());
  TEST_ASSERT_EQUAL(0x80, buffer.at(0));
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_push_1b);
  UNITY_END();
}

void loop()
{
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
  delay(500);
}
