/**
 * @file PS2Controller.c
 * @author Shaun Thornton (sdt8987)
 * @date 3 May 2025
 * @brief PS2Controller implementation
 *
 * Implements the PS2 controller interface defined in PS2Controller.h
 *
 * @see https://wiki.osdev.org/I8042_PS/2_Controller
 */

#include "PS2Controller.h"

#include <cio.h>
#include <x86/ops.h>

// The data port (R/W)
#define DATA_PORT   (0x60)

// The status register (Read-Only)
#define STATUS_REG  (0x64)

// The command register (Write-only)
#define COMMAND_REG (0x64)

// Implementations of the PS2 controller structure function pointers.
// These are static and are only intended to be function pointer targets.
static int DataAvailable_Impl(void) { return inb(STATUS_REG) & 0x01; }
static int ControllerBusy_Impl(void) { return inb(STATUS_REG) & 0x02; }
static void WriteCommand_Impl(enum PS2Controller_Command command) {
	outb(COMMAND_REG, (uint8_t)command); }
static void WriteData_Impl(uint8_t data) { outb(DATA_PORT, data); }
static void ReadData_Impl(uint8_t* response) { *response = inb(DATA_PORT); }

/**
 * @brief PS2 controller self test implementation
 *
 * Tells the PS2 controller to perform a self test and awaits the test results.
 * The specifics of this test are chipset dependent, but the result will always
 * be one of the two values specified within the PS2Controller_SelfTest enum.
 *
 * @param controller The controller to initialize
 * @param result A pointer to store the test results at
 * @return 0 if successful, nonzero otherwise
 */
static int SelfTest(PS2Controller_t* controller, enum PS2Controller_SelfTest* result)
{
    uint8_t tmpResult, ret;
    ret = PS2Controller_ProcessCommand(controller, 
    	PS2_CMD_TEST_CONTROLLER, 0, &tmpResult);
    *result = tmpResult;
    return ret;
}

/**
 * @brief PS2 controller Port 1 test implementation
 *
 * Tells the PS2 controller to test Port 1 (keyboard port) for any issues.
 * This test will examine the CLOCK and DATA lines of the Port 1 bus to
 * check for any electrical issues that would indicate a rogue device.
 * For example:
 * - The DATA line is being forced HIGH/LOW by the device
 * - The CLOCK line is being forced HIGH/LOW by the device
 *
 * @param controller The controller to initialize
 * @param result A pointer to store the test results at
 * @return 0 if successful, nonzero otherwise
 */
static int PortTest(PS2Controller_t* controller, enum PS2Controller_PortTest* result)
{
    uint8_t tmpResult, ret;
    ret = PS2Controller_ProcessCommand(controller, PS2_CMD_TEST_PORT1, 0, &tmpResult);
    *result = tmpResult;
    return ret;
}

/**
 * @brief PS2 controller initialization implementation
 *
 * Configures the provided controller structure and performs both a controller
 * self-test and a port 1 test. The results are printed to the serial console.
 *
 * @param controller The controller to initialize
 * @return 0 if successful, nonzero otherwise
 */
int PS2Controller_Init(PS2Controller_t* controller)
{
    int ret;

    // Initialize the controller structure
    controller->WriteCommand = &WriteCommand_Impl;
    controller->WriteData = &WriteData_Impl;
    controller->ReadData = &ReadData_Impl;
    controller->DataAvailable = &DataAvailable_Impl;
    controller->ControllerBusy = &ControllerBusy_Impl;

    // Disable port 1
    if ((ret = PS2Controller_ProcessCommand(controller,
    	PS2_CMD_DISABLE_PORT1, 0, NULL)))
    {
        return ret;
    }

    // Disable port 2
    if ((ret = PS2Controller_ProcessCommand(controller,
    	PS2_CMD_DISABLE_PORT2, 0, NULL)))
    {
        return ret;
    }

    // Flush the buffer
    if (controller->DataAvailable())
    {
        uint8_t ignored;
        controller->ReadData(&ignored);
    }

    // Read the current configuration byte
    union PS2Controller_Config config;
    PS2Controller_ReadConfig(controller, &config);

    // Disable interrupts and translation, enable clock
    config.fields.port1InterruptEnabled = 0;
    config.fields.port2InterruptEnabled = 0;
    config.fields.port1ClockDisabled = 0;

    // Write the configuration byte back
    PS2Controller_WriteConfig(controller, config);

    // Perform a self test
    cio_printf("Running controller self-test...");

    enum PS2Controller_SelfTest selfTest;
    SelfTest(controller, &selfTest);

    // Check the self test result
    if (selfTest != PS2_SelfTest_Passed)
    {
        cio_printf("FAILED!\n");
        return -100;   // Self test failed
    }
    cio_printf("PASSED!\n");

    // Perform a test of port 1
    cio_printf("Testing port 1... ");
    enum PS2Controller_PortTest portTest;
    PortTest(controller, &portTest);

    // Check the port test result
    if (portTest != PS2_PortTest_Passed)
    {
    	// Something is wrong with Port 1
        cio_printf("FAILED!\n\tReason: ");
        switch (portTest)
        {
            case PS2_PortTest_Clk_Stuck_Low:
                cio_printf("Clock line stuck low\n");
                break;
            case PS2_PortTest_Clk_Stuck_High:
                cio_printf("Clock line stuck high\n");
                break;
            case PS2_PortTest_Data_Stuck_Low:
                cio_printf("Data line stuck low\n");
                break;
            case PS2_PortTest_Data_Stuck_High:
                cio_printf("Data line stuck high\n");
                break;
            default:
                cio_printf("Unknown\n");
                break;
        }
        
	return -100 - (int)portTest;  // Port test failed
    }
    cio_printf("PASSED!\n");

    // Enable port 1
    if ((ret = PS2Controller_ProcessCommand(controller,
    	PS2_CMD_ENABLE_PORT1, 0, NULL)))
    {
        return ret;
    }

    return 0;	// Success
}

