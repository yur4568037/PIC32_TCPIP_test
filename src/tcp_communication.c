/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    tcp_communication.c

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

#include <stdint.h>
#include <string.h>

#include "tcp_communication.h"
#include "system/console/sys_console.h"
#include "tcpip/tcpip.h"
#include "peripheral/gpio/plib_gpio.h"

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
    This structure should be initialized by the TCP_COMMUNICATION_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TCP_COMMUNICATION_DATA tcp_communicationData;

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
    void TCP_COMMUNICATION_Initialize ( void )

  Remarks:
    See prototype in tcp_communication.h.
 */

void TCP_COMMUNICATION_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    tcp_communicationData.state = TCP_COMMUNICATION_STATE_INIT;

    SYS_CONSOLE_MESSAGE("Step 1\r\n");

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void TCP_COMMUNICATION_Tasks ( void )

  Remarks:
    See prototype in tcp_communication.h.
 */

uint32_t temp_counter = 0;

extern volatile uint8_t timer_F_2;

uint16_t tcp_connect_counter = 0;
uint16_t data_counter = 0;

uint8_t transmit_counter = 0;

void parsing_rx_tcp(uint8_t* buf, uint16_t length)
{
    if (length < 3)
        return;
    
    if(
        (char) buf[0] != 'C' ||
        (char) buf[1] != 'M' ||    
        (char) buf[2] != 'D'    )
    {
        return;
    }
    
    if(length < 7)
        return;
    
    if(
        (char) buf[3] == 'L' &&
        (char) buf[4] == 'E' &&    
        (char) buf[5] == 'D'    )
    {
        uint8_t led_num = buf[6] - 0x30;
        
        switch(led_num)
        {
            case 1:
                LED1_Toggle();
                break;
                
            case 2:
                LED2_Toggle();
                break;
                
            case 3:
                LED3_Toggle();
                break;
        }
    }
            
    
}   // void parsing_rx_tcp(uint8_t* buf, uint16_t length)


