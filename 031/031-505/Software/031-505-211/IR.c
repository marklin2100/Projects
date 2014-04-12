/*****************************************************************************

    IR module

    This module implements IR functions

    David Annett 25 Oct 2008

****************************************************************************/

// ------- Includes ----------

#include "Globals.h"
#include "IR.h"
#include "Timer.h"


// ------- Constants ---------


// ------- Types -------------


// ------- Vars --------------

volatile enum TIRState IRState;
volatile Uint32     IRCode;
volatile Uint8      IRBit;


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
  IRState = NECAGC;
  OCR1A = OCR1A + NECAGCDelay;
  IRBlast;
  while(IRState != Idle);
  DelayMS(42);
}

/*
  NECRepeat routine

  This routine will send out an NEC IR repeat frame.
*/
void NECRepeat(void)
{
  IRState = NECRepeatBurst;
  OCR1A = OCR1A + NECAGCDelay;
  IRBlast;
  while(IRState != Idle);
  DelayMS(98);
}



/*
 ToshibaBlast routine

  This routine blasts the 12 bits of Toshiba format IR data to Monster Beatbox
 If should be bits: C1 C2 C3 H S1 S2 K1 K2 K3 K4 K5 K6
 e.g. Power button = 0b000010100000
*/
void ToshibaBlast(Uint16 Code)
{
  IRCode  = Code;
  IRBit   = 0;
  IRState = ToshibaBitGap;            // Set a gap so interrupt state machine starts with bit
  OCR1A = OCR1A + ToshibaShortDelay;
  IRIdle;
  while(IRState != Idle);
  DelayMS(35);
}
