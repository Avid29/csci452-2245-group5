// Generic PS2Controller interface
// Defines basic functionality that the controller can perform
// These functions, error enums, etc. are based on the official PS/2 standard

#ifndef PS2_CONTROLLER_H
#define PS2_CONTROLLER_H

#include <common.h>

// Get the output buffer status (bit 0). Must be set to 1 before attempting to read the data port
#define WAIT_FOR_DATA(controller) while(!controller->DataAvailable());

// Get the input buffer status (bit 1). Must be set to 0 before attempting to write data to the controller/data port
#define WAIT_FOR_CONTROLLER(controller) while(controller->ControllerBusy());


// The controller status register
union PS2Controller_Status
{
    struct
    {
        uint8_t outputBufferFull : 1;
        uint8_t inputBufferFull : 1;
        uint8_t systemFlag : 1;
        uint8_t commandData : 1;
        uint8_t undefined : 2; // These bits have no standard definition/purpose. We'll still expose them just in case
        uint8_t timeoutError : 1;
        uint8_t parityError : 1;
    };
    uint8_t byte;
};

// The controller configuration
union PS2Controller_Config
{
    struct
    {
        uint8_t port1InterruptEnabled : 1;
        uint8_t port2InterruptEnabled : 1;
        uint8_t systemFlag : 1;
        uint8_t : 1; // "should be zero" according to the standard. No implementation should touch this
        uint8_t port1ClockDisabled : 1;
        uint8_t port2ClockDisabled : 1;
        uint8_t port1TranslationEnabled : 1;
        uint8_t : 1; // "Must be zero" according to the standard. No implementation should touch this
    };
    uint8_t byte;
};

// The controller output port
union PS2Controller_Output
{
    struct
    {
        uint8_t systemReset : 1; // Should always be 1
        uint8_t A20Gate : 1;
        uint8_t port2Clock : 1;
        uint8_t port2Data : 1;
        uint8_t port1Interrupt : 1; // Gets set when data arrives from port 1
        uint8_t port2Interrupt : 1; // Gets set port 2
        uint8_t port1Clock : 1;
        uint8_t port1Data : 1;
    };
    uint8_t byte;
};

// Enumeration of all standard PS/2 controller commands
enum PS2Controller_Command
{
    // Enum min value, for bounds checking
    PS2_CMD_MIN_VALUE,

    // Configuration read/write
    PS2_CMD_READ_CONFIG = 0x20,
    PS2_CMD_WRITE_CONFIG = 0x60,

    // Port 2 enable/disable
    PS2_CMD_DISABLE_PORT2 = 0xA7,
    PS2_CMD_ENABLE_PORT2 = 0xA8,

    // Testing commands
    PS2_CMD_TEST_PORT2 = 0xA9,
    PS2_CMD_TEST_CONTROLLER = 0xAA,
    PS2_CMD_TEST_PORT1 = 0xAB,

    // Port 1 enable/disable
    PS2_CMD_DISABLE_PORT1 = 0xAD,
    PS2_CMD_ENABLE_PORT1 = 0xAE,

    // Read controller input port
    PS2_CMD_CPY_INPUT_PORT1 = 0xC1, // Copy bits 0-3 from input port to status
    PS2_CMD_CPY_INPUT_PORT2 = 0xC2, // Copy bits 4-7 from input port to status

    // Read/write controller output port
    PS2_CMD_READ_OUTPUT_PORT = 0xD0,
    PS2_CMD_WRITE_OUTPUT_PORT = 0xD1,

    // Loopback writes
    PS2_CMD_WRITE_PORT1_BUFFER_IN = 0xD2, // Looks as if port1 wrote data
    PS2_CMD_WRITE_PORT2_BUFFER_IN = 0xD3, // Looks as if port2 wrote data

    // Port 2 write (must be explicitly commanded)
    PS2_CMD_WRITE_PORT2_BUFFER_OUT = 0xD4,

    // Pulse output line (reset)
    PS2_CMD_PULSE_RESET = 0xF0,

    // Enum max value, for bounds checking
    PS2_CMD_MAX_VALUE,
};


// Define the commands we support and their traits
#define CMD_VALID (0x01)
#define CMD_ARGUMENT  (0x02)
#define CMD_RESPONSE  (0x04)

#define CMD_IS_VALID(cmd) (cmd & CMD_VALID)
#define CMD_REQUIRES_ARGUMENT(cmd) (cmd & CMD_ARGUMENT)
#define CMD_HAS_RESPONSE(cmd) (cmd & CMD_RESPONSE)

