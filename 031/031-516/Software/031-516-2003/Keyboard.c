/*****************************************************************************

    Keyboard module

    This module implements keyboard interface related software.

    The keyboard buttons are read directly.

    David Annett, david@lynxinnovation.com, 25 Jan 2013

    (c) 2013 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Keyboard.h"



//========================== Constants =======================================


//========================== Functions =======================================

/*
  GetKey routine

  This routine will get the key number for the currently touched key,
  1 to n, or 0 if no key.
*/
#if BoardType == KeyBoard
Uint8 GetKey(void)
{
  if (!GetBit(Switch1to6InPin, Switch1In))
    return 1;

  if (!GetBit(Switch1to6InPin, Switch2In))
    return 2;

  if (!GetBit(Switch1to6InPin, Switch3In))
    return 3;

  if (!GetBit(Switch1to6InPin, Switch4In))
    return 4;

  if (!GetBit(Switch1to6InPin, Switch5In))
    return 5;

  if (!GetBit(Switch1to6InPin, Switch6In))
    return 6;

  if (!GetBit(Switch7InPin, Switch7In))
    return 7;

  if (!GetBit(Switch8InPin, Switch8In))
    return 8;

  return 0;
}
#elif BoardType == TouchBoard
Uint8 GetKey(void)
{
  if (GetBit(Switch1to6InPin, Switch1In))
    return 1;

  if (GetBit(Switch1to6InPin, Switch2In))
    return 4;

  if (GetBit(Switch1to6InPin, Switch3In))
    return 3;

  if (GetBit(Switch1to6InPin, Switch4In))
    return 2;

  if (GetBit(Switch1to6InPin, Switch5In))
    return 6;

  if (GetBit(Switch1to6InPin, Switch6In))
    return 7;

  if (GetBit(Switch7InPin, Switch7In))
    return 0;

  return 0;
}
#endif
