/*
  031-505-210 program

  This program is the code for the 031-505-210 board.
  The wiki page is http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-505
  It PWM sets the brightness of LEDs.

  Created by David Annett 14 September 2012

  Copyright Lynx Innovation 2012
*/

#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"


// Constants

// Mapping to match 0 to 9 switch where 0 = off, 1 = 11.1% ... 8 = 88.8% and 9 = 100%
#define PWMSteps  9
#define PWMLevel  7

// Types


// Vars





/*
  main routine

  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
  uint8 PWM;


  // Set up the I/O lines

  DDRB = PortDirB;
//  SetBit(VibSwPort,VibSw);    // Turn on vibration switch pull up

  // Init the periphials

  Timer0_Init();

  // Final set up

  sei();    // Enable global interrupts

  // Do power on test

  PWM = 0;

  // Enter the main loop

  for( ; ; ) {  // Run forever
    DelayMS(1);                  // Set basic PWM rate

    if (++PWM >= PWMSteps)        // Advance PWM phase
      PWM = 0;

    if (PWM < PWMLevel)           // Set on or off depending on phase and required brightness
      SetBit(LampPort,Lamp);       // Turn on LEDs
    else
      ClearBit(LampPort,Lamp);     // Turn off LEDs
  }
  return 0;
}
