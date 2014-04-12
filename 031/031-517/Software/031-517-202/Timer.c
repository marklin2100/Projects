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
#include "LampSerial.h"
#include "BoardComms.h"

#define TimeInitValue (F_CPU/(8*1000))
//====================== Constants ============================================


//====================== Types ================================================


//====================== Public vars ==========================================

volatile Uint16 RS485TXDrop;     // Timer for RS485 transmission drop
volatile Uint16 RS485RXTimeout;  // Timer for RS485 packet reset

#if PWMMode == SlowPWM
volatile Uint8 BeatsLogo;
#endif
volatile bool  PulseUp;


//====================== Vars =================================================

volatile Uint16 CountDown;
volatile Uint16 CountUp;
volatile Uint8  PulseScaler;
volatile Uint16 CountSlaveMode;
#if PWMMode == SlowPWM
volatile Uint8 Phase;
#endif

//====================== Functions ============================================

/*
  PulseLogo routine

  This routine handles pulsing of the Beats logo
 */
void PulseLogo(void)
{
#if PWMMode == FastPWM
  if (PulseUp) {                    // Getting brighter
    BeatsLogo++;
    if (BeatsLogo >= PulseMax) {    // Increment and see if time to change direction
      PulseUp = false;
    }
  } else {                          // Getting dimmer
    BeatsLogo--;
    if (BeatsLogo <= PulseMin) {    // Decrement and see if time to change direction
      PulseUp = true;
    }
  }
#endif

  #if PWMMode == SlowPWM
  if (PulseUp) {                    // Getting brighter
    if ((255 - PulseStep) < BeatsLogo)  // Prevent wrap around
      BeatsLogo = 255;
    else
      BeatsLogo += PulseStep;
    if (BeatsLogo >= PulseMax) {    // Increment and see if time to change direction
      PulseUp = false;
    }
  } else {                          // Getting dimmer
    if (BeatsLogo < PulseStep)      // Prevent wrap around
      BeatsLogo = 0;
    else
      BeatsLogo -= PulseStep;
    if (BeatsLogo <= PulseMin) {    // Decrement and see if time to change direction
      PulseUp = true;
    }
  }
#endif
}



/*
  Timer_Init routine

  This routine initialise the timer module.
 */
void Timer_Init(void)
{
  // Timer 0, 36kHz pulses using CTC mode and pin toggle while blasting.  Tick is Xtal/8 = 400nS.

#if PWMMode == SlowPWM
  Phase   = 0;
  TCCR0A = (1 << COM0B1) | (1 << WGM01);  // Ensure not send IR burst and T0 is in CTC mode
  TCCR0B = (1 << CS01);                   // Prescaler divide by 8 = 3MHz/333nS tick
  OCR0A  = 39;                            // 36kHz from 3MHz clock
  OCR0B  = 39;                            // 36kHz from 3MHz clock
  TIMSK0 = (1 << OCIE0A);                 // 36kHz interrupt for use on software LED PWM
#endif

#if PWMMode == FastPWM
  PulseScaler = 1;
  TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); // OC0B (PB4) in fast PWM mode
  TCCR0B = (1 << CS00);                                 // Prescaler no divide = 24MHz tick / 256 for 94kHz hardware PWM
  OCR0A  = 96;                                          // 36kHz from 3MHz clock
  OCR0B  = 0;                                           // Initial PWM output level
  TIMSK0 = (1 << OCIE0A);                               // 36kHz interrupt for use on LED PWM
#endif

  // Timer 1, normal mode. Tick is Xtal/8 = 400nS period for maximum delay of 26mS

  TCCR1A = 0;
  TCCR1B = (1 << CS11);                                 // Prescaler divide by 8 = 2.5MHz/400nS tick
  TIMSK1 =  (1 << OCIE1A) | (1 << OCIE1B);              // Enable delay
  TIFR1 =   (1 << OCF1A) | (1 << OCF1B) | (1 << ICF1);  // Clear off interrupt flags

  CountSlaveMode = 0;

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

void SlaveModeTimerClear(void)
{
  CountSlaveMode = 0;
}

/*
  Timer_Read routine

  This routine reads the current value of the 1mS timer.
 */
Uint16 Timer_Read(void)
{
  return CountUp;
}

Uint16 SlaveModeTimerRead(void)
{
  return CountSlaveMode;
}

//====================== Interrupt service routines============================

/*
  Timer 1 output compare B interrupt service

  This timer is used for a 1mS periodic interrupt.
 */
ISR(TIMER1_COMPB_vect)
{
  OCR1B = OCR1B + TimeInitValue;   // Reload for next tick, 2500 x 400nS = 1mS

  // Timer user for DelayMS

  if (CountDown)
    CountDown--;

  // Timer used for Timer_Clear and Timer_Read

  CountUp++;
  CountSlaveMode++;
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
  Timer 0 output compare A interrupt service

  This timer is used for software PWM timing.
 */
ISR(TIMER0_COMPA_vect)
{
#if PWMMode == SlowPWM
  Phase++;

  if (BeatsLogo > Phase)
    SetBit(LogoLEDPort, LogoLED);
  else
    ClearBit(LogoLEDPort, LogoLED);
#endif
}


/*
  Timer 1 output compare A interrupt service

  This timer is used for IR send bit timing.
 */
ISR(TIMER1_COMPA_vect)
{

}
