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
#include "Keyboard.h"


// Settings vars, mirrored in EEPROM

TSettings Settings;
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

const TSettings DefaultSettings PROGMEM = {
    // Header
    PILynxID,
    PICustomer,
    PIProject,
    PIPartNumber,
    PISettingsVersion,
    0,
    0,
    0,
    0,
    0,  // Save routine will calculate checksum
    // Actual settings data
    {
        {100,  80, 100, 127}, // 0 Five headphones
        {  0,  80, 102, 125}, // 1 Pill speaker
        {  0,  80,  92, 125}, // 2 Beat Box speaker
        {  0,  80, 100, 127}, // 3 Reserved for future products
        {  0,  80, 100, 127}, // 4 Reserved for future products
        {  0,  80, 100, 127}, // 5 Reserved for future products
        {  0,  80, 100, 127}, // 6 Reserved for future products
        {  0,  80, 100, 127}, // 7 Reserved for future products
        {  0,  80, 100, 127}, // 8 A media player source
        {  0,   0,   0, 0}    // 9 An unknown product
    },
    3,                        // 3 volume steps per key press
    3,                        // 3 volume steps per ramp period
    UnknownProduct,           // Left bay product as reported by comms
    UnknownProduct,           // Center bay product as reported by comms
    UnknownProduct,           // Right bay product as reported by comms
    450,                      // Idle timeout is 2mins
    0xFF,                     // Left bay idle input set by coded logic
    0xFF,                     // Center bay idle input set by coded logic
    0xFF,                     // Right bay idle input set by coded logic
    0xFF,                     // Default bay set by coded logic
    0xFF,                     // Default active input set by coded logic
    0,                        // Left select key override not active
    0,                        // Center select key override not active
    0,                        // Right select key override not active
    0,                        // Bass control off
    8,                        // Bass frequency 80Hz
    0,                        // Treble control off
    8,                        // Treble frequency 8kHz
    true,                     // Play/pause key enabled
    {
        0b01010001,           // Lamp 6 off brightness is 3
        0b01100001,           // Lamp 7 off brightness is 3
        0xFF,                 // Lamp setting not used
        0xFF,                 // Lamp setting not used
        0xFF,                 // Lamp setting not used
        0xFF,                 // Lamp setting not used
        0xFF,                 // Lamp setting not used
        0xFF                  // Lamp setting not used
    },
    false,                    // Don't go back to track 1 on input selection change
    {                         // Settings for 4 pos, manual updated will be needed for 5 pos
        0,                    // Right hand side, Mixr, 0dB relative to other headphones
        0,                    // Solo 0dB relative to other headphones
        0,                    // Solo 0dB relative to other headphones
        0,                    // Studio 0dB relative to other headphones
        0                     // Executive 0dB relative to other headphones
    },
    {                         // Settings for 5 pos, manual updated will be needed for 4 pos
      125,                    // Right hand side, Mixr
      125,                    // Solo
#ifdef Headphone4PosDefault
      118,                    // Studio
      125,                    // Executive
#else
      125,                    // Solo
      118,                    // Studio
#endif
      125                     // Executive
    }
};

//====================== Functions ============================================


/*
  CheckForBoardMsg routine

  This routine will check is new message is present, display it and do default
  processing if it is.
 */
void CheckForBoardMsg(void)
{
#ifdef DumpComms
  Uint8   TempInt;
#endif


  if (BCRXAvail) {                // See if we have a new frame
#ifdef DumpComms
    UART_TxStr("Received (p)  ");
    for (TempInt = 0; TempInt < BCMsgSize; TempInt++) {
      UART_TxChar(' ');
      UART_TxUint8(RxBuf[TempInt]);
    }
    UART_TxNewLine();
#endif
    ProcessBoardMsg();
    BCMessageReceive(RxBuf);      // Get the next message
  }
}



/*
  ProcessBoardMsg routine

  This routine performs default processing on an incoming message.
  The caller is responsible for calling BCMessageReceive(RxBuf) to get the
  next message;
 */