void TCP_COMMUNICATION_Tasks ( void )
{
    // 1 ms
    if(timer_F_2)   
    {
        timer_F_2 = 0;
        
        if(tcp_connect_counter)
        {
            tcp_connect_counter++;
        }
        
        if(data_counter)
        {
            data_counter++;
        }
    }   // if(timer_F_2)   
    
    /* Check the application's current state. */
    switch ( tcp_communicationData.state )
    {
        /* Application's initial state. */
        case TCP_COMMUNICATION_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {

                tcp_communicationData.state = TCP_COMMUNICATION_STATE_CHECK_NET_READY;
                
                tcp_communicationData.mrqSocket = INVALID_SOCKET;
             
                SYS_CONSOLE_MESSAGE("Step 2\r\n");
            }
            break;
        }

        case TCP_COMMUNICATION_STATE_WAIT_NEW_CONNECTION:
        {
            if(tcp_connect_counter >= 5000)
            {
                tcp_connect_counter = 0;
                
                tcp_communicationData.state = TCP_COMMUNICATION_STATE_CHECK_NET_READY;
            }
            break;
        }
        
        case TCP_COMMUNICATION_STATE_CHECK_CONNECTION:
        {
            if(tcp_connect_counter >= 500)
            {
                tcp_connect_counter = 0;
                
                if(tcp_communicationData.mrqSocket == INVALID_SOCKET || !TCPIP_TCP_IsConnected(tcp_communicationData.mrqSocket))
                {
                    SYS_CONSOLE_MESSAGE("INVALID_SOCKET \r\n");
                    TCPIP_TCP_Close(tcp_communicationData.mrqSocket);
                    tcp_communicationData.state = TCP_COMMUNICATION_STATE_TRY_TO_CONNECT_TO_SERVER;

                    break;
                }
                else
                {
                    SYS_CONSOLE_MESSAGE("Client opened port \r\n");
                    tcp_communicationData.state = TCP_COMMUNICATION_STATE_SERVER_CONNECTION_HANDLER;   
                }
            }
            
            break;
        }
        
        case TCP_COMMUNICATION_STATE_CHECK_NET_READY:
        {
            TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("PIC32INT");
            //TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("ETHMAC");
            
            
            temp_counter++;
            if(temp_counter >= 100000)
            {
                temp_counter = 0;
                SYS_CONSOLE_MESSAGE("test\r\n");
            }
            
            
            if(TCPIP_STACK_NetIsReady(hNet))
            {
                SYS_CONSOLE_MESSAGE("TCPIP_STACK_NetIsReady\r\n");
                                
                tcp_communicationData.state = TCP_COMMUNICATION_STATE_TRY_TO_CONNECT_TO_SERVER;
            }
            
            break;
        }
        
        case TCP_COMMUNICATION_STATE_TRY_TO_CONNECT_TO_SERVER:
        {
            
            IPV4_ADDR addr;
            //char pcip[] = "192.168.100.55";
            char pcip[] = "192.168.1.10";
            
            TCPIP_Helper_StringToIPAddress(pcip, &addr);
            
            // trying to connect
            tcp_communicationData.mrqSocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, 2323,
                    (IP_MULTI_ADDRESS*) &addr);
            
            tcp_connect_counter = 1;
            tcp_communicationData.state = TCP_COMMUNICATION_STATE_CHECK_CONNECTION;
            
            
            
            //data_counter = 1;
            
            /*
            TCPIP_TCP_Flush(tcp_communicationData.mrqSocket);
            if (TCPIP_TCP_Connect(tcp_communicationData.mrqSocket))
            {
                SYS_CONSOLE_MESSAGE("Client is connected to server \r\n");

                tcp_communicationData.state = TCP_COMMUNICATION_STATE_SERVER_CONNECTION_HANDLER;
            }
            */
            break;
        }

        case TCP_COMMUNICATION_STATE_SERVER_CONNECTION_HANDLER:
        {
            if(!TCPIP_TCP_IsConnected(tcp_communicationData.mrqSocket))
            {
                SYS_CONSOLE_MESSAGE("Client is disconnected \r\n");
                TCPIP_TCP_Close(tcp_communicationData.mrqSocket);
                tcp_communicationData.state = TCP_COMMUNICATION_STATE_TRY_TO_CONNECT_TO_SERVER;
                break;
            }
            
            // Receive TCP packet
            uint16_t rx_lenght = TCPIP_TCP_GetIsReady(tcp_communicationData.mrqSocket);
            if(rx_lenght > 0)
            {
                uint8_t rx_buffer[512] = {0};
                char rx_char_buffer[512] = {0};
                
                TCPIP_TCP_ArrayGet(tcp_communicationData.mrqSocket, rx_buffer, rx_lenght);
                
                SYS_CONSOLE_PRINT("Receive buffer. Length: %d\r\n", rx_lenght);
                
                parsing_rx_tcp(rx_buffer, rx_lenght);
                
                rx_buffer[rx_lenght] = (uint8_t)'\r';
                rx_buffer[rx_lenght+1] = (uint8_t)'\n';
                
                for(uint16_t i = 0; i < rx_lenght + 2; i++)
                    rx_char_buffer[i] = (char) rx_buffer[i];
                
                SYS_CONSOLE_MESSAGE(rx_char_buffer);
                
            }
            
            // Periodic transmit TCP packet
            /*
            if(data_counter >= 3000)
            {
                data_counter = 1;
                
                if(tcp_communicationData.mrqSocket == INVALID_SOCKET)
                {
                    SYS_CONSOLE_MESSAGE("INVALID_SOCKET \r\n");
                    tcp_connect_counter = 1;
                    tcp_communicationData.state = TCP_COMMUNICATION_STATE_WAIT_NEW_CONNECTION;
                    break;
                }
                
                
                TCPIP_TCP_ArrayPut(tcp_communicationData.mrqSocket, (uint8_t*)"123456", 6);
            
            }
            */
            
            break;
        }

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
