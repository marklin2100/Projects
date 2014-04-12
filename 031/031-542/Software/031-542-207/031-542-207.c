/*
  SL-STC_monsterheadphone program

  This program is for the Lynx Innovation 006-506 Monster Rave headphone
  display controller.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.006-506
  It reads button presses from the touch controller and uses them to control a
  10" LCD media player via direct NEC IR.  It all controls the volume for 6 or 7
  LM4811 audio amps.

  NB:  No actual lamps but code left in case panel is changed in future.

  Created by David Annett 5 July 2012

  Copyright Lynx Innovation 2012
 */

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "UARTPolled.h"
#include "Touch.h"
#include "LampSerial.h"

// Constants




// Run loop at 100Hz

#define LoopRate 10

// Loop rate ticks    SSSmm
//#define RGBHold         500

// 5mS ticks for 20 secs

// Keys
#define VolDownKey        1
#define VolUpKey          2
#define NextKey           3
#define PrevKey           4

#define AllLamps          0b1111





/*
  main routine

  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
  Uint8   TempInt;
  Uint8   Key;


  // Set up the I/O lines

  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;

  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;

  PORTB = 0X00;
  // Init the periphials

  UARTInit();                                 // Set up the UART
  UARTTxByte(0x0A);
  UARTTxByte(0x0D);
  UARTTxByte('S');
  Timer_Init();                               // Set up timers
  sei();    // Enable global interrupts

  // Test the LEDs


  UARTTxByte('R');
  UARTTxByte(0x0A);
  UARTTxByte(0x0D);


  /*for(TempInt = 0; TempInt <= 100; TempInt++)
  {
    SetLampsPercent(TempInt);
    DelayMS(1000);
  }*/
  // Enter the main loop

  for( ; ; ) {  // Run forever
    DelayMS(LoopRate);
    Key = GetKey();
    if(Key < 10)
    {
      SetLampsPercent((Key + 1) * 10);
    }

    // Look for exit of demo mode

  }
}
