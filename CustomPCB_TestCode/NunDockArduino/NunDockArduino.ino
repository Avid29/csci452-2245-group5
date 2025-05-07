// Program for interacting with custom Nunchuk PCB
// Shaun Thornton

#include <Wire.h>

#include "Nunchuck.h"
#include "Led.h"
#include "Multiplexer.h"

#define N_Nunchuks (4)
static struct Nunchuck 
{
  bool present;
  uint8_t data[6];
} nunchuks[N_Nunchuks];

void setup() {
  Serial.begin(115200);
  Wire.begin();
  LED_Init();

  // Determine if any nunchuks are already connected
  uint8_t cr = Multiplexer_Status() >> 4;
  for(int i = 0; i < N_Nunchuks; i++, cr >>= 1)
  {
    nunchuks[i].present = (cr & 1); // Lie (say the opposite)
  }
}

void loop() {

  // Read data from all nunchuks
  for(int i = 0, controlRegister = Multiplexer_Status() >> 4; i < N_Nunchuks; i++, controlRegister >>= 1)
  {
    if(!(controlRegister & 1))
    {
      Multiplexer_Select(i);

      if(!nunchuks[i].present)
      {
        // This nunchuk was just plugged in
        nunchuks[i].present = true;
        NUN_Init();

        // Update LED status indicator
        LED_Pattern(i, SOLID);    // Solid
        LED_Color(i, 0, 255, 0);  // Green
      }

      NUN_Read(nunchuks[i].data);
    }
    else if(nunchuks[i].present)
    {
      // The nunchuk that was here last time is now gone
      nunchuks[i].present = false;

      // Update LED status indicator
      LED_Pattern(i, BLINK);    // Blinking
      LED_Color(i, 255, 0, 0);  // Red
    }
  }

  // Pretty-print the state of all nunchuks
  for(int i = 0; i < N_Nunchuks; i++)
  {
    Serial.write(nunchuks[i].present);
    Serial.write(nunchuks[i].data, 6);
  }

  Serial.flush();
  
  delay(50);
}
