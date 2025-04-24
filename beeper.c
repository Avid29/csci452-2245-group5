#define KERNEL_SRC

#include <common.h>

#include <beeper.h>
#include <bootstrap.h>
#include <x86/pit.h>

int beeper_write(char *buf, uint_t length)
{
	//char tmp = 42;
	/* PANIC(0, "chom?"); */
	/* assert(1 == 0); */

	/* memory address 1 */
	char tmp = inb(KBD_DATA | 1);
	/* osdev wiki says it's real... I don't know what this is. */
	outb(KBD_DATA | 1, tmp | 0b11);

	for (int i = 0; i < length; ++i) {
		char amplitude = buf[i];
		/* Stolen from osdev wiki */
		uint32_t microseconds = (amplitude * 60) / 255;
		uint32_t divisor = (PIT_FREQ / 0.000001) * microseconds;
		outb( PIT_CONTROL_PORT, PIT_2_SELECT | PIT_0_LOAD | PIT_0_SQUARE );
		outb( PIT_2_PORT, divisor & 0xff );        // LSB of divisor
		outb( PIT_2_PORT, (divisor >> 8) & 0xff ); // MSB of divisor
		// Delay until next sample
		// 44100 Hz audio... 22 microseconds
		// TODO
		//delay(1);
	}

	/* Unset speaker clock gate */
	tmp = inb(KBD_DATA | 1);
	outb (KBD_DATA | 1, tmp & (~0b11));

	return 0;
}
