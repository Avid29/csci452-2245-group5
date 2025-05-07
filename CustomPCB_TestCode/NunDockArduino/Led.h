// Shaun Thornton

#include <Wire.h>

#define LED_DRIVER_ADDR  (0x74)

#define LED_MAX_CURRENT (0x1)
#define RGB_TO_CURRENT(rgb) map(rgb, 0, 255, 0, LED_MAX_CURRENT)

enum Pattern
{
  SOLID = 0x0F,
  BLINK = 0x05
};


static void LED_Color(int ledn, uint8_t red, uint8_t green, uint8_t blue)
{
  Wire.beginTransmission(LED_DRIVER_ADDR);
    Wire.write(0x03 + ledn * 3);
    Wire.write(RGB_TO_CURRENT(red));
    Wire.write(RGB_TO_CURRENT(green));
    Wire.write(RGB_TO_CURRENT(blue));
  Wire.endTransmission();
}

static void LED_Init()
{
  Wire.beginTransmission(LED_DRIVER_ADDR);
    Wire.write(0x0F); // Starting reg address

    Wire.write(0x55);  // Configure pattern generator
    Wire.write(0x00);  // Fade rate 0

    // Turn off all LEDs
    Wire.write(SOLID);
    Wire.write(SOLID);
    Wire.write(SOLID);
    Wire.write(SOLID);

    // Watchdog (infinite)
    Wire.write(0xFF);
  Wire.endTransmission();

  // Control register (enable LEDs)
  Wire.beginTransmission(LED_DRIVER_ADDR);
    Wire.write(0x02);
    Wire.write(0x80);
  Wire.endTransmission();

  // Test colors
  for(int i = 0; i < 4; LED_Color(i++, 255, 0, 0));
  delay(1000);
  for(int i = 0; i < 4; LED_Color(i++, 0, 255, 0));
  delay(1000);
  for(int i = 0; i < 4; LED_Color(i++, 0, 0, 255));
  delay(1000);
}




static void LED_Pattern(int nled, enum Pattern pattern)
{
  Wire.beginTransmission(LED_DRIVER_ADDR);
    Wire.write(0x11 + nled);
    Wire.write(pattern);
  Wire.endTransmission();
}

