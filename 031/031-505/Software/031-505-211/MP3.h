#ifndef mp3_h
#define mp3_h
/*****************************************************************************

    MP3 module

    This module implements MP3 related software.

    The MP3 chip is a VS1003 or VS1053.

    See http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.RMP-MP3

    David Annett, david@lynxinnovation.com, 18 Jan 2012

    (c) 2012 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include "CommonTypes.h"
#include "Globals.h"
#include "sd.h"

//========================== Constants =======================================



//========================== Functions =======================================

/*
  MP3_Init routine

  This routine will prepare the MP3 player hardware for use.
*/
void MP3_Init(void);


/*
  MP3_Track routine

  This routine will start playback of the requested track.  The track number
  should be in the range 1 to 99.
*/
void MP3_Track(Uint8 Track);


/*
  MP3_Volume routine

  This routine will set the output volume of the playback.
*/
void MP3_Volume(Uint8 Volume);


/*
  MP3_Process routine

  This routine must be called regularly to keep the MP3 chip feed with data
  or else playback will stop.  It will return true if still playing.
 */
bool MP3_Process(void);

#endif
