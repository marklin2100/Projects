#ifndef IR_h
#define IR_h
/*****************************************************************************

    IR module

    This module implements IR functions

    David Annett 25 Oct 2008

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"

#define SystemBoard   200
// ------- Constants ---------

#if SystemBoard == 180
#define SystemCode  0x08E6
#elif SystemBoard == 200
#define SystemCode  0x41BE
#define VOLUME_UP   0X5E
#define VOLUME_DOWN 0X0E
#endif
// IR delays, in 400nS time periods

#define AGCDelay        22500
#define AGCGapDelay     11250
#define DataBurstDelay  1400
#define DataZeroDelay   1400
#define DataOneDelay    4200
#define RepeatGapDelay  5625


// ------- Types -------------

enum TNECState {Idle, AGC, AGCGap, Bit, BitGap, Tail, Repeat, RepeatGap};


// ------- Vars --------------

extern volatile enum TNECState  NECState;
extern volatile Uint32    IRCode;
extern volatile Uint8     IRBit;


// ------- Functions ---------

/*
  NECBlast routine

  This routine will send out an NEC IR frame.
  The IR frame is 32 bits, 16 bits system code, 8 bits data and 8 bits data negated.
  The 8 bit data code should be supplied.
  It will wait for the frame to complete.
*/
void NECBlast(Uint8 Code);


/*
  NECRepeat routine

  This routine will send out an NEC IR repeat frame.
*/
void NECRepeat(void);

void VolumeSet(Uint8 VolumeUp);
void BlastDigit(Uint8 Digit);
void PlayVOD(Uint8 Track);
#endif
