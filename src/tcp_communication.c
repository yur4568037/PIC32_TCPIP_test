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
#include "peripheral/nvm/plib_nvm.h"

#define READ_WRITE_SIZE         (NVM_FLASH_PAGESIZE)
//#define BUFFER_SIZE             (READ_WRITE_SIZE / sizeof(uint32_t))
#define BUFFER_SIZE             (88)

#define APP_FLASH_ADDRESS       (NVM_FLASH_START_ADDRESS + (NVM_FLASH_SIZE / 2) + (NVM_FLASH_SIZE / 4))

// NVM defines
#define DHCP        0
#define STATIC_IP   1

// NVM variables
//static volatile bool xferDone = false;

uint32_t writeData[BUFFER_SIZE] CACHE_ALIGN;
uint32_t readData[BUFFER_SIZE];

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

/*
static void eventHandler(uintptr_t context)
{
    xferDone = true;
}
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


bool check_CRC_32(uint32_t* buf, uint32_t length)
{
    //uint32_t read_CRC = buf[length-1];
    uint32_t read_CRC = 0;
    uint32_t calculate_CRC = 0;
    uint8_t $_counter = 0;
    
    for(uint32_t i = 0; i < length; i++)
    {
        if(buf[i] == (uint32_t)'$')
        {
            $_counter++;
            
            if($_counter == 8)
                break;
            if($_counter == 7)
            {
                read_CRC = buf[i+1];
                i++;
            }
        }
        else
            calculate_CRC += buf[i];
    }
        
    if(read_CRC == calculate_CRC && $_counter == 8)
        return true;
    else
        return false;
}

uint32_t calc_CRC_32(uint32_t* buf, uint32_t length)
{
    uint32_t result = 0;
    
    for(uint32_t i = 0; i < length; i++)
    {
        if(buf[i] != (uint32_t)'$')
            result += buf[i];
    }
    
    return result;
}

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

    // read NVM
    uint32_t address = APP_FLASH_ADDRESS;
    uint8_t *writePtr = (uint8_t *)writeData;
    
    //NVM_CallbackRegister(eventHandler, (uintptr_t)NULL);
    
    while(NVM_IsBusy() == true);
    
    // Read flash data
    NVM_Read(readData, sizeof(readData), APP_FLASH_ADDRESS);
    
    // Check CRC
    if(check_CRC_32(readData, BUFFER_SIZE))
    {
        // DHCP check
        if(readData[1] == DHCP)
            return;     // set DHCP default 
        
        // save new data and renew TCP interface
        char new_ip[15] = {0};
        char new_mask[15] = {0};
        char new_gw[15] = {0};
        char new_dns1[15] = {0};
        char new_dns2[15] = {0};
        
        uint8_t $_counter = 0;
        uint8_t pointer = 0;
        
        for(uint8_t i = 3; i < BUFFER_SIZE; i++)
        {
            if($_counter == 5)  // without CRC
                break;
            
            if(readData[i] == (uint32_t)'$')
            {
                $_counter++;
                pointer = 0;
            }
            else
            {
                switch($_counter)
                {
                    case 0:
                        new_ip[pointer++] = readData[i];
                        break;
                        
                    case 1:
                        new_mask[pointer++] = readData[i];
                        break;
                        
                    case 2:
                        new_gw[pointer++] = readData[i];
                        break;
                        
                    case 3:
                        new_dns1[pointer++] = readData[i];
                        break;
                        
                    case 4:
                        new_dns2[pointer++] = readData[i];
                        break;
                }
            }
        }
        
        const TCPIP_NETWORK_CONFIG __attribute__((unused))  new_config[] =
        {
            {
                .interface = "ETHMAC",
                .hostName = "MCHPBOARD_E",
                .macAddr = 0,
                .ipAddr = new_ip,
                .ipMask = new_mask,
                .gateway = new_gw,
                .priDNS = new_dns1,
                .secondDNS = new_dns2,
                .powerMode = TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,
                .startFlags = TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON | TCPIP_NETWORK_CONFIG_IP_STATIC,
                .pMacObject = &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,
            },
        };
        
        TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("PIC32INT");
        TCPIP_STACK_NetDown(hNet);
        TCPIP_STACK_NetUp(hNet, new_config);
    }
    else
    {
        // Erase the Page
        NVM_PageErase(address);
        //while(xferDone == false);
        //xferDone = false;
        while(NVM_IsBusy() == true);
        
        // write a new buffer
        writeData[0] = (uint32_t)'$';
        writeData[1] = DHCP;
        for(uint8_t i = 2; i < 15; i++)
        {
            if(i%2 == 0)
                writeData[i] = (uint32_t)'$';
            else
                writeData[i] = 0;
        }
        NVM_RowWrite((uint32_t *)writePtr, address);
    }   
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
    
    // set static IP
    if(
        (char) buf[3] == 'I' &&
        (char) buf[4] == 'P'    )
    {
        // parse data
        char new_ip[15] = {0};
        char new_mask[15] = {0};
        char new_gw[15] = {0};
        char new_dns1[15] = {0};
        char new_dns2[15] = {0};
        
        writeData[0] = (uint32_t)'$';
        writeData[1] = STATIC_IP;
        writeData[2] = (uint32_t)'$';
        
        uint8_t $_counter = 0;
        uint8_t pointer = 0;
        
        for(uint8_t i = 5; i < length; i++)
        {
            // start from 3
            writeData[i-2] = buf[i];
        
            if(buf[i] == (uint32_t)'$')
            {
                $_counter++;
                pointer = 0;
            }
            else
            {
                switch($_counter)
                {
                    case 0:
                        new_ip[pointer++] = buf[i];
                        break;
                        
                    case 1:
                        new_mask[pointer++] = buf[i];
                        break;
                        
                    case 2:
                        new_gw[pointer++] = buf[i];
                        break;
                        
                    case 3:
                        new_dns1[pointer++] = buf[i];
                        break;
                        
                    case 4:
                        new_dns2[pointer++] = buf[i];
                        break;
                }
            }
        }
        
        // CRC
        writeData[length-2] = (uint32_t)'$';
        writeData[length-1] = calc_CRC_32(writeData, length-2);
        writeData[length] = (uint32_t)'$';
        
        const TCPIP_NETWORK_CONFIG __attribute__((unused))  new_config[] =
        {
            {
                .interface = "ETHMAC",
                .hostName = "MCHPBOARD_E",
                .macAddr = 0,
                .ipAddr = new_ip,
                .ipMask = new_mask,
                .gateway = new_gw,
                .priDNS = new_dns1,
                .secondDNS = new_dns2,
                .powerMode = TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,
                .startFlags = TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON | TCPIP_NETWORK_CONFIG_IP_STATIC,
                .pMacObject = &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,
            },
        };
        
        TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("PIC32INT");
        TCPIP_STACK_NetDown(hNet);
        TCPIP_STACK_NetUp(hNet, new_config);
        
        // save new data
        uint32_t address = APP_FLASH_ADDRESS;
        uint8_t *writePtr = (uint8_t *)writeData;
        
        while(NVM_IsBusy() == true);
        NVM_PageErase(address);
        //while(xferDone == false);
        //xferDone = false;
        while(NVM_IsBusy() == true);
        
        NVM_RowWrite((uint32_t *)writePtr, address);
    }
    
    // Set DHCP
    if(
        (char) buf[3] == 'D' &&
        (char) buf[4] == 'H' &&    
        (char) buf[5] == 'C' &&
        (char) buf[6] == 'P'    )
    {
        const TCPIP_NETWORK_CONFIG __attribute__((unused))  new_config[] =
        {
            /*** Network Configuration Index 0 ***/
            {
                .interface = TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0,
                .hostName = TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0,
                .macAddr = TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0,
                .ipAddr = TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0,
                .ipMask = TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0,
                .gateway = TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0,
                .priDNS = TCPIP_NETWORK_DEFAULT_DNS_IDX0,
                .secondDNS = TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0,
                .powerMode = TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,
                .startFlags = TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0,
                .pMacObject = &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,
            },
        };
        
        TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("PIC32INT");
        TCPIP_STACK_NetDown(hNet);
        TCPIP_STACK_NetUp(hNet, new_config);
        
        // save new data to NVM
        // Erase the Page
        uint32_t address = APP_FLASH_ADDRESS;
        uint8_t *writePtr = (uint8_t *)writeData;
        
        while(NVM_IsBusy() == true);
        NVM_PageErase(address);
        //while(xferDone == false);
        //xferDone = false;
        while(NVM_IsBusy() == true);
        
        // write a new buffer
        writeData[0] = (uint32_t)'$';
        writeData[1] = DHCP;
        for(uint8_t i = 2; i < 15; i++)
        {
            if(i%2 == 0)
                writeData[i] = (uint32_t)'$';
            else
                writeData[i] = 0;
        }
        NVM_RowWrite((uint32_t *)writePtr, address);
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
                    //SYS_CONSOLE_MESSAGE("INVALID_SOCKET \r\n");
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
                
                rx_buffer[rx_lenght] = (uint8_t)'\r';
                rx_buffer[rx_lenght+1] = (uint8_t)'\n';
                
                for(uint16_t i = 0; i < rx_lenght + 2; i++)
                    rx_char_buffer[i] = (char) rx_buffer[i];
                
                SYS_CONSOLE_MESSAGE(rx_char_buffer);
                
                parsing_rx_tcp(rx_buffer, rx_lenght);
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
