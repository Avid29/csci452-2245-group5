// Shaun Thornton

#include <Wire.h>

#define MULTIPLEXER_ADDR (0x70)

static void Multiplexer_Select(int channel)
{
  Wire.beginTransmission(MULTIPLEXER_ADDR);
    Wire.write(0x04 | channel);
  Wire.endTransmission();
}


static uint8_t Multiplexer_Status()
{
  Wire.requestFrom(MULTIPLEXER_ADDR, 1);
  return Wire.read();
}