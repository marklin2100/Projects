/*****************************************************************************

    Timer module

    This module implments timer related software.
  Timer 0 implements the 36kHz IR carrier.
  Timer 1 implements the IR bit periods and delay routines.

    David Annett, david@annett.co.nz, 2 August 2008

 ******************************************************************************/

//====================== Includes =============================================

#include <avr/interrupt.h>
#include "Timer.h"
#include "Globals.h"
#include "BoardComms.h"
#include "LampDirect.h"


//====================== Constants ============================================

#define LEDChannels   8
#define PhaseStep     (255/LEDChannels)

//====================== Types ================================================


//====================== Public vars ==========================================

volatile Uint16 RS485TXDrop;     // Timer for RS485 transmission drop
volatile Uint16 RS485RXTimeout;  // Timer for RS485 packet reset

volatile Uint8 LED1;
volatile Uint8 LED2;
volatile Uint8 LED3;
volatile Uint8 LED4;
volatile Uint8 LED5;
volatile Uint8 LED6;
volatile Uint8 LED7;
volatile Uint8 LED8;

//====================== Vars =================================================

volatile Uint16 CountDown;
volatile Uint16 CountUp;
volatile Uint8  T0Phase;


//====================== Functions ============================================

/*
  Timer_Init routine

  This routine initialise the timer module.
 */
void Timer_Init(void)
{
  // Timer 0, 36kHz pulses using CTC mode and pin toggle while blasting.  Tick is Xtal/8 = 400nS.

  TCCR0A = 1 << WGM01;
  TCCR0B = (1 << CS01);           // Prescaler divide by 8 = 3MHz/333nS tick
  OCR0A  = 39;                  // 36kHz from 3MHz clock
  OCR0B  = 39;                  // 36kHz from 3MHz clock
  TIMSK0 = (1 << OCIE0A);       // 36kHz interrupt for use on LED PWM

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


  T0Phase++;

  // LED1

  if (LED1 && (LED1 >= T0Phase))
    ClearBit(LED1Port, LED1Line);   // Turn LED on
  else
    SetBit(LED1Port, LED1Line);     // Turn LED off

  // LED2

  LEDPhase = T0Phase + PhaseStep;
  if (LED2 && (LED2 >= LEDPhase))
    ClearBit(LED2Port, LED2Line);
  else
    SetBit(LED2Port, LED2Line);

  // LED3

  LEDPhase = T0Phase + PhaseStep * 2;
  if (LED3 && (LED3 >= LEDPhase))
    ClearBit(LED3Port, LED3Line);
  else
    SetBit(LED3Port, LED3Line);

  // LED4

  LEDPhase = T0Phase + PhaseStep * 3;
  if (LED4 && (LED4 >= LEDPhase))
    ClearBit(LED4Port, LED4Line);
  else
    SetBit(LED4Port, LED4Line);

  // LED5

  LEDPhase = T0Phase + PhaseStep * 4;
  if (LED5 && (LED5 >= LEDPhase))
    SetBit(LED5Port, LED5Line);
  else
    ClearBit(LED5Port, LED5Line);

  // LED6

  LEDPhase = T0Phase + PhaseStep * 5;
  if (LED6 && (LED6 >= LEDPhase))
    SetBit(LED6Port, LED6Line);
  else
    ClearBit(LED6Port, LED6Line);

  // LED7

  LEDPhase = T0Phase + PhaseStep * 6;
  if (LED7 && (LED7 >= LEDPhase))
    SetBit(LED7Port, LED7Line);
  else
    ClearBit(LED7Port, LED7Line);

  // LED8
  LEDPhase = T0Phase + PhaseStep * 7;
  if (LED8 && (LED8 >= LEDPhase))
    SetBit(LED8Port, LED8Line);
  else
    ClearBit(LED8Port, LED8Line);
}


/*
  Timer 1 output compare A interrupt service

  This timer is used for IR send bit timing.
 */
ISR(TIMER1_COMPA_vect)
{

}
