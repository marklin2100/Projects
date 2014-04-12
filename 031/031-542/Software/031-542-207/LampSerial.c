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

// ------- Functions ---------

/*
  SetLamps routine

 Percent for 0-->100
*/
void SetLampsPercent(Uint8 Percent)
{
  Uint16 OCR_Value;
  OCR_Value = Percent * 10;
  OCR1A = OCR_Value;
}



