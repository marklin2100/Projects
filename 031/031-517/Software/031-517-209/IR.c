/*****************************************************************************

    IR module

    This module implements IR functions

    David Annett 25 Oct 2008

****************************************************************************/

// ------- Includes ----------

#include "Globals.h"
#include "IR.h"
#include "Timer.h"
#include "CommonTypes.h"
#include <avr/pgmspace.h>

// ------- Constants ---------


// ------- Types -------------


// ------- Vars --------------

volatile enum TNECState NECState;
volatile Uint32     IRCode;
volatile Uint8      IRBit;

// Vars
#if SystemBoard == 180
const Uint8 IRDigit[] PROGMEM = {
    0x1C, // 0
    0x15, // 1
    0x14, // 2
    0x16, // 3
    0x17, // 4
    0x19, // 5
    0x18, // 6
    0x1A, // 7
    0x1B, // 8
    0x1D  // 9
};
#elif SystemBoard == 200
const Uint8 IRDigit[] PROGMEM = {
    0x04, // 0
    0x1a, // 1
    0x1b, // 2
    0x02, // 3
    0x1c, // 4
    0x1d, // 5
    0x17, // 6
    0x40, // 7
    0x44, // 8
    0x48  // 9
};
#endif



// ------- Private functions -


// ------- Public functions --

/*
  NECBlast routine

  This routine will send out an NEC IR frame.
  The IR frame is 32 bits, 16 bits system code, 8 bits data and 8 bits data negated.
  The 8 bit data code should be supplied.
*/
void NECBlast(Uint8 Code)
{
  Uint8   TempByte;


//  IR codes, edianess requires byte order reversal when defined
//  e.g. System 807F with data 20 would normally be written as
//       0x807F20DF but here is 0xDF207F80

  IRCode   = (Uint32)(((SystemCode & 0x00FF) << 8) | ((SystemCode & 0xFF00) >> 8));
  IRCode   |= (Uint32)Code << 16;
  TempByte = ~Code;
  IRCode   |= (Uint32)TempByte << 24;
  IRBit    = 0;
  NECState = AGC;
  OCR1A = OCR1A + AGCDelay;
  IRBlast;
  while(NECState != Idle);
  DelayMS(42);
}

/*
  NECRepeat routine

  This routine will send out an NEC IR repeat frame.
*/
void NECRepeat(void)
{
  NECState = Repeat;
  OCR1A = OCR1A + AGCDelay;
  IRBlast;
  while(NECState != Idle);
  DelayMS(98);
}


void VolumeSet(Uint8 VolumeUp)
{
  if(VolumeUp)
    NECBlast(VOLUME_UP);
  else
    NECBlast(VOLUME_DOWN);
}

/*
  BlastDigit routine

  This routine will send an IR digit to the player
 */
void BlastDigit(Uint8 Digit)
{
  NECBlast(pgm_read_byte(&IRDigit[Digit]));
}


/*
  PlayVOD routine

  This routine will start the playback of the requested VOD track
 */
void PlayVOD(Uint8 Track)
{
  BlastDigit(Track / 10);       // Tell the player to change track
  BlastDigit(Track % 10);
}

