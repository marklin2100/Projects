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


// ------- Constants ---------



// ------- Types -------------



// ------- Vars --------------

#ifdef DebugLamps
volatile bool Debug1;
volatile bool Debug2;
#endif


// ------- Macros ------------


// ------- Functions ---------

/*
  SetLamps routine

  This routine sets the Lamps as a bit pattern.
*/
void SetLamps(Uint8 NewLamps)
{
  if(NewLamps & 0b1)
    ClearBit(LED1to3Port, LED1);
  else
    SetBit(LED1to3Port, LED1);

  if(NewLamps & 0b10)
    ClearBit(LED1to3Port, LED2);
  else
    SetBit(LED1to3Port, LED2);

  if(NewLamps & 0b100)
    ClearBit(LED1to3Port, LED3);
  else
    SetBit(LED1to3Port, LED3);

  if(NewLamps & 0b1000)
    ClearBit(LED4to7Port, LED4);
  else
    SetBit(LED4to7Port, LED4);

#ifndef DebugLamps
  if(NewLamps & 0b10000)
    ClearBit(LEDLogoPort, LEDLogo);
  else
    SetBit(LEDLogoPort, LEDLogo);
#endif

  if(NewLamps & 0b10000)
    ClearBit(LED4to7Port, LED6);
  else
    SetBit(LED4to7Port, LED6);


  if(NewLamps & 0b1000000)
    SetBit(LED4to7Port, LED7);
  else
    ClearBit(LED4to7Port, LED7);
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
