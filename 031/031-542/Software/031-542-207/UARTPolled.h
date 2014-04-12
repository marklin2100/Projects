#ifndef uartpolled_h
#define uartpolled_h
/*****************************************************************************

    UARTPolled module

    This module implements polled UART related software.

    David Annett, david@lynxinnovation.com, 30 May 2011

    (c) 2011 Lynx Innovation

****************************************************************************/

#include "CommonTypes.h"
#include "Globals.h"



#define BaudRate  38400



/*
  UARTInit routine

  This routine prepares the UART for use.
*/
void UARTInit(void);


/*
  UARTTxByte routine

  Sends a byte at 38400 8N1 via the serial port.
*/
void UARTTxByte(uint8 Data);


/*
  UARTTxNum routine

  Sends a number, in decimal via the serial port.
*/
void UARTTxNum(uint16 Num);


/*
  UARTRxByte routine

  This routine receives a byte at 38400 8N1 via the serial port from other boards.
  It returns 0 if no data so CAN NOT BE USED FOR BINARY PROTOCOLS.
  It will not wait for data.
*/
Uint8 UARTRxByte(void);

#endif
