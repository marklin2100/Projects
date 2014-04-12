/*****************************************************************************

    UARTPolled module

    This module implements polled UART related software.

    David Annett, david@lynxinnovation.com, 30 May 2011

    (c) 2011 Lynx Innovation

 ****************************************************************************/

#include "CommonTypes.h"
#include "Globals.h"
#include "UARTPolled.h"

//=============================== Constants ===================================
#if UseUART
#define BaudRate0 ((F_CPU/(16*PCBaudRate)-1L)+0.5L)
#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message "The value of ABC: " XSTR(BaudRate0)
/*
  UARTInit routine

  This routine prepares the UART for use.
 */
void UARTInit(void)
{
  UBRR0H = (uint8) BaudRate0 >> 8;       // Set the baud rate
  UBRR0L = (uint8) BaudRate0;
//  UBRR0H = 0;       // Set the baud rate
//  UBRR0L = 10;      // 115200
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);    // Enable UART receiver and transmitter
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Set up as 8N1
}



/*
  UARTTxByte routine

  Sends a byte via the serial port.
 */
void UARTTxByte(uint8 Data)
{
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = Data;
}



/*
  UARTTxHexByte routine

  Send a hex pair out serial port
*/
void UARTTxHexByte(Uint8 Data)
{
  static char Hex[16] = "0123456789abcdef";


  UARTTxByte(Hex[Data >> 4]);
  UARTTxByte(Hex[Data & 0xf]);
}



/*
  UARTTxStr routine

  This routine sends a string out the serial port.
*/
void UARTTxStr(const char *s)
{
  for (; *s; s++) {
    UARTTxByte(*s);
  }
}



/*
  UARTTxNum routine

  Sends a number, in decimal via the serial port.
 */
void UARTTxNum(Uint16 Num)
{
  bool Leading;


  if (Num > 10000) {
    UARTTxByte('0' + Num / 10000);
    Num %= 10000;
    Leading = true;
  } else {
    Leading = false;
  }

  if (Leading || (Num >= 1000)) {
    UARTTxByte('0' + Num / 1000);
    Num %= 1000;
    Leading = true;
  }

  if (Leading || (Num >= 100)) {
    UARTTxByte('0' + Num / 100);
    Num %= 100;
    Leading = true;
  }

  if (Leading || (Num >= 10)) {
    UARTTxByte('0' + Num / 10);
    Num %= 10;
    Leading = true;
  }

  UARTTxByte('0' + Num);
}



/*
  UARTRxByte routine

  This routine receives a byte at 38400 8N1 via the serial port from other boards.
  It returns 0 if no data so CAN NOT BE USED FOR BINARY PROTOCOLS.
  It will not wait for data.
 */
/*Uint8 UARTRxByte(void)
{
  if (UCSR0A & (1 << RXC0))   // Test if RX data available
    return UDR0;              // Return it if it is
  else
    return 0;                 // Otherwise return 0
}
*/

#else
void UARTInit() {}
#endif
