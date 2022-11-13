/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    tcp_communication.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "TCP_COMMUNICATION_Initialize" and "TCP_COMMUNICATION_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "TCP_COMMUNICATION_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _TCP_COMMUNICATION_H
#define _TCP_COMMUNICATION_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"

#include "config/default/library/tcpip/tcpip.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    TCP_COMMUNICATION_STATE_INIT=0,
    TCP_COMMUNICATION_STATE_CHECK_NET_READY,
    TCP_COMMUNICATION_STATE_START_UDP_LISTENER,
    TCP_COMMUNICATION_STATE_TRY_TO_CONNECT_TO_SERVER,
    TCP_COMMUNICATION_STATE_CHECK_CONNECTION,
    TCP_COMMUNICATION_STATE_WAIT_NEW_CONNECTION,
    TCP_COMMUNICATION_STATE_SERVER_CONNECTION_HANDLER,
    
    

} TCP_COMMUNICATION_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    TCP_COMMUNICATION_STATES state;

    TCP_SOCKET mrqSocket ;
    
    TCP_SOCKET UDPsocket ;
    
    IP_MULTI_ADDRESS server_IP;
    
    /* TODO: Define any additional data used by the application. */

} TCP_COMMUNICATION_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TCP_COMMUNICATION_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    TCP_COMMUNICATION_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TCP_COMMUNICATION_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void TCP_COMMUNICATION_Initialize ( void );


/*******************************************************************************
  Function:
    void TCP_COMMUNICATION_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TCP_COMMUNICATION_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void TCP_COMMUNICATION_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _TCP_COMMUNICATION_H */

/*******************************************************************************
 End of File
 */

