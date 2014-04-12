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

//========================== Includes =========================================

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "BoardComms.h"
#include "Volume.h"
#include "Pcm9211.h"
#include "iicio.h"

#if HWVer == 2
#endif


//========================== Constants ========================================

// Run loop at 1kHz

#define LoopRate 1

// Loop rate ticks        SSSmmm
#define CommsIdleTimeout      10


//========================== Types ============================================


//========================== Vars =============================================




//========================== Functions =======================================




/*
  ReadProductID routine

  This routine read the product ID.
 */
Uint8 ReadProductID(void)
{
  Uint8 Result;


  Result = 0;

  if (!GetBit(IDInPin,ID3In))
    Result += 2;

  if (!GetBit(IDInPin,ID2In))
    Result++;

  return Result;
}


/*
  ReadPosition routine

  This routine read the board bay position.
 */
Uint8 ReadPosition(void)
{
  Uint8 Result;


  Result = 0;

  if (!GetBit(IDInPin,ID1In))
    Result += 2;

  if (!GetBit(IDInPin,ID0In))
    Result++;

  return Result;
}



/*
  PortInit routine

  This routine prepares the I/O ports for use.
*/
void PortInit(void)
{
  DDRA = PortDirA;
  PINA = PortPullUpA;
  PORTA = PortPullUpA;
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;


  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;


  PORTB = PortPullUpB;
  PORTB = PortPullUpC;
  PORTB = PortPullUpD;

  //Enable 74HC245 Input B Output A
  HC245Port &= (1 << HC245DIR);
  HC245Port &= (~(1 << HC245OE));
}



/*
  MainInit routine

  This routine performs the main initialisation.
*/
void MainInit(void)
{
  PortInit();
  Timer_Init();
  IICIO_Init();
  sei();    // Enable global interrupts
  Pcm9211_Init();
  Pcm9211_Init();//again
}



/*
  main routine

  Program entry point
*/
int main(void)
{
  Uint8   TxBuf[BCMsgSize];
  Uint8   RxBuf[BCMsgSize];
  Uint8   Address;            // Address of device in bay
  Uint8   Destination;        // Address we will reply to
  Uint8   Param;


  MainInit();


  BCMessageInit(BDCLCD);         // Set up the UART
  BCMessageReceive(RxBuf);        // Kick off receive

  // Enter the main loop

  for( ; ; ) {  // Run forever
    DelayMS(LoopRate);
    // Look for new commands

    if (BCRXAvail) {     // We have a new message
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
            //VolumeSet(RxBuf[BCPParam1]);
            break;

          case BCTPlayTrack: // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf, true);           // Send the reply
            //PlayVOD(RxBuf[BCPParam1]);
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
    }
  }
}
