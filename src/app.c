/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "peripheral/tmr/plib_tmr2.h"
#include "peripheral/gpio/plib_gpio.h"
#include "system/debug/sys_debug.h"
#include "system/console/sys_console.h"
#include "usb/usb_device.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

volatile uint8_t timer_F = 0;
volatile uint8_t timer_F_2 = 0;

uint8_t sw1_flag = 0;
uint8_t sw2_flag = 0;
uint8_t sw3_flag = 0;

uint16_t sw1_counter = 0;
uint16_t sw2_counter = 0;
uint16_t sw3_counter = 0;

void Timer_Callback(uint32_t status, uintptr_t context)
{
    timer_F = 1;
    
    timer_F_2 = 1;
    /*
    timer_counter++;
    
    if(timer_counter >= 200)
    {
        timer_counter = 0;  
    }
    */
}

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    TMR2_CallbackRegister(Timer_Callback, 0);

    TMR2_Start();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {

            if(timer_F)
            {
                // buttons
                if(!SW1_Get() && sw1_counter < 50 )   sw1_counter++;
                else if(SW1_Get() && sw1_counter > 0) sw1_counter--;
                
                if(!SW1_Get() && sw1_counter < 50 )   sw1_counter++;
                else if(SW1_Get() && sw1_counter > 0) sw1_counter--;
                
                if(!SW2_Get() && sw2_counter < 50 )   sw2_counter++;
                else if(SW2_Get() && sw2_counter > 0) sw2_counter--;
                
                if(!SW3_Get() && sw3_counter < 50 )   sw3_counter++;
                else if(SW3_Get() && sw3_counter > 0) sw3_counter--;
                
                if(sw1_counter >= 50 && !sw1_flag)
                {
                    sw1_flag = 1;
                    
                    SYS_CONSOLE_MESSAGE("SW1 is pressed\r\n");
                    //SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "***This is USB Console Instance 0***\n\r");
                    //SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "Test Debug Message!\n\r");    
                    //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "Sys Print test %d, %s\n\r", 1, "str1");
                    //SYS_DEBUG_PRINT(SYS_ERROR_INFO, "Sys Print test %d, %s\n\r", 2, "str2");
                    /* Change the error level to only print the debug messages with error value set to SYS_ERROR_ERROR or lower */
                    //SYS_DEBUG_ErrorLevelSet(SYS_ERROR_ERROR);

                    /* The below message should not get printed as "SYS_ERROR_DEBUG" is higher than "SYS_ERROR_ERROR" */
                    //SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "This message should not be printed!\n\r");

                    /* Set the error level back to SYS_ERROR_DEBUG */
                    //SYS_DEBUG_ErrorLevelSet(SYS_ERROR_DEBUG);
                    
                    LED1_Set();
                }
                else if(sw1_counter == 0 && sw1_flag)
                {
                    sw1_flag = 0;
                    
                    SYS_CONSOLE_MESSAGE("SW1 is released\r\n");
                    LED1_Clear();
                }
                
                if(sw2_counter >= 50 && !sw2_flag)
                {
                    sw2_flag = 1;
                    
                    SYS_CONSOLE_MESSAGE("SW2 is pressed\r\n");
                    LED2_Set();
                }
                else if(sw2_counter == 0 && sw2_flag)
                {
                    sw2_flag = 0;
                    
                    SYS_CONSOLE_MESSAGE("SW2 is released\r\n");
                    LED2_Clear();
                }
                
                if(sw3_counter >= 50 && !sw3_flag)
                {
                    sw3_flag = 1;
                    
                    SYS_CONSOLE_MESSAGE("SW3 is pressed\r\n");
                    LED3_Set();
                }
                else if(sw3_counter == 0 && sw3_flag)
                {
                    sw3_flag = 0;
                    
                    SYS_CONSOLE_MESSAGE("SW3 is released\r\n");
                    LED3_Clear();
                }
                
                
                timer_F = 0;
            }
            
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
