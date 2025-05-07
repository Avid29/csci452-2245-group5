/**
 * @file PS2Controller.h
 * @author Shaun Thornton (sdt8987)
 * @date 3 May 2025
 * @brief Generic PS2Controller interface
 *
 * Defines the set of functions that a standard-compliant PS2 controller
 * driver implementation would need to implement. Various unions, structs,
 * bitfields, and enums are defined in accordance with the standard.
 *
 * @see https://wiki.osdev.org/I8042_PS/2_Controller
 */

// Guard
#ifndef PS2_CONTROLLER_H
#define PS2_CONTROLLER_H

#include <common.h>

// Busy waiting loops to be used before attempting to interact with
// the controller command register or data port.
// These are undef-ed later to avoid polluting the namespace
#define WAIT_FOR_DATA(controller) while(!controller->DataAvailable());
#define WAIT_FOR_CONTROLLER(controller) while(controller->ControllerBusy());

/**
 * @brief The controller status register
 *
 * A byte-bitfield union represending the controller status register.
 * The names and meanings of each bit are clearly defined by the standard,
 * though some are implementation dependent ("undefined").
 *
 * This register is read only (IO Port 0x64)
 */
union PS2Controller_Status
{
    struct
    {
        uint8_t outputBufferFull : 1;	/**< Incoming data buffer status */
        uint8_t inputBufferFull : 1;	/**< Outgoing data/cmd buffer status */
        uint8_t systemFlag : 1;		/**< Set to 1 on system POST */
        uint8_t commandData : 1;	/**< Buffer data is cmd (1), dev (0) */
        uint8_t undefined : 2;		/**< Chipset specific meaning */
        uint8_t timeoutError : 1;	/**< Timeout error occurred (1) */
        uint8_t parityError : 1;	/**< Parity error occurred (1) */
    } fields;		/**< Individual bit components of the control status */
    uint8_t byte;	/**< Byte representation of the controller status */
};

/**
 * @brief The controller configuration byte
 *
 * A byte-bitfield union representing the controller configuration byte.
 * The names and meanings of most bits are defined by the standard, with
 * some being chipset specific.
 *
 * This byte can be read using command 0x20 and written with command 0x21
 */
union PS2Controller_Config
{
    struct
    {
        uint8_t port1InterruptEnabled : 1;  /**< Port 1 interrupt enable */
        uint8_t port2InterruptEnabled : 1;  /**< Port 2 interrupt enable */
        uint8_t systemFlag : 1;		    /**< Set to 1 on system  POST */
        uint8_t : 1;			    /**< Should be zero, don't touch */
        uint8_t port1ClockDisabled : 1;     /**< Port 1 communication disable*/
        uint8_t port2ClockDisabled : 1;     /**< Port 2 communication disable*/
        uint8_t port1TranslationEnabled: 1; /**< Port 1 translation enable */
        uint8_t : 1;			    /**< MUST be zero, don't touch */
    } fields;		/**< Individual bit components of the config byte */
    uint8_t byte;	/**< Byte representation of the config byte */
};

/**
 * @brief The controller output port
 *
 * A byte-bitfield union representing the controller output port.
 * The names and meanings of most bits are defined by the standard, and
 * although a command does exist to overwrite the value of this port,
 * testing suggests that it is functionally read-only.
 *
 * This byte can be read using command 0xD0 and written with command 0xD1 
 */
union PS2Controller_Output
{
    struct
    {
        uint8_t systemReset : 1;	/**< Should always be 1 */
        uint8_t A20Gate : 1;		/**< State of the A20 gate */
        uint8_t port2Clock : 1;		/**< State of the port 2 clock line */
        uint8_t port2Data : 1;		/**< State of the port 2 data line */
        uint8_t port1Interrupt : 1;	/**< Connected to port 1 IRQ */
        uint8_t port2Interrupt : 1;	/**< Connected to port 2 IRQ */
        uint8_t port1Clock : 1;		/**< State of the port 1 clock line */
        uint8_t port1Data : 1;		/**< State of the port 1 data line */
    } fields;		/**< Individual bit representation of the output port*/
    uint8_t byte;	/**< Byte representation of the output port */
};

