#ifndef touch_h
#define touch_h
/*****************************************************************************

    Touch module

    This module implements touch controller related software.

    The touch controller is an ADPT016 with a binary interface.

    See http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.AT42QT1110

    David Annett, david@lynxinnovation.com, 14 May 2011

    (c) 2011 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include "CommonTypes.h"
#include "Globals.h"

//========================== Constants =======================================



//========================== Functions =======================================

/*
  GetKey routine

  This routine will the key number for the currently touched key, 1 to n, or 0
  if no key.
*/
Uint8 GetKey(void);

#endif
