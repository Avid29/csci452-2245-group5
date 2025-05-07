// Shaun Thornton

#include <Wire.h>

#define NUNCHUK_ADDR     (0x52)

static void NUN_Init() {
  Wire.beginTransmission(NUNCHUK_ADDR);
    Wire.write(0xF0);
    Wire.write(0x55);
  Wire.endTransmission();

  Wire.beginTransmission(NUNCHUK_ADDR);
    Wire.write(0xFB);
    Wire.write(0x00);
  Wire.endTransmission();
}

static void NUN_Read(uint8_t *data) {

  Wire.requestFrom(NUNCHUK_ADDR, 6);

  for (uint8_t i = 0; i < 6 && Wire.available(); i++) {
    data[i] = Wire.read();
  }

  Wire.beginTransmission(NUNCHUK_ADDR);
    Wire.write(0x00);
  Wire.endTransmission();
}

static uint8_t NUN_ZButton(uint8_t *data) {
    return (~data[5] >> 0) & 1;
}

static uint8_t NUN_CButton(uint8_t *data) {
    return (~data[5] >> 1) & 1;
}

static uint8_t NUN_joystickX(uint8_t *data) {
    return data[0];
}

static uint8_t NUN_joystickY(uint8_t *data) {
    return data[1];
}