/**
 * @brief Enumeration of all standard PS/2 controller commands
 *
 * Note that some "standard" commands with pooly-defined functions
 * have been ommitted to avoid potential confusion and misuse
 */
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
#define CMD_VALID     (0x01)
#define CMD_ARGUMENT  (0x02)
#define CMD_RESPONSE  (0x04)

// Test for certain command traits
#define CMD_IS_VALID(cmd) (cmd & CMD_VALID)
#define CMD_REQUIRES_ARGUMENT(cmd) (cmd & CMD_ARGUMENT)
#define CMD_HAS_RESPONSE(cmd) (cmd & CMD_RESPONSE)

/**
 * @brief List of supported commands and their traits
 *
 * Indexes into this array are command codes.
 */
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

/**
 * @brief Controller self test results
 *
 * When a controller self test is performed, one of these
 * values will be returned to indicate the result
 */
enum PS2Controller_SelfTest
{
    PS2_SelfTest_Passed = 0x55,	/**< Test passed */
    PS2_SelfTest_Failed = 0xFC,	/**< Test failed */
};

/**
 * @brief Port test results
 *
 * When a PS2 port test is performed, one of these
 * values will be returned to indicate the result
 */
enum PS2Controller_PortTest
{
    PS2_PortTest_Passed = 0x00,
    PS2_PortTest_Clk_Stuck_Low = 0x01,
    PS2_PortTest_Clk_Stuck_High = 0x02,
    PS2_PortTest_Data_Stuck_Low = 0x03,
    PS2_PortTest_Data_Stuck_High = 0x04,
};

/**
 * @brief A PS2 controller instance
 *
 * Contains function pointers for performing the most basic operations
 * (register reads, writes, etc.) All other functions are built on these
 */
typedef struct PS2Controller_s
{
    /**< Send a command to the ps2 controller, write command register */
    void (*WriteCommand)(enum PS2Controller_Command command);

    /**< Write data to the data port */
    void (*WriteData)(uint8_t data);

    /**< Read data from the data port */
    void (*ReadData)(uint8_t *data);

    /**< Check if data is available */
    int (*DataAvailable)(void);

    /**< Check if the controller is busy */
    int (*ControllerBusy)(void);
} PS2Controller_t;

/**
 * @brief Initialize PS2 controller
 *
 * Initializes the provided PS2 controller. At the very least, this function
 * should initialize the function pointers within the controller strucutre.
 * It would be wise to also perform a controller self test / port test, too.
 *
 * @param controller The controller structure to initialize
 * @return 0 on success, nonzero otherwise
 */
int PS2Controller_Init(PS2Controller_t* controller);

/**
 * @brief Fully process a given command, including any arguments or results
 *
 * Executes the given command using a provided argument if the command requires
 * one. Populates response pointer if the command gives a response. If a
 * response is given but the response pointer is NULL, the response is 
 * discarded.
 *
 * @param controller The PS2 controller to perform the command with
 * @param command The command to send
 * @param argument The argument to send with the command, if required
 * @param response A pointer to the response location, if desired
 * @returns 0 on success, nonzero otherwise
 */
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

// ================================
// Static functions for convenience
// ================================

/**
 * @brief Reads the PS2 controller configuration register
 *
 * @param controller The PS2 controller to use
 * @param config A pointer to store the response at
 * @returns 0 on success, nonzero otherwise
 */
static inline int PS2Controller_ReadConfig(
	PS2Controller_t* controller,
	union PS2Controller_Config* config)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_READ_CONFIG, 0, &config->byte);
}

/**
 * @brief Writes to the PS2 controller configuration register
 *
 * @param controller The PS2 controller to use
 * @param config The configuration byte to write
 * @returns 0 on success, nonzero otherwise
 */
static inline int PS2Controller_WriteConfig(
	PS2Controller_t* controller,
	const union PS2Controller_Config config)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_WRITE_CONFIG, config.byte, NULL);
}

/**
 * @brief Reads the PS2 controller output port
 *
 * Note that the output port does NOT hold data but rather a series of status
 * bits indicating the state of the controller output port
 *
 * @param controller The PS2 controller to use
 * @param config A pointer to store the output status at
 * @returns 0 on success, nonzero otherwise
 */
static inline int PS2Controller_ReadOutputPort(
	PS2Controller_t* controller,
	union PS2Controller_Output* output)
{
    return PS2Controller_ProcessCommand(controller, PS2_CMD_READ_OUTPUT_PORT, 0, &output->byte);
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
