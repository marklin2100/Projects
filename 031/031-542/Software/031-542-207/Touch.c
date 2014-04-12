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
#include "UARTPolled.h"



//========================== Constants =======================================


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
  if (!(BCDInPin & (1<<BCD1)))
    Result += 0x01;
  if (!(BCDInPin & (1<<BCD2)))
    Result += 0x02;
  if (!(BCDInPin & (1<<BCD4)))
    Result += 0x04;
  if (!(BCDInPin & (1<<BCD8)))
    Result += 0x08;
  return Result;
}
