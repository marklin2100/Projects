/*****************************************************************************

    Volume module

    This module implements a volume control via a LM4811 IC.

    David Annett, david@lynxinnovation.com, 4 Jan 2009

 ****************************************************************************/

// ------- Includes ----------

#include "Globals.h"
#include "CommonTypes.h"
#include "Volume.h"
#include "Timer.h"
#include "Wm8960.h"

// ------- Constants ---------


// ------- Types -------------


// ------- Public vars -------


// ------- Private vars ------

Uint8 CurrentVolume;


// ------- Private functions -
#if HWVer == 1
/*
  Volume_Clock routine

  This routine pulses the clock line.
 */
void Volume_Clock(void)
{
  SetBit(VolPort, VolClk);
  NOP;
  ClearBit(VolPort, VolClk);
  NOP;
}



// ------- Public functions -

/*
  Volume_Init routine

  This routine sets up the volume chip with maximum attenuation
 */
void Volume_Init(void)
{
  ClearBit(VolPort, VolClk);   // Preset clock state
  CurrentVolume = MaxVol;
  Volume_Set(0);               // Effectively mute
}



/*
  Volume_Set routine

  This routine set the volume level using the volume IC.
 */
void Volume_Set(Uint8 Volume)
{
  if (Volume)
    UnMute;
  else
    Mute;

  // Ramp volume if needed

  if (CurrentVolume < Volume) {         // Need to ramp up
    SetBit(VolPort, VolData);
    for (; CurrentVolume < Volume; CurrentVolume++)
      Volume_Clock();

  } else if (CurrentVolume > Volume) {  // Need to ramp down
    ClearBit(VolPort, VolData);
    for (; CurrentVolume > Volume; CurrentVolume--)
      Volume_Clock();
  }
}



/*
  Volume_Up routine

  This routine will step up the volume one step
 */
void Volume_Up(void)
{
  Uint8 Volume;

  Volume = CurrentVolume;
  if (Volume < MaxVol)
    Volume_Set(Volume + 1);
}



/*
  Volume_Down routine

  This routine will step down the volume one step
 */
void Volume_Down(void)
{
  Uint8 Volume;

  Volume = CurrentVolume;
  if (Volume > MinVol)
    Volume_Set(Volume - 1);
}


#elif HWVer == 2
void Volume_Set(Uint8 Volume,Uint8 Channel)
{

  if(Channel)
  {
    if(Volume)
      WM8960_SetVolume(Channel,Volume);
  }
  else
  {
    WM8960_SetVolume(1,Volume);
    WM8960_SetVolume(2,Volume);
  }
}

#endif
