/*
  031-517-202 MP3 program

  This program is for the Lynx Innovation 031-517-202 MP3 controller board and
  MP3 player in the Beats 8' inline headphone display.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517
  It reads button presses from the keyboard controller and uses them to play MP3s.

  Created by David Annett 26 September 2012

  Copyright Lynx Innovation 2012
 */



#include "Globals.h"
#include "CommonTypes.h"

// Types

// Vars


// Forward declares

/*
  main routine

  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{


  // Set up the I/O lines

  DDRA = PortDirA;
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;

  PINA = PortPullUpA;
  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;

  //set the channel is the keyboard.
  //mp3 init finish ,after reset the slave board have volume;
  SetBit(SelAPort, Sel0A);
  SetBit(SelAPort, Sel1A);

  SetBit(SelBPort, Sel0B);
  SetBit(SelBPort, Sel1B);

  SetBit(SelCPort, Sel0C);
  SetBit(SelCPort, Sel1C);


  ClearBit(RS485DEPort,RS485DE);
  while(1);

  return 0;
}
