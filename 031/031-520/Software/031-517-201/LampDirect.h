#ifndef LampDirect_h
#define LampDirect_h
/*****************************************************************************

    LampSerial module

    This module implements lamp functions connected directly to the processor.
    The design is used with both lamps and LEDs.  The term lamps is used in
    both cases.

    David Annett 25 Jan 2013

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"
#include "Globals.h"


// ------- Constants ---------

#define NoLamp    0
#define MaxLamp   7

// ------- Macros ------------



// ------- Types -------------



// ------- Vars --------------

extern Uint8 LampsOff[MaxLamp];
extern Uint8 LampsOn[MaxLamp];


// ------- Functions ---------

/*
  InitLamps routine

  This routine prepares the lamps for use.
*/
void InitLamps(void);


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
