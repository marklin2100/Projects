/*****************************************************************************

    BoardComms module

    This module implements communications between boards via RS485.
    The comms is half duplex 38400 8N1.

    The packet format is binary:
    0 - 00000001      SOH, Start character
    1 - SSSSDDDD      Source and destination nibbles
    2 - Type          Message type
    3 - Parameter 1   First parameter, if needed
    4 - Parameter 2   Second parameter, if needed
    5 - Checkum       Simple sum of preceding bytes

    David Annett, david@lynxinnovation.com, 11 December 2012

    (c) 2012 Lynx Innovation

 ****************************************************************************/

//========================== Includes ========================================

#include "CommonTypes.h"
#include "Globals.h"
#include "BoardComms.h"
#ifdef  SecondUART
#include "UART.h"
#endif
#include "Timer.h"
#ifdef DebugLamps
#include "LampDirect.h"
#endif

//========================== Constants =======================================


// If no second UART then map back to first UART

#ifndef  SecondUART
#define RXCIE1            RXCIE0
#define RXEN1             RXEN0
#define TXCIE1            TXCIE0
#define TXEN1             TXEN0
#define UCSR1B            UCSR0B
#define UCSR1C            UCSR0C
#define UCSZ10            UCSZ00
#define UCSZ11            UCSZ01
#define UBRR1L            UBRR0L
#define UBRR1H            UBRR0H
#define UDR1              UDR0
#define UDRIE1            UDRIE0
#define USART1_RX_vect    USART_RX_vect
#define USART1_TX_vect    USART_TX_vect
#define USART1_UDRE_vect  USART_UDRE_vect
#endif



//========================== Public vars =====================================

volatile bool    BCRXAvail;
volatile bool    BCTXBusy;


//========================== Vars ============================================

Uint8  *RXBuf;
Uint8  *TXBuf;
volatile Uint8   BCRXPos;   // Position in RXBuf
Uint8   BCTXPos;            // Position in TxBuf
Uint8   BCOurAddress;


//========================== Public functions ================================

/*
  BCMessageInit routine

  This routine prepares for board to board communications.  It is passed the
  address to use for sending
 */
void BCMessageInit(Uint8 OurAddress)
{
  ClearBit(RS485DEPort, RS485DE);       // Turn off RS485 driver
  BCRXAvail = false;
  BCTXBusy  = false;
  BCTXPos   = 0;
  BCRXPos   = 0;
  RXBuf     = NULL;
  BCOurAddress = OurAddress;

  UBRR1H = (uint8) BaudRate1 >> 8;          // Set the baud rate
  UBRR1L = (uint8) BaudRate1;

  UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1<<RXCIE1) | (1<<TXCIE1); // Enable UART RX, TX, RX interrupt and TX complete interrupt
  UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);   // Set up as 8N1
}



/*
  BCMessageSend routine

  This routine will send a board comms message.  If busy it will wait before
  sending.  It will fill in the header, sender and checksum.  The user callers
  should set up destination address, type and parameters.
 */
void BCMessageSend(Uint8 *TxMsg, bool WaitForSend)
{
  Uint8 Pos;
  Uint8 Checksum;


#ifdef  SecondUART
  if (BCTXBusy)
    UART_TxStr("BCMessageSend BCTXBusy\r\n");
  if (BCRXPos)
    UART_TxStr("BCMessageSend BCRXPos\r\n");
#endif

  while(BCTXBusy || BCRXPos);  // Wait for last TX to complete and RX idle

  // Build packet

#ifdef  SecondUART
#ifdef DumpComms
  UART_TxStr("BCMessageSend 01 ");
#endif
#endif
  TxMsg[BCPSOH]  = SOH;
  TxMsg[BCPAddr] = (TxMsg[BCPAddr] & 0b00001111) | (BCOurAddress << 4);
  Checksum = SOH;
  for (Pos = BCPAddr; Pos < BCPChecksum; Pos++) {
    Checksum += TxMsg[Pos];
#ifdef  SecondUART
#ifdef DumpComms
    UART_TxUint8(TxMsg[Pos]);
    UART_TxChar(' ');
#endif
#endif
  }
  TxMsg[BCPChecksum] = Checksum;
#ifdef  SecondUART
#ifdef DumpComms
  UART_TxUint8(Checksum);
  UART_TxStr("\r\n");
#endif
#endif

  // Send it

  SetBit (RS485DEPort, RS485DE);      // Turn on RS485 driver
  DelayMS(1);
  BCTXBusy = true;
  TXBuf    = TxMsg;  // Set up the buffer and kick off the actual transmission
  BCTXPos  = 0;
  UCSR1B   |= (1<<UDRIE1);  // Enable UDRE and TXC interrupts

  if(WaitForSend)
    while(BCTXBusy);  // Wait for last TX to complete
}



/*
  BCMessageReceive routine

  This routine will allow the reception a board comms message.
 */
void BCMessageReceive(Uint8 *RxMsg)
{
  BCRXPos   = 0;
  RXBuf     = RxMsg;
  BCRXAvail = false;
}



//========================== Private functions ===============================



//========================== Interrupt handlers ==============================

/*
  USART1 receive interrupt routine

  This routine will handle a receive interrupt.
 */
ISR(USART1_RX_vect)
{
  Uint8 RxData;
  Uint8 Checksum;
  Uint8 Pos;

  RxData = UDR1;  // Get the incoming byte

  if ((RXBuf != NULL) && (BCRXPos < BCMsgSize) && !BCRXAvail) {
    RS485RXTimeout = 2;
    RXBuf[BCRXPos] = RxData;
    if (!((BCRXPos == BCPSOH) && (RxData != SOH))) { // Check for bad start of frame
      if (BCRXPos >= (BCMsgSize-1)) { // If end of frame reached
        RS485RXTimeout = 0;     // stop interpacket timeout
        Checksum = SOH;
        for (Pos = BCPAddr; Pos < BCPChecksum; Pos++)
          Checksum += RXBuf[Pos];
        if (Checksum == RxData) { // If checksum is good
          if ((RXBuf[BCPAddr] & 0b00001111) == BCOurAddress) {  // Check if for us
            BCRXAvail = true;       // then advise new frame is ready
#ifdef DebugLamps
            Debug1 = !Debug1;
#endif
          } else {
          }
        } else {
        }
        BCRXPos = 0;              // Let the transmit routines know we are not in frame
      } else {
        BCRXPos++;                // We saved a valid packet character so move on to next
      }
    } else {
    }
  }
}



/*
  USART1 transmit interrupt routine

  This routine will handle a transmit interrupt.
 */
ISR(USART1_UDRE_vect)
{
  if (BCTXPos < BCMsgSize) {  // If frame not complete
    UDR1 = TXBuf[BCTXPos];
    BCTXPos++;
  } else {
    UCSR1B &= ~(1<<UDRIE1);         // Disable UDRE interrupt
  }
}



/*
  USART1 transmit interrupt complete routine

  This routine will handle a transmit complete interrupt.
 */
ISR(USART1_TX_vect)
{
  RS485TXDrop = 1;  // Set the transmit driver to turn off in 1mS
}
