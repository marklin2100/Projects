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



//========================== Constants =======================================


//========================== Types ===========================================


//========================== Public vars =====================================


//========================== Private vars ====================================

Uint8 CurrentKey = 0;


//========================== Functions =======================================

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
