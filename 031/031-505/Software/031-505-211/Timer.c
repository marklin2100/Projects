/*****************************************************************************

    Timer module

    This module implments timer related software.
  Timer 0 implements the 36kHz IR carrier.
  Timer 1 implements the IR bit periods and delay routines.

    David Annett, david@annett.co.nz, 2 August 2008

******************************************************************************/

//====================== Includes =============================================

#include "Timer.h"
#include "IR.h"
#include "Globals.h"
#include <avr/interrupt.h>

//====================== Constants ============================================


//====================== Types ================================================


//====================== Vars =================================================

volatile Uint16 CountDown;
volatile Uint16 CountUp;


//====================== Functions ============================================

/*
  Timer_Init routine

  This routine initialise the timer module.
*/
void Timer_Init(void)
{
// Timer 0, 36kHz pulses using CTC mode and pin toggle while blasting.  Tick is Xtal/8 = 400nS.

  IRInit;                       // Ensure not send IR burst and T0 is in CTC mode
  TCCR0B = (1 << CS01);           // Prescaler divide by 8 = 3MHz/333nS tick
  OCR0A  = 39;                  // 36kHz from 3MHz clock
  OCR0B  = 39;                  // 36kHz from 3MHz clock


// Timer 1, normal mode. Tick is Xtal/8 = 400nS period for maximum delay of 26mS

  TCCR1A = 0;
  TCCR1B = (1 << CS11);                    // Prescaler divide by 8 = 2.5MHz/400nS tick
  TIMSK1 =  (1 << OCIE1A) | (1 << OCIE1B);              // Enable delay
  TIFR1 =   (1 << OCF1A) | (1 << OCF1B) | (1 << ICF1);  // Clear off interrupt flags
}



/*
  Timer 1 output compare B interrupt service

  This timer is used for a 1mS periodic interrupt.
*/
ISR(TIMER1_COMPB_vect)
{
  OCR1B = OCR1B + 3000;   // Reload for next tick, 2500 x 400nS = 1mS

  if (CountDown)
    CountDown--;

  CountUp++;
}



/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
*/
void DelayMS(Uint16 Time)
{
  CountDown = Time;   // CountDown is decremented in ISR every millisecond
  while(CountDown);
}



/*
  Timer_Clear routine

  This routine clears the 1mS timer.
*/
void Timer_Clear(void)
{
  CountUp = 0;
}



/*
  Timer_Read routine

  This routine reads the current value of the 1mS timer.
*/
Uint16 Timer_Read(void)
{
  return CountUp;
}



/*
  Timer 1 output compare A interrupt service

  This timer is used for IR send bit timing.
*/
ISR(TIMER1_COMPA_vect)
{
// IR transmit mode, change if IR led pulsed and set up period time

  switch (IRState) {
    case Idle:
      OCR1A = OCR1A + NECDataBurstDelay;
      IRIdle;
    break;

    case NECAGC:
      IRState = NECAGCGap;
      OCR1A = OCR1A + NECAGCGapDelay;
      IRIdle;
      break;

    case NECAGCGap:
      IRState = NECBit;
      OCR1A = OCR1A + NECDataBurstDelay;
      IRBlast;
      break;

    case NECBit:
      IRState = NECBitGap;
      if (IRCode & 0x0000001)
        OCR1A = OCR1A + NECDataOneDelay;
      else
        OCR1A = OCR1A + NECDataZeroDelay;
      IRCode >>= 1;
      IRIdle;
      break;

    case NECBitGap:
      if (++IRBit >= 32)
        IRState = NECTail;
      else
        IRState = NECBit;
      OCR1A = OCR1A + NECDataBurstDelay;
      IRBlast;
      break;

    case NECTail:
      IRState = Idle;
      IRIdle;
      break;

    case NECRepeatBurst:
      IRState = NECRepeatGap;
      OCR1A = OCR1A + NECRepeatGapDelay;
      IRIdle;
      break;

    case NECRepeatGap:
      IRState = NECTail;
      OCR1A = OCR1A + NECDataBurstDelay;
      IRBlast;
      break;

    case ToshibaBit:
      IRState = ToshibaBitGap;
      if (IRCode & 0b100000000000)
        OCR1A = OCR1A + ToshibaShortDelay;
      else
        OCR1A = OCR1A + ToshibaLongDelay;
      IRCode <<= 1;
      IRIdle;
      break;

    case ToshibaBitGap:
      IRState = ToshibaBit;
      if (++IRBit > 12) {
        IRState = Idle;
        OCR1A = OCR1A + ToshibaShortDelay;
      } else {
        if (IRCode & 0b100000000000)
          OCR1A = OCR1A + ToshibaLongDelay;
        else
          OCR1A = OCR1A + ToshibaShortDelay;
        IRBlast;
      }
      break;
  }
}
