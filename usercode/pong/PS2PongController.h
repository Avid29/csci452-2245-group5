
#include <common.h>
#include <PS2Controller.h>

#define PS2Control(data, fieldptr, press, release) \
    if(data == press) { *fieldptr = 1; return 1; } \
    else if(data == release) { *fieldptr = 0; return 1; }

// Represents pong controller
typedef struct PS2PongController_s
{
    PS2Controller_t controller;

    struct {
        uint8_t lUp;
	uint8_t rUp;
	uint8_t lDown;
	uint8_t rDown;
	uint8_t reset;
    } inputStates;

} PS2PongController_t;

// Initialize pong controller
static int PS2PongController_Init(PS2PongController_t *pongController) {
    // Set all key states to zero (not pressed)
    pongController->inputStates.lUp   = 0;
    pongController->inputStates.rUp   = 0;
    pongController->inputStates.lDown = 0;
    pongController->inputStates.rDown = 0;

    // Initialize the underlying PS2Controller
    return PS2Controller_Init(&pongController->controller);
}


static int PS2PongController_Update(PS2PongController_t *pongController) {

    // Check if keyboard data is available
    if(pongController->controller.DataAvailable()) {
        // Read data from controller
	uint8_t data;
        pongController->controller.ReadData(&data);

        // Update the key states
        PS2Control(data, &pongController->inputStates.lDown, 0x1D, 0x9D);
        PS2Control(data, &pongController->inputStates.lUp,   0x2A, 0xAA);
        PS2Control(data, &pongController->inputStates.rDown, 0x50, 0xD0);
        PS2Control(data, &pongController->inputStates.rUp,   0x48, 0xC8);
        PS2Control(data, &pongController->inputStates.reset, 0x13, 0x93);
    }

    return 0;	// No relevant data was present
}

