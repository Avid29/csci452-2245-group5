#define KERNEL_SRC

#include <common.h>

#include <beeper.h>
#include <bootstrap.h>
#include <clock.h>
#include <x86/pit.h>

// Audio sample rate (Hz)
#define BITRATE 44100

int beeper_write(uint8_t *buf, uint_t length)
{
	/* Turn on :b:eeper */
	outb( PIT_CONTROL_PORT, PIT_2_SELECT | PIT_0_LOAD | PIT_MODE_0 );
	outb( PIT_2_PORT, 0 ); // LSB of divisor
	outb( PIT_2_PORT, 0 ); // MSB of divisor

	/* memory address 1 */
	uint8_t tmp = inb(KBD_DATA | 1);
	/* 0b1 should be speaker gate. 0b10 is gate into PIT port 2 */
	tmp = (tmp & ~0b11) | 0b11;
	outb(KBD_DATA | 1, tmp);

	for (int i = 0; i < length; ++i) {
		uint8_t amplitude = buf[i];

		uint32_t divisor = (amplitude * PIT_FREQ) / (BITRATE * 256);

		// Ignore all but extreme swings
		if (amplitude > 128+32 || amplitude < 128-32) {
			outb(PIT_2_PORT, divisor & 0xff);
			outb(PIT_2_PORT, (divisor >> 8) & 0xff);
		}
		sleep_micros(1000000/BITRATE); // (1000000=second->microsecond)
	}

	/* Unset speaker clock gate */
	tmp = inb(KBD_DATA | 1);
	outb (KBD_DATA | 1, tmp & (~0b11));

	return 0;
}
