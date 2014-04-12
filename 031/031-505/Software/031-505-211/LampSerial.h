#ifndef LampSerial_h
#define LampSerial_h
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

#define NoLamp 0


// ------- Types -------------



// ------- Vars --------------


// ------- Functions ---------

/*
  SetLamps routine

  This routine sets the Lamps as a bit pattern.
*/
void SetLamps(Uint8 Lamps);


/*
  SetLamp routine

  This routine sets the LEDs.  0 = off, 1 to 8 for the lamp, all others off.
*/
void SetLamp(Uint8 Lamp);

#endif
