/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
// *****************************************************************************
#define SWITCH_STATE_PRESSED 0
#define SWITCH_STATE_RELEASED 1
// *****************************************************************************
volatile bool isTmr1Expired;
volatile bool isUARTTxComplete;
volatile bool changeTempSamplingRate;
// *****************************************************************************
static void SW1_User_Handler(GPIO_PIN pin, uintptr_t context)
{
    if(SWITCH1_Get() == SWITCH_STATE_PRESSED)
    {
        changeTempSamplingRate = true;      
    }
}
// *****************************************************************************
static void tmr1EventHandler (uint32_t intCause, uintptr_t context)
{
    static uint count = 0;
    count++;
    if (context < count)
    {
        count = 0;
        isTmr1Expired = true;                              
    }
}
// *****************************************************************************
static void UARTDmaChannelHandler(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        isUARTTxComplete = true;
    }
}
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    isTmr1Expired = false;
    isUARTTxComplete = false;
    changeTempSamplingRate = false;
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, UARTDmaChannelHandler, 0);
    
    GPIO_PinInterruptCallbackRegister(SWITCH1_PIN, SW1_User_Handler, 0);
    GPIO_PinInterruptEnable(SWITCH1_PIN);
    
    TMR1_CallbackRegister(tmr1EventHandler, 500);
    TMR1_Start();
    TMR1_InterruptEnable();
    
    while ( true )
    {
        if (isTmr1Expired)
        {
            isTmr1Expired = false;
            LED1_Toggle();
        }
        if (changeTempSamplingRate)
        {
            changeTempSamplingRate = false;
            LED2_Toggle();
        }
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

