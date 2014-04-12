#ifndef IR_h
#define IR_h
/*****************************************************************************

    IR module

    This module implements IR functions

    David Annett 25 Oct 2008

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"


// ------- Constants ---------

#define SystemCode  0x00FF

// IR delays, in 400nS time periods

#define NECAGCDelay        22500
#define NECAGCGapDelay     11250
#define NECDataBurstDelay  1400
#define NECDataZeroDelay   1400
#define NECDataOneDelay    4200
#define NECRepeatGapDelay  5625
#define ToshibaShortDelay  1055
#define ToshibaLongDelay   3165


// ------- Types -------------

enum TIRState {Idle, NECAGC, NECAGCGap, NECBit, NECBitGap, NECTail, NECRepeatBurst, NECRepeatGap, ToshibaBit, ToshibaBitGap};


// ------- Vars --------------

extern volatile enum TIRState  IRState;
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


/*
 ToshibaBlast routine

 This routine blasts the 12 bits of Toshiba format IR data to Monster Beatbox
 If should be bits: C1 C2 C3 H S1 S2 K1 K2 K3 K4 K5 K6
 e.g. Power button = 0b000010100000
*/
void ToshibaBlast(Uint16 Code);

#endif
