/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 18 December 2012

  Copyright Lynx Innovation 2012

---------------------------------------------------------------------------*/

//====================== Public vars ==========================================

#include <avr/pgmspace.h>
#include "Globals.h"
#include "Timer.h"
#include "UART.h"


// Settings vars, mirrored in EEPROM

/*
TProdDef        ProdDef[UnknownProduct+1];
Uint16          IdlePeriod;
Uint8           LeftProduct;
Uint8           RightProduct;
 */

// Comms buffers

Uint8           TxBuf[BCMsgSize];
Uint8           RxBuf[BCMsgSize];

bool            SlaveMode;

//====================== Functions ============================================


/*
  CheckForBoardMsg routine

  This routine will check is new message is present, display it and do default
  processing if it is.
 */


/*
  ExchangeBoardMsg routine

  This routine sends a message to a bay and waits for an answer.  It will retry
  upto 3 times until the require response is received.  Other types of messages
  will be sent to ProcessBoardMsg for handling.
 */
bool ExchangeBoardMsg(Uint8 Address, enum TBCT BCTSend, Uint8 Param1, Uint8 Param2, enum TBCT BCTExpect)
{
  Uint8 Retry;
  Uint8 Pos;


  BCMessageReceive(RxBuf);      // Clear off any existing message and start again
  for(Retry = 0; Retry < 3; Retry++) {
    /*
    Timer_Clear();
    while (Timer_Read() < 10)     // Wait a while between frames
      MP3_Process();
     */
    // Send the required message to the other end

    TxBuf[BCPAddr]   = (BCAMP3Contoller << 4) | Address;
    TxBuf[BCPType]   = BCTSend;
    TxBuf[BCPParam1] = Param1;
    TxBuf[BCPParam2] = Param2;
    BCMessageSend(TxBuf, true);

    Timer_Clear();                    // Frames take about 2mS to send
    do {
      if (BCRXAvail) {                // See if we have a new frame
#ifdef DumpComms
        UART_TxStr("Received (e) ");
        for (Pos = 0; Pos < BCMsgSize; Pos++) {
          UART_TxChar(' ');
          UART_TxUint8(RxBuf[Pos]);
        }
        UART_TxNewLine();
#endif
        if ((RxBuf[BCPAddr] == ((Address << 4) | BCAMP3Contoller)) &&               // See if it for us and from them
            (RxBuf[BCPType] == BCTExpect)) {                                        // and is the response we want
          return true;
        }
        //ProcessBoardMsg();            // Default processing for other messages
        //BCMessageReceive(RxBuf);      // Try again
      }
    } while (Timer_Read() < 100);       // Wait several milliseconds for a reply


    // No reply return default

#ifdef DumpComms
    UART_TxStr("Retrying comms\r\n");
#else
    if (BCTSend != BCTInquire) {
      UART_TxStr("Retrying comms:");
      for (Pos = 0; Pos < BCMsgSize; Pos++) {
        UART_TxChar(' ');
        UART_TxUint8(RxBuf[Pos]);
      }
      UART_TxStr("\r\n");
    }
#endif
  }

  // No reply return default

#ifdef DumpComms
  UART_TxStr("Response not seen\r\n");
#else
  if (BCTSend != BCTInquire)
    UART_TxStr("Response not seen\r\n");
#endif
  return false;
}


