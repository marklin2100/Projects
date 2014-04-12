/*****************************************************************************

    LampSerial module

    This module implements lamp functions connected via an HC595 serial
    interface.  The design is used with both lamps and LEDs.  The term lamps
    is used in both cases.

    David Annett 19 October 2011

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"
#include "Globals.h"


// ------- Constants ---------



// ------- Types -------------



// ------- Vars --------------


// ------- Macros ------------

#define LampDataLow           ((LampPort)  &= (~(1<<LampSR_Data)))
#define LampDataHigh          ((LampPort)  |= (1<<LampSR_Data))
#define LampClockLow          ((LampPort)  &= (~(1<<LampSH_CP)))
#define LampClockHigh         ((LampPort)  |= (1<<LampSH_CP))
#define LampLatchLow          ((LampPort)  &= (~(1<<LampST_CP)))
#define LampLatchHigh         ((LampPort)  |= (1<<LampST_CP))


// ------- Functions ---------

/*
  SetLamps routine

  This routine sets the Lamps as a bit pattern.
*/
void SetLamps(Uint8 Lamps)
{
  Uint8  TempInt;
  Uint8  Mask;


  // Update the lamps via the shift register

  LampLatchLow;
  LampClockLow;

  Mask = 0x80;
  for (TempInt = 0; TempInt < 8; TempInt++) {
    if (Lamps & Mask)
      LampDataLow;
    else
      LampDataHigh;
    NOP;
    LampClockHigh;
    Mask >>= 1;
    NOP;
    LampClockLow;
  }

  NOP;
  LampLatchHigh;
  NOP;
  LampLatchLow;
}


/*
  SetLamp routine

  This routine sets the Lamps.  0 = off, 1 to 8 for the lamp, all others off.
*/
void SetLamp(Uint8 Lamp)
{
  if ((Lamp > 0) && (Lamp <= 8))
    SetLamps(1 << (Lamp - 1));  // Set the bit pattern for the Lamps
  else
    SetLamps(0);
}
