/*
  031-517-204/5/6 program

  This program is for the Lynx Innovation 031-517-204/5/6 Beats headphone
  and speaker interface board.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517
  It communicates with the 031-517-202 system control board. It all controls the
  volume for LM4811 audio amps.  It also sets the brightness of a LED strip
  using PWM

  Created by David Annett 30 November 2012

  Copyright Lynx Innovation 2012
 */

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "BoardComms.h"
#include "IR.h"

#if HWVer == 2
#include "UART.h"
#endif
// Constants

// Run loop at 1kHz

#define LoopRate 1

// Types
//Uint8   RxBuf[BCMsgSize];


// Functions



void PortInit(void)
{
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;


  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;


  PORTB = PortPullUpB;
  PORTB = PortPullUpC;
  PORTB = PortPullUpD;

}



void MainInit(void)
{
  PortInit();
  Timer_Init();
}



/*
  main routine

  Program entry point
 */
int main(void)
{
  Uint8   TxBuf[BCMsgSize];
  Uint8   RxBuf[BCMsgSize];
  Uint8   Destination;        // Address we will reply to
  Uint8   TempInt;


  MainInit();
  ClearBit(TestPort, Test1);
  ClearBit(TestPort, Test2);
  ClearBit(TestPort, Test3);

  sei();    // Enable global interrupts

  BCMessageInit(BDCLCD);         // Set up the UART
  BCMessageReceive(RxBuf);        // Kick off receive
  for(TempInt = 0; TempInt <60 ;TempInt++)
  {
    DelayMS(1000);
  }

  for(TempInt = 0; TempInt <12 ;TempInt++)
  {
    DelayMS(800);
    NECBlast(MUTE);
  }
  NECBlast(ENTER);
  for(TempInt = 0; TempInt <30 ;TempInt++)
  {
    DelayMS(1000);
  }
  NECBlast(LEFT);
  DelayMS(1000);
  NECBlast(ENTER);

  // Enter the main loop

  SetBit(TestPort, Test1);
  for( ; ; ) {  // Run forever
    DelayMS(LoopRate);
    // Look for new commands

    if (BCRXAvail) {     // We have a new message
      SetBit(TestPort, Test2);
      if ((RxBuf[BCPAddr] & 0b1111) == BDCLCD)  { // Check it is for us
        Destination = RxBuf[BCPAddr] >> 4;  // Preset up assuming we will reply
        Destination &= 0b1111;
        Destination |= BDCLCD << 4;
        TxBuf[BCPAddr] = Destination;
        DelayMS(2);                         // Allow line turn around delay
        switch (RxBuf[BCPType]) {
          case BCTInquire:                  // Master request of slave ID
            TxBuf[BCPType] = BCTInquireAnswer;
            TxBuf[BCPParam1] = MediaPLayer;
            TxBuf[BCPParam2] = I2S16Bit;
            BCMessageSend(TxBuf, true);           // Send the reply
            break;

          case BCTVolume: // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf, true);           // Send the reply
            VolumeSet(RxBuf[BCPParam1]);
            break;

          case BCTPlayTrack: // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf, true);           // Send the reply
            PlayVOD(RxBuf[BCPParam1]);
            break;

          default:  // Unknown command
            TxBuf[BCPType] = BCTNAck;
            TxBuf[BCPParam1] = BCNUnkownType;
            TxBuf[BCPParam2] = RxBuf[BCPType];
            BCMessageSend(TxBuf, true);           // Send the reply
            break;
        }
      }
      BCMessageReceive(RxBuf);    // Kick off receive of next frame
      ClearBit(TestPort, Test2);
    }
  }
}
