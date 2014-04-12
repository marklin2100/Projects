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
#define BaudRate0   (F_CPU/(16L*BaudRate)-1)
//#define BaudRate0 129

/*
  UARTInit routine

  This routine prepares the UART for use.
*/
void UARTInit(void)
{
  UBRR0H = (uint8) BaudRate0 >> 8;       // Set the baud rate
  UBRR0L = (uint8) BaudRate0;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);    // Enable UART receiver and transmitter
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Set up as 8N1
}



/*
  UARTTxByte routine

  Sends a byte at 38400 8N1 via the serial port.
*/
void UARTTxByte(uint8 Data)
{
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = Data;
}



/*
  UARTTxNum routine

  Sends a number, in decimal via the serial port.
*/
void UARTTxNum(uint16 Num)
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
/*
Uint8 UARTRxByte(void)
{
  if (UCSRA & (1<<RXC))        // Test if RX data available
  return UDR;            // Return it if it is
  else
  return 0;            // Otherwise return 0
}
*/

#else
void UARTInit() {}
#endif
