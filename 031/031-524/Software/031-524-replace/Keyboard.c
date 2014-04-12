/*****************************************************************************

    Keyboard module

    This module implements keyboard interface related software.

    The keyboard is on another board with it's own processor.
    is via RS485

    David Annett, david@lynxinnovation.com, 25 Jan 2013

    (c) 2013 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Keyboard.h"
#include "UART.h"


//========================== Constants =======================================

#define KonamiLen 10

const Uint8 KonamiCode[KonamiLen] PROGMEM = {
    VolUp,
    VolUp,
    VolDown,
    VolDown,
    PrevTrack,
    NextTrack,
    PrevTrack,
    NextTrack,
    LeftSelect,
    RightSelect
};

//========================== Types ===========================================


//========================== Public vars =====================================


//========================== Private vars ====================================

Uint8 CurrentKey;
Uint8 KonamiStep;


//========================== Functions =======================================


/*
  InitKey routine

  This routine will prepare the key pad code for use.
*/
void InitKey(void)
{
  CurrentKey = 0;
  KonamiStep = 0;
}



/*
  GetKey routine

  This routine will get the key number for the currently touched key,
  1 to n, or 0 if no key.
*/
Uint8 GetKey(void)
{
  CheckForBoardMsg();
  return CurrentKey;
}



/*
  ProcessCommsKey routine

  This routine will process a key number sent via the comms system
*/
void ProcessCommsKey(Uint8 NewKey)
{
  CurrentKey = NewKey;
}



/*
  KonamiCheck routine

  This routine will check if the key completes the Konami code sequence
*/
bool KonamiCheck(Uint8 NewKey)
{
  if (NewKey == pgm_read_byte(&KonamiCode[KonamiStep])) {
    UART_TxStr("Konami step ");
    UART_TxNum(KonamiStep, 1);
    UART_TxNewLine();

    if (++KonamiStep >= KonamiLen) {
      KonamiStep = 0;
      return true;
    }
  } else {
    KonamiStep = 0;
  }
  return false;   // Default return
}
