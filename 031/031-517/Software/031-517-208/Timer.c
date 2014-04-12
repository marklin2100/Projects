/*****************************************************************************

    Timer module

    This module implments timer related software.
  Timer 0 implements the 36kHz IR carrier.
  Timer 1 implements the IR bit periods and delay routines.

    David Annett, david@annett.co.nz, 2 August 2008

 ******************************************************************************/

//====================== Includes =============================================

#include "Timer.h"
#include "Globals.h"
#include "BoardComms.h"
#include <avr/interrupt.h>


//====================== Constants ============================================

// Switch the Lamps at different times on the PWM cycle to even out load

#define LEDChannels   6
#define PhaseStep     (255/LEDChannels)

//====================== Types ================================================


//====================== Public vars ==========================================

volatile Uint16 RS485TXDrop;     // Timer for RS485 transmission drop
volatile Uint16 RS485RXTimeout;  // Timer for RS485 packet reset

Uint8 LED1;
Uint8 LED2;
Uint8 LED3;
Uint8 LED4;
Uint8 LED5;
Uint8 LED6;

//====================== Vars =================================================

volatile Uint16 CountDown;
volatile Uint16 CountUp;
volatile Uint8  Phase;



//====================== Functions ============================================

/*
  Timer_Init routine

  This routine initialise the timer module.
 */
void Timer_Init(void)
{
  // Timer 0, 36kHz pulses using CTC mode for multi channel PWM.  Tick is Xtal/8 = 400nS.

  TCCR0A = ((1 << COM0B1) | (1 << WGM01));  // Ensure not send IR burst and T0 is in CTC mode
  TCCR0B = (1 << CS01);                     // Prescaler divide by 8 = 2.5MHz/400nS tick
  OCR0A  = 96;                              // 26kHz from 2.5MHz clock
  OCR0B  = 39;                              // ?kHz from 2.5MHz clock
  TIMSK0 = (1 << OCIE0A);                   // 26kHz interrupt for use on LED PWM, 256 steps = 100Hz cycle

  // Timer 1, normal mode. Tick is Xtal/8 = 400nS period for maximum delay of 26mS

  TCCR1A = 0;
  TCCR1B = (1 << CS11);                                 // Prescaler divide by 8 = 2.5MHz/400nS tick
  TIMSK1 =  (1 << OCIE1A) | (1 << OCIE1B);              // Enable delay
  TIFR1 =   (1 << OCF1A) | (1 << OCF1B) | (1 << ICF1);  // Clear off interrupt flags
  OCR1B = OCR1B + 2500;                                 // Reload for next tick, 2500 x 400nS = 1mS
}



/*
  Timer 1 output compare B interrupt service

  This timer is used for a 1mS periodic interrupt.
 */
ISR(TIMER1_COMPB_vect)
{
  OCR1B = OCR1B + 2500;   // Reload for next tick, 2500 x 400nS = 1mS

  // Timer user for DelayMS

  if (CountDown)
    CountDown--;

  // Timer used for Timer_Clear and Timer_Read

  CountUp++;

  // Do count down for RS485 transmit stop

  if (RS485TXDrop)
    if (!--RS485TXDrop) {
      ClearBit (RS485DEPort, RS485DE);      // Turn off RS485 driver
      BCTXBusy = false;
    }

  // Do count down for RS485 receive timeout

  if (RS485RXTimeout)
    if (!--RS485RXTimeout) {
      BCRXPos = 0;                          // Dump any data and start again
    }
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
  Timer 0 output compare A interrupt service

  This timer is used for PWM timing.
 */
ISR(TIMER0_COMPA_vect)
{
  Uint8 LEDPhase;


  Phase++;

  // LED1

  if (LED1 && (LED1 >= Phase))
    SetBit(LEDPort, LED1Line);
  else
    ClearBit(LEDPort, LED1Line);


  // LED2

  LEDPhase = Phase + PhaseStep;
  if (LED2 && (LED2 >= LEDPhase))
    SetBit(LEDPort, LED2Line);
  else
    ClearBit(LEDPort, LED2Line);

  // LED3

  LEDPhase = Phase + PhaseStep * 2;
  if (LED3 && (LED3 >= LEDPhase))
    SetBit(LEDPort, LED3Line);
  else
    ClearBit(LEDPort, LED3Line);

  // LED4

  LEDPhase = Phase + PhaseStep * 3;
  if (LED4 && (LED4 >= LEDPhase))
    SetBit(LEDPort, LED4Line);
  else
    ClearBit(LEDPort, LED4Line);

  // LED5

  LEDPhase = Phase + PhaseStep * 4;
  if (LED5 && (LED5 >= LEDPhase))
    SetBit(LEDPort, LED5Line);
  else
    ClearBit(LEDPort, LED5Line);

  // LED6

  LEDPhase = Phase + PhaseStep * 5;
  if (LED6 && (LED6 >= LEDPhase))
    SetBit(LEDPort, LED6Line);
  else
    ClearBit(LEDPort, LED6Line);
}



/*
  Timer 1 output compare A interrupt service

  This timer is used for IR send bit timing.
 */
ISR(TIMER1_COMPA_vect)
{

}
