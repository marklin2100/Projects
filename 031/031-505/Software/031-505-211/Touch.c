/*****************************************************************************

    Touch module

    This module implements touch controller related software.

    The touch controller is an ADPT016 with a binary interface.

    See http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.AT42QT1110

    David Annett, david@lynxinnovation.com, 14 May 2011

    (c) 2011 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Touch.h"
#include "Timer.h"



//========================== Constants =======================================

const Uint8 TouchMap[] PROGMEM = {0,
  TouchMap1,  TouchMap2,  TouchMap3,  TouchMap4,
  TouchMap5,  TouchMap6,  TouchMap7,  TouchMap8,
  TouchMap9,  TouchMap10, TouchMap11, TouchMap12,
  TouchMap13, TouchMap14, TouchMap15, TouchMap16};



//========================== Functions =======================================

/*
  GetKey routine

  This routine will the key number for the currently touched key, 1 to 8, or 0
  if no key.
*/
Uint8 GetKey(void)
{
  Uint8 Result;


  Result = 0;
  if (TouchInPin & (1<<Touch0In))
    Result += 0x01;
  if (TouchInPin & (1<<Touch1In))
    Result += 0x02;
  if (TouchInPin & (1<<Touch2In))
    Result += 0x04;
  if (TouchInPin & (1<<Touch3In))
    Result += 0x08;

  return pgm_read_byte(&TouchMap[Result]);
}
