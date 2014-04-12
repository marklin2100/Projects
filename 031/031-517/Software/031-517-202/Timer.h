#ifndef timer_h
#define timer_h
/*****************************************************************************

    Timer module

    This module implments timer related software

    David Annett, david@annett.co.nz, 2 August 2008

****************************************************************************/

#include "CommonTypes.h"
#include <avr/interrupt.h>

//====================== Constants ============================================


//====================== Public vars ==========================================

extern volatile Uint16 RS485TXDrop;     // Timer for RS485 transmission drop
extern volatile Uint16 RS485RXTimeout;  // Timer for RS485 packet reset

#if PWMMode == SlowPWM
extern volatile Uint8 BeatsLogo;
#endif
#if PWMMode == FastPWM
#define BeatsLogo OCR0B
#endif
extern volatile bool  PulseUp;


//====================== Functions ============================================

/*
  Timer_Init routine

  This routine initialise the timer module.
*/
void Timer_Init(void);


/*
  Timer_Clear routine

  This routine clears the 1mS timer.
*/
void Timer_Clear(void);
void SlaveModeTimerClear(void);


/*
  Timer_Read routine

  This routine reads the current value of the 1mS timer.
*/
Uint16 Timer_Read(void);
Uint16 SlaveModeTimerRead(void);



/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
*/
void DelayMS(Uint16 Time);

/*
  PulseLogo routine

  This routine handles pulsing of the Beats logo
*/
void PulseLogo(void);

#endif
