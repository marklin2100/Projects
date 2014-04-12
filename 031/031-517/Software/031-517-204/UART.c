/*****************************************************************************

    UART module

    This module implements UART related software for the ATmega164/324/644 AVRs

    David Annett, david@annett.co.nz, 12 July 2008

****************************************************************************/

// ------- Includes ----------

#include "Globals.h"
#include "CommonTypes.h"
#include "UART.h"
#include "Timer.h"
#include <avr/interrupt.h>



// ------- Constants ---------


// ------- Types -------------


// ------- Public vars -------

volatile Uint8  TxUnsent;
volatile Uint8  RxAvail;
#ifdef  SecondUART
volatile Uint8  Tx1Unsent;
volatile Uint8  Rx1Avail;
#endif

// ------- Private vars ------

volatile Uint8 *TxPtr;
volatile Uint8 *RxPtr;
volatile Uint8  RxFree;

// ------- Private functions -

/*
  USART0 transmit interrupt routine

  This routine will handle a transmit interrupt.
*/
ISR(USART0_UDRE_vect)
{
  if (TxUnsent) {
    TxUnsent--;
    UDR0 = *TxPtr++;
  } else {
    UCSR0B &= ~(1<<UDRIE0);         // Disable UDRE interrupt
  }
}



/*
  USART0 receive interrupt routine

  This routine will handle a receive interrupt.
*/
ISR(USART0_RX_vect)
{
  Uint8 RxData;


  RxData = UDR0;  // Get the incoming byte

  if (RxFree) {
    RxFree--;
    *RxPtr++ = RxData;
    RxAvail++;
  }
}



// ------- Public functions -

/*
  UART_Init routine

  This routine will init the serial port ready for use.
*/
void UART_Init(void)
{
  // UART 0

  UBRR0H = (uint8) BaudRate0 >> 8;                          // Set the baud rate
  UBRR0L = (uint8) BaudRate0;
/*
  UBRR0H = 0;                           // Set the baud rate
//  UBRR0L = 10;                          // 115200 with 20MHz xtal
  UBRR0L = 12;                          // 115200 with 24MHz xtal
*/
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0);       // Enable UART receiver and transmitter and rx interrupt
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);                   // Set up as 8N1
  RxFree = 0;
  TxUnsent = 0;

}



/*
    UART_Tx routine

    This routine will start the transmission of a buffer.
*/
void UART_Tx(Uint8 *TxBuff, Uint8 TxSize)
{
  TxUnsent = TxSize;
  TxPtr    = TxBuff;

  UCSR0B |= (1<<UDRIE0);  // Enable UDRE interrupt

  while (TxUnsent);       // Wait for send to complete
}



/*
    UART_Rx routine

    This routine will start the reception to a buffer.
*/
void UART_Rx(Uint8 *RxBuff, Uint8 RxSize)
{
  RxPtr = RxBuff;
  RxFree = RxSize;
  RxAvail = 0;
}



/*
    UART_TxChar routine

    This routine will s                  end an ASCII character.
*/
void UART_TxChar(char c)
{
  UART_Tx((Uint8 *)&c, 1);
}



/*
    UART_TxStr routine

    This routine will send a null terminated string.
*/
void UART_TxStr(const char *s)
{
  for (; *s; s++) {
    UART_TxChar(*s);
  }
}



/*
    UART_TxNum routine

    This routine will send a 32 bit int as a decimal string.
*/
void UART_TxNum(Uint32 c)
{
  Uint32 Modulo;
  Uint32 Digit;

  Modulo = 1000000000;

// Suppress leading zeros

  do {
    if (Modulo <= c)
      break;
    Modulo /= 10;
  } while (Modulo > 1);

// Print digits

  if (Modulo > 1)
    do {
      Digit = c / Modulo;
      UART_TxChar('0' + (char) Digit);
      c -= Digit * Modulo;
      Modulo /= 10;
    } while (Modulo > 1);

  UART_TxChar('0' + (char) c);
}


/*
    UART_TxUint8 routine

    This routine will send a byte as a hex string.
*/
void UART_TxUint8(Uint8 c)
{
  static char hex[16] = "0123456789abcdef";
  UART_TxChar(hex[c >> 4]);
  UART_TxChar(hex[c & 0xf]);
}



/*
    UART_TxUint16 routine

    This routine will send a 16 bit int as a hex string.
*/
void UART_TxUint16(Uint16 c)
{
  UART_TxUint8(c >> 8);
  UART_TxUint8(c & 0xff);
}



/*
    UART_TxUint32 routine

    This routine will send a 32 bit int as a hex string.
*/
void UART_TxUint32(Uint32 c)
{
  UART_TxUint8(c >> 24);
  UART_TxUint8(c >> 16);
  UART_TxUint8(c >> 8);
  UART_TxUint8(c & 0xff);
}