void ProcessBoardMsg(void)
{
  Uint8   Destination;        // Address we will reply to


  Destination = RxBuf[BCPAddr] >> 4;  // Preset up assuming we will reply
  Destination &= 0b1111;
  Destination |= BCAMP3Contoller << 4;
  TxBuf[BCPAddr] = Destination;
  DelayMS(2);                         // Allow line turn around delay
  switch (RxBuf[BCPType]) {
    case BCTAck:  // Acknowledge
      UART_TxStr("Unexpected Ack from ");
      UART_TxNum(RxBuf[BCPAddr] >> 4, 1);
      UART_TxNewLine();
      break;

    case BCTNAck: // Negative acknowledge
      UART_TxStr("NAck from ");
      UART_TxNum(RxBuf[BCPAddr] >> 4, 1);
      UART_TxStr(" because ");
      UART_TxNum(RxBuf[BCPParam1], 1);
      UART_TxChar(' ');
      UART_TxNum(RxBuf[BCPParam2], 1);
      UART_TxNewLine();
      break;

    case BCTKey: // Received a key press
      if (!SlaveMode) {
        ProcessCommsKey(RxBuf[BCPParam1]);
        TxBuf[BCPType]   = BCTAck;
        TxBuf[BCPParam1] = 0;
        TxBuf[BCPParam2] = 0;
        BCMessageSend(TxBuf, true);           // Send the reply
      }
      break;

    case BCTSlaveMode: // Handle slave mode
      if (RxBuf[BCPParam1] & 0b11110000) {
        SlaveMode = true;
        UART_TxStr("Slave mode\r\n");
        SetMux((RxBuf[BCPParam1] & 0b000001100) >> 2, LeftBay);
        SetMux(RxBuf[BCPParam1] & 0b000000011, RightBay);
      } else {
        SlaveMode = false;
        UART_TxStr("Normal mode\r\n");
      }
      TxBuf[BCPType]   = BCTAck;
      TxBuf[BCPParam1] = 0;
      TxBuf[BCPParam2] = 0;
      BCMessageSend(TxBuf, true);           // Send the reply
      break;

    default:  // Unknown command
      TxBuf[BCPType]   = BCTNAck;
      TxBuf[BCPParam1] = BCNUnkownType;
      TxBuf[BCPParam2] = RxBuf[BCPType];
      BCMessageSend(TxBuf, true);           // Send the reply
      break;
  }

}



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
        ProcessBoardMsg();            // Default processing for other messages
        BCMessageReceive(RxBuf);      // Try again
      }
      //MP3_Process();
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
      UART_TxNewLine();
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



/*
  SetMux routine

  This routine sets the input muxes for a bay.  No checks are made, use
  SetInput for safe version.
 */
void SetMux(enum TInput Source, enum TBay SIBay)
{
  UART_TxStr("SetMux ");
  UART_TxNum(Source, 1);
  UART_TxStr(" for bay ");
  UART_TxNum(SIBay, 1);
  UART_TxStr("\n\r");

  if (SIBay == LeftBay) {
    switch (Source) {
      case MP3In:
        ClearBit(SelAPort, Sel0A);
        ClearBit(SelAPort, Sel1A);
        break;

      case LCDIn:
        SetBit(SelAPort, Sel0A);
        ClearBit(SelAPort, Sel1A);
        break;

      case LeftTablet:
        ClearBit(SelAPort, Sel0A);
        SetBit(SelAPort, Sel1A);
        break;

      case RightTablet:
        SetBit(SelAPort, Sel0A);
        SetBit(SelAPort, Sel1A);
        break;
    }
  } else if (SIBay == CenterBay) {
    switch (Source) {
      case MP3In:
        ClearBit(SelBPort, Sel0B);
        ClearBit(SelBPort, Sel1B);
        break;

      case LCDIn:
        SetBit(SelBPort, Sel0B);
        ClearBit(SelBPort, Sel1B);
        break;

      case LeftTablet:
        ClearBit(SelBPort, Sel0B);
        SetBit(SelBPort, Sel1B);
        break;

      case RightTablet:
        SetBit(SelBPort, Sel0B);
        SetBit(SelBPort, Sel1B);
        break;
    }

  } else if (SIBay == RightBay) {
    switch (Source) {
      case MP3In:
        ClearBit(SelCPort, Sel0C);
        ClearBit(SelCPort, Sel1C);
        break;

      case LCDIn:
        SetBit(SelCPort, Sel0C);
        ClearBit(SelCPort, Sel1C);
        break;

      case LeftTablet:
        ClearBit(SelCPort, Sel0C);
        SetBit(SelCPort, Sel1C);
        break;

      case RightTablet:
        SetBit(SelCPort, Sel0C);
        SetBit(SelCPort, Sel1C);
        break;
    }
  }
}