// List of commands implemented by this controller
static const uint8_t PS2Controller_Commands[PS2_CMD_MAX_VALUE] = {
    [PS2_CMD_READ_CONFIG] = CMD_VALID | CMD_RESPONSE,
    [PS2_CMD_WRITE_CONFIG] = CMD_VALID | CMD_ARGUMENT,
    [PS2_CMD_DISABLE_PORT2] = CMD_VALID,
    [PS2_CMD_ENABLE_PORT2] = CMD_VALID,
    [PS2_CMD_TEST_PORT2] = CMD_VALID | CMD_RESPONSE,
    [PS2_CMD_TEST_CONTROLLER] = CMD_VALID | CMD_RESPONSE,
    [PS2_CMD_TEST_PORT1] = CMD_VALID | CMD_RESPONSE,
    [PS2_CMD_DISABLE_PORT1] = CMD_VALID,
    [PS2_CMD_ENABLE_PORT1] = CMD_VALID,
    [PS2_CMD_CPY_INPUT_PORT1] = CMD_VALID,
    [PS2_CMD_CPY_INPUT_PORT2] = CMD_VALID,
    [PS2_CMD_READ_OUTPUT_PORT] = CMD_VALID | CMD_RESPONSE,
    [PS2_CMD_WRITE_OUTPUT_PORT] = CMD_VALID | CMD_ARGUMENT,
    [PS2_CMD_WRITE_PORT1_BUFFER_IN] = CMD_VALID | CMD_ARGUMENT,
    [PS2_CMD_WRITE_PORT2_BUFFER_IN] = CMD_VALID | CMD_ARGUMENT,
    [PS2_CMD_WRITE_PORT2_BUFFER_OUT] = CMD_VALID | CMD_ARGUMENT,
    [PS2_CMD_PULSE_RESET] = CMD_VALID,
};

// Self test results
enum PS2Controller_SelfTest
{
    PS2_SelfTest_Passed = 0x55,
    PS2_SelfTest_Failed = 0xFC,
};

// Port test results
enum PS2Controller_PortTest
{
    PS2_PortTest_Passed = 0x00,
    PS2_PortTest_Clk_Stuck_Low = 0x01,
    PS2_PortTest_Clk_Stuck_High = 0x02,
    PS2_PortTest_Data_Stuck_Low = 0x03,
    PS2_PortTest_Data_Stuck_High = 0x04,
};

typedef struct PS2Controller_s
{
    // Send a command to the ps2 controller, write command register
    void (*WriteCommand)(enum PS2Controller_Command command);

    // Write data to the data port
    void (*WriteData)(uint8_t data);

    // Read data from the data port
    void (*ReadData)(uint8_t *data);

    // Check if data is available
    int (*DataAvailable)(void);

    // Check if the controller is busy
    int (*ControllerBusy)(void);
} PS2Controller_t;


// =====================================
// Implementation-specific functionality. Users typically won't want to use these functions directly.
// =====================================

// Initialize controller
int PS2Controller_Init(PS2Controller_t* controller);

// Executes the given command. Uses provided argument if command requires one. Populates response if command gives one

static inline int PS2Controller_ProcessCommand(
    PS2Controller_t *controller,
    enum PS2Controller_Command command,
    const uint8_t argument,
    uint8_t *response)
{
    // First, make sure the command is valid
    if (command <= PS2_CMD_MIN_VALUE || command >= PS2_CMD_MAX_VALUE || 
    	!CMD_IS_VALID(PS2Controller_Commands[command]))
    {
        return -1;  // Invalid command
    }

    // Send the command
    WAIT_FOR_CONTROLLER(controller);
    controller->WriteCommand(command);

    // Check if an argument is required
    if (CMD_REQUIRES_ARGUMENT(PS2Controller_Commands[command]))
    {
        // Send the argument as data
        WAIT_FOR_CONTROLLER(controller);
        controller->WriteData(argument);
    }

    // Check if response is expected
    if (CMD_HAS_RESPONSE(PS2Controller_Commands[command]))
    {
        // Read the response data, but discard it if we were given a NULL response pointer
        uint8_t tmpResponse;

        WAIT_FOR_DATA(controller);
        controller->ReadData(&tmpResponse);

        if (response)
        {
            *response = tmpResponse;
        }
    }

    return 0;   // Success
}

// ============================================================
// Standard-defined functionality that uses the above functions
// ============================================================

// Determines whether data is available
static inline int PS2Controller_DataAvailable(PS2Controller_t *controller)
{
    return controller->DataAvailable();
}

// Read the configuration register
static inline int PS2Controller_ReadConfig(PS2Controller_t* controller, union PS2Controller_Config* config)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_READ_CONFIG, 0, &config->byte);
}

// Write to the configuration register
static inline int PS2Controller_WriteConfig(PS2Controller_t* controller, const union PS2Controller_Config config)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_WRITE_CONFIG, config.byte, NULL);
}

// Read the controller output port - note that this does NOT hold data but rather the current status of the output port
static inline int PS2Controller_ReadOutputPort(PS2Controller_t* controller, union PS2Controller_Output* output)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_READ_OUTPUT_PORT, 0, &output->byte);
}

// Write the controller output port - note that this does NOT hold data but rather the current status of the output port
static inline int PS2Controller_WriteOutputPort(PS2Controller_t* controller, const union PS2Controller_Output output)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_WRITE_OUTPUT_PORT, output.byte, NULL);
}

// Clean up the global namespace
#undef WAIT_FOR_DATA
#undef WAIT_FOR_CONTROLLER

#undef CMD_VALID
#undef CMD_ARGUMENT
#undef CMD_RESPONSE

#undef CMD_IS_VALID
#undef CMD_REQUIRES_ARGUMENT
#undef CMD_HAS_RESPONSE

#endif
