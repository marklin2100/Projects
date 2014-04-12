/*
  031-517-201 program

  This program is for the Lynx Innovation 031-517-201 buttons and lamps board.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517
  It communicates with the 031-517-202 system control board. It reads button
  presses and lights up lamps.

  Created by David Annett 30 November 2012

  Copyright Lynx Innovation 2012
 */

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "BoardComms.h"
#include "Keyboard.h"
#include "LampDirect.h"

// Constants

// Run loop at 1000Hz
#define LoopRate 1

// Loop rate ticks        SSSmmm
#define CommsIdleTimeout     100
#define RetryTimePeriod      250
#define KeyDebouncePeriod    100

// Types


// Vars

Uint8   TxBuf[BCMsgSize];



// Functions


/*
  SendKey routine

  This routine sends a key press event over the RS485 link.
 */
void SendKey(Uint8 Key)
{
  TxBuf[BCPAddr] = (BCAKeypadController << 4) | BCAMP3Contoller;
  TxBuf[BCPType] = BCTKey;
  TxBuf[BCPParam1] = Key;
  TxBuf[BCPParam2] = 0;
  BCMessageSend(TxBuf, true);           // Send the reply
}



/*
  main routine

  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
  Uint8   RxBuf[BCMsgSize];
  Uint8   Destination;        // Address we will reply to
  Uint8   RetryTime;
  Uint8   RetryCount;
  bool    PowerOnTest;
  Uint16  Phase;
  Uint8   OldKey;
  Uint8   NewKey;


  // Set up the I/O lines

  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;

  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;

  SetBit(MosLEDPort,MosLED);
  // Init the periphials

  SetLamp(0);
  Timer_Init();                               // Set up timers

  sei();    // Enable global interrupts

  // Keyboard test

  // Final set up

  BCMessageInit(BCAKeypadSlave);         // Set up the UART
  BCMessageReceive(RxBuf);                    // Kick off receive

  PowerOnTest = true;
  Phase       = 0;
  RetryTime   = 0;
  RetryCount  = 0;
  OldKey      = 0;
  NewKey      = 0;

  // Enter the main loop

  for( ; ; ) {  // Run forever
    DelayMS(LoopRate);
    Phase++;

#ifdef DebugLamps
  if((Phase & 0b1000000000) | NewKey)
    SetBit(LEDLogoPort, LEDLogo);
  else
    ClearBit(LEDLogoPort, LEDLogo);
#endif

    // Look for new commands

    if (BCRXAvail) {     // We have a new message
      if ((RxBuf[BCPAddr] & 0b1111) == BCAKeypadSlave)  { // Check it is for us
        Destination    = RxBuf[BCPAddr] >> 4;  // Preset up assuming we will reply
        Destination    &= 0b1111;
        Destination    |= BCAKeypadSlave << 4;
        TxBuf[BCPAddr] = Destination;
        DelayMS(2);                         // Allow line turn around delay
        switch (RxBuf[BCPType]) {
          case BCTAck:                      // An ack of our key event
            RetryTime = 0;                  // They got it so cancel retry counter
            break;

          case BCTNAck: // Negative acknowledge
            break;

          case BCTLamps: // Set lamps
            PowerOnTest = false;
            SetLamps(RxBuf[BCPParam1]);
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
      BCMessageReceive(RxBuf);    // Kick off receive of next frame
    }

    // Look for change of key state

    if (!RetryTime) {           // Don't send a new key while waiting for the last one to be acknowledged
      NewKey = GetKey();
      if (NewKey != OldKey) {                                   // If change of key state
        //    if ((NewKey != OldKey) &&                                   // If change of key state
        //        (RetryTime < (RetryTimePeriod - KeyDebouncePeriod))) {  // and not too soon after the last one (debounce)
        SendKey(NewKey);
        RetryTime = RetryTimePeriod;
        RetryCount = 0;
        OldKey = NewKey;
      }
    }

    // Handle retry of key message

    if (RetryTime)
      if (!--RetryTime) {
        if (RetryCount++ < 3) {
          SendKey(OldKey);
          RetryTime = RetryTimePeriod;
        }
      }

    // Test LEDs

    if (PowerOnTest) {
     // SetLamp((~Phase >> 7) & 0b111);   // Rotate clockwise, the negate makes this opposite of what 031-517-202 does
    }
  }
}
