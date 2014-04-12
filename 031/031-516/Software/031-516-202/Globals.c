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
#include "MP3.h"
#include "UART.h"
#include "Keyboard.h"
#include "LampSerial.h"

// Settings vars, mirrored in EEPROM

TSettings Settings;



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
    {      //47 == mute
        {   83,  80, 100, 125}, // 0 Five headphones
        {   47,  80, 102, 125}, // 1 Pill speaker
        {   47,  80,  92, 125}, // 2 Beat Box speaker
        {    0,  80, 100, 127}, // 3 Reserved for future products
        {    0,  80, 100, 127}, // 4 Reserved for future products
        {    0,  80, 100, 127}, // 5 Reserved for future products
        {    0,  80, 100, 127}, // 6 Reserved for future products
        {    0,  80, 100, 127}, // 7 Reserved for future products
        {    0,  80, 100, 127}, // 8 A media player source
        {    0,   0,   0,   0}  // 9 An unknown product
    },
    3,                        // 3 volume steps per key press
    3,                        // 3 volume steps per ramp period
    UnknownProduct,           // Don't force product, use as reported by comms
    450,                      // Idle timeout is 45s
    0xFF,                     // Idle input set by coded logic
    0xFF,                     // Default active input set by coded logic
    0,                        // Bass control off
    8,                        // Bass frequency 80Hz
    0,                        // Treble control off
    8                         // Treble frequency 8kHz
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
    UART_TxStr("\r\n");
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
      UART_TxStr("\r\n");
      break;

    case BCTNAck: // Negative acknowledge
      UART_TxStr("NAck from ");
      UART_TxNum(RxBuf[BCPAddr] >> 4, 1);
      UART_TxStr(" because ");
      UART_TxNum(RxBuf[BCPParam1], 1);
      UART_TxChar(' ');
      UART_TxNum(RxBuf[BCPParam2], 1);
      UART_TxStr("\r\n");
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
        SetMux(RxBuf[BCPParam1] & 0b000000011);
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
        UART_TxStr("\r\n");
#endif
        if ((RxBuf[BCPAddr] == ((Address << 4) | BCAMP3Contoller)) &&               // See if it for us and from them
            (RxBuf[BCPType] == BCTExpect)) {                                        // and is the response we want
          return true;
        }
        ProcessBoardMsg();            // Default processing for other messages
        BCMessageReceive(RxBuf);      // Try again
      }
      MP3_Process();
    } while (Timer_Read() < 100);       // Wait several milliseconds for a reply


    // No reply return default

#ifdef DumpComms
    UART_TxStr("Retrying comms\r\n");
#else
    if (BCTSend != BCTInquire) {
      UART_TxStr("Retrying comms:");
      for (Pos = 0; Pos < BCMsgSize; Pos++) {
        UART_TxChar(' ');
        UART_TxUint8(TxBuf[Pos]);
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



/*
  SetMux routine

  This routine sets the input muxes for a bay.  No checks are made, use
  SetInput for safe version.
 */
void SetMux(enum TInput Source)
{
  UART_TxStr("SetMux ");
  UART_TxNum(Source, 1);
  UART_TxStr("\r\n");

  switch (Source) {
    case MP3In:
      ClearBit(SelAPort, Sel0A);
      ClearBit(SelAPort, Sel1A);
      break;

    case LCDIn:
      SetBit(SelAPort, Sel0A);
      ClearBit(SelAPort, Sel1A);
      break;
  }
}


/*
  ShowError routine

  Display and error number pattern on the lamps and reboot if second parameter true
*/
void ShowError(Uint8 ErrorCode, bool Reboot)
{
  Uint8 ShownCount;
  Uint8 TempInt;


  ShownCount = 0;

  wdt_reset();
  UART_TxStr("ShowError ");
  UART_TxNum(ErrorCode, 1);
  if (Reboot)
    UART_TxStr(", rebooting");
  UART_TxNewLine();
  wdt_reset();

  SetLamps (0b0);  // Show '-'
  DelayMS(100);
  wdt_reset();
  for (;;) {
    // Show an intro pattern
    for (TempInt = 1; TempInt <= 5; TempInt++) {
      if (!Reboot)
        wdt_reset();
      SetLamps (0b00000101);  // Show '-'
      DelayMS(200);
      SetLamps (0b00001010);  // Show '|'
      DelayMS(200);
    }

    // Show the error code
    if (!Reboot)
      wdt_reset();
    SetLamps(ErrorCode);
    DelayMS(7000);

    // See if we are done yet

    if ((++ShownCount > 4) && !Reboot)
      return;
  }
}

