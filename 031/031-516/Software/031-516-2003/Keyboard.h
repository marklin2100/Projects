#ifndef touch_h
#define touch_h
/*****************************************************************************

    Keyboard module

    This module implements keyboard interface related software.

    The keyboard buttons are read directly.

    David Annett, david@lynxinnovation.com, 25 Jan 2013

    (c) 2013 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include "CommonTypes.h"
#include "Globals.h"

//========================== Constants =======================================


//========================== Functions =======================================

/*
  GetKey routine

  This routine will get the key number for the currently touched key,
  1 to n, or 0 if no key.
*/
Uint8 GetKey(void);

#endif
