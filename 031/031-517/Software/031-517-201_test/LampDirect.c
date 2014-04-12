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
#include "LampDirect.h"
#include "Timer.h"


// ------- Constants ---------



// ------- Types -------------



// ------- Vars --------------

Uint8 LampsOff[MaxLamp];
Uint8 LampsOn[MaxLamp] = {7,7,7,7,7,7,7};


// ------- Macros ------------


// ------- Functions ---------


/*
  InitLamps routine

  This routine prepares the lamps for use.
*/
void InitLamps(void)
{
  Uint8 Lamp;

  for(Lamp = 0; Lamp < MaxLamp; Lamp++) {
    LampsOff[Lamp] = 0;
    LampsOn[Lamp] = 255;
  }
}



/*
  SetLamps routine

  This routine sets the Lamps as a bit pattern.
*/
void SetLamps(Uint8 NewLamps)
{
  if(NewLamps & 0b1)
    LED1 = LampsOn[0];
  else
    LED1 = LampsOff[0];

  if(NewLamps & 0b10)
    LED2 = LampsOn[1];
  else
    LED2 = LampsOff[1];

  if(NewLamps & 0b100)
    LED3 = LampsOn[2];
  else
    LED3 = LampsOff[2];

  if(NewLamps & 0b1000)
    LED4 = LampsOn[3];
  else
    LED4 = LampsOff[3];

  if(NewLamps & 0b10000)
    LED5 = LampsOn[4];
  else
    LED5 = LampsOff[4];

  if(NewLamps & 0b100000)
    LED6 = LampsOn[5];
  else
    LED6 = LampsOff[5];

  if(NewLamps & 0b1000000)
    LED7 = LampsOn[6];
  else
    LED7 = LampsOff[6];
}



/*
  SetLamp routine

  This routine sets the Lamps.  0 = off, 1 to 8 for the lamp, all others off.
*/
void SetLamp(Uint8 Lamp)
{
  if ((Lamp > 0) && (Lamp <= 7))
    SetLamps(1 << (Lamp - 1));  // Set the bit pattern for the Lamps
  else
    SetLamps(0);
}
