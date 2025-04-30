#ifndef I2C_INC_C_
#define I2C_INC_C_
#include <common.h>

#include <x86/ops.h>

/**
** i2c test program
**/

// Read & Write
#define DATA_PORT    (0x60)

// Read-only
#define STATUS_REG   (0x64)

// Write-only
#define COMMAND_REG  (0x64)

// Poll the output buffer status (bit 0)
// Must be set to 1 before attempting to read data
#define POLL_READ_READY() while(inb(STATUS_REG) & 0x1);

// Poll the input buffer status (bit 1)
// Must be clear before attempting to write data
#define POLL_WRITE_READY() while(!(inb(STATUS_REG) & 0x2))

static uint8_t PS2_CONTROLLER_READ(int cmd) {
    // Send command to controller
    POLL_WRITE_READY();
    outb(COMMAND_REG, cmd);

    // Read response from controller
    POLL_READ_READY();
    return inb(DATA_PORT);
}

static void PS2_CONTROLLER_WRITE(int cmd, uint8_t data) {
    // Send command to controller
    POLL_WRITE_READY();
    outb(COMMAND_REG, cmd);

    // Write data to controller
    POLL_WRITE_READY();
    outb(DATA_PORT, data);
}


// Put the PS2 controller into the desired state
static void PS2_INIT() {
    uint8_t config;

    // Read the controller configuration byte
    config = PS2_CONTROLLER_READ(0x20);

    // Disable first PS/2 port interrupt (clear bit 0)
    // Enable first PS/2 port clock (clear bit 4)
    // Disable first PS/2 port translation (clear bit 6)
    config &= 0b10101110;

    // Write to the controller configuration byte
    PS2_CONTROLLER_WRITE(0x60, config);
}

USERMAIN( i2c ) {
	char buf[128];
	
	usprint( buf, "Landed in i2c program\n" );
	cwrites( buf );
	
        while(true) {}

	exit( 0 );

	return( 42 );  // shut the compiler up!
}
#endif
