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


// ------- Types -------------



// ------- Vars --------------


// ------- Functions ---------

void SetLampsPercent(Uint8 Percent);

#endif
