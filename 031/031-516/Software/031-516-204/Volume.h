#ifndef Volume_h
#define Volume_h
/*****************************************************************************

    Volume module

    This module implments a volume control via a LM4811 IC.

    David Annett, david@lynxinnovation.com, 4 Jan 2009

****************************************************************************/

// ------- Includes ----------

#include "Globals.h"
#include "CommonTypes.h"


// ------- Constants ---------

#define MinVol    0
#define MaxVol    15
#define VolStep   1



// ------- Types -------------


// ------- Public vars -------

extern Uint8 CurrentVolume;


// ------- Public functions -

/*
  Volume_Init routine

  This routine sets up the LM4811 with maximum attenuation
*/
void Volume_Init(void);


/*
  Volume_Set routine

  This routine sets the volume level using the LM4811 IC.
*/
void Volume_Set(Uint8 Volume,Uint8 Channel);



/*
  Volume_Up routine

  This routine will step up the volume one step.
*/
void Volume_Up(void);



/*
  Volume_Down routine

  This routine will step down the volume one step.
*/
void Volume_Down(void);

#endif
