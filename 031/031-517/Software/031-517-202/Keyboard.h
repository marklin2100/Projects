#ifndef touch_h
#define touch_h
/*****************************************************************************

    Keyboard module

    This module implements keyboard interface related software.

    The keyboard is on another board with it's own processor.
    is via RS485

    David Annett, david@lynxinnovation.com, 25 Jan 2013

    (c) 2013 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include "CommonTypes.h"
#include "Globals.h"

//========================== Constants =======================================


//========================== Functions =======================================

/*
  InitKey routine

  This routine will prepare the key pad code for use.
*/
void InitKey(void);


/*
  GetKey routine

  This routine will get the key number for the currently touched key,
  1 to n, or 0 if no key.
*/
Uint8 GetKey(void);


/*
  ProcessCommsKey routine

  This routine will process a key number sent via the comms system
*/
void ProcessCommsKey(Uint8 NewKey);


/*
  KonamiCheck routine

  This routine will check if the key completes the Konami code sequence
*/
bool KonamiCheck(Uint8 NewKey);

#endif
