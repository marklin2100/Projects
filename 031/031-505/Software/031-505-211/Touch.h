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

// Key mapping.  Maps physically routed and chip reported to number returned
//      T-INn +1   Kn
#define TouchMap1  7
#define TouchMap2  8
#define TouchMap3  6
#define TouchMap4  5
#define TouchMap5  4
#define TouchMap6  3
#define TouchMap7  2
#define TouchMap8  1
#define TouchMap9  0
#define TouchMap10 0
#define TouchMap11 0
#define TouchMap12 0
#define TouchMap13 0
#define TouchMap14 0
#define TouchMap15 0
#define TouchMap16 0


//========================== Functions =======================================

/*
  GetKey routine

  This routine will the key number for the currently touched key, 1 to n, or 0
  if no key.
*/
Uint8 GetKey(void);

#endif
