#ifndef timer_h
#define timer_h
/*****************************************************************************

    Timer module

    This module implments timer related software

    David Annett, david@annett.co.nz, 2 August 2008

****************************************************************************/

#include "CommonTypes.h"
#include <avr/interrupt.h>

// ------- Constants ---------

/*
  IR blaster styles:

                Pin idle  Pin blast Notes
  BlastHigh     Low       36kHz     Use with blaster IR LED driven from transistor (inverts)
  BlastLow      High      36kHz     Use with blaster IR LED driven directly without transistor
  DirectHigh    Low       High      Use with direct cable replacing O/C IR detector via transistor
  DirectLow     High      Low       Use with direct cable replacing O/C IR detector without transistor
  OtherHigh     Low       High      Like DirectHigh but on different pin. Must define IRDrive and IRDrivePort
  OtherLow      High      Low       Like DirectHigh but on different pin. Must define IRDrive and IRDrivePort

  Page 82 of ATtiny2313 datasheet covers register usage.
  Page 105 of ATmega324 datasheet covers register usage.
*/
#define BlastHigh

#ifdef BlastHigh
#define Timer0Idle  ((1 << COM0B1) |                 (1 << WGM01))
#define Timer0Blast (                (1 << COM0B0) | (1 << WGM01))
#define IRInit      (TCCR0A = Timer0Idle)
#define IRIdle      (TCCR0A = Timer0Idle)
#define IRBlast     (TCCR0A = Timer0Blast)
#endif

#ifdef BlastLow
#define Timer0Idle  ((1 << COM0B1) | (1 << COM0B0) | (1 << WGM01))
#define Timer0Blast (                (1 << COM0B0) | (1 << WGM01))
#define IRInit      (TCCR0A = Timer0Idle)
#define IRIdle      (TCCR0A = Timer0Idle)
#define IRBlast     (TCCR0A = Timer0Blast)
#endif

#ifdef DirectHigh
#define Timer0Idle  ((1 << COM0B1) |                 (1 << WGM01))
#define Timer0Blast ((1 << COM0B1) | (1 << COM0B0) | (1 << WGM01))
#define IRInit      (TCCR0A = Timer0Idle)
#define IRIdle      (TCCR0A = Timer0Idle)
#define IRBlast     (TCCR0A = Timer0Blast)
#endif

#ifdef DirectLow
#define Timer0Idle  ((1 << COM0B1) | (1 << COM0B0) | (1 << WGM01))
#define Timer0Blast ((1 << COM0B1) |                 (1 << WGM01))
#define IRInit      (TCCR0A = Timer0Idle)
#define IRIdle      (TCCR0A = Timer0Idle)
#define IRBlast     (TCCR0A = Timer0Blast)
#endif

#ifdef OtherHigh
#define Timer0Idle  (1 << WGM01)
#define Timer0Blast (1 << WGM01)
#define IRInit      (TCCR0A = Timer0Idle)
#define IRIdle      (ClearBit(IRDrivePort,IRDrive))
#define IRBlast     (SetBit(IRDrivePort,IRDrive))
#endif

#ifdef OtherLow
#define Timer0Idle  (1 << WGM01)
#define Timer0Blast (1 << WGM01)
#define IRInit      (TCCR0A = Timer0Idle)
#define IRIdle      (SetBit(IRDrivePort,IRDrive))
#define IRBlast     (ClearBit(IRDrivePort,IRDrive))
#endif


//====================== Public vars ==========================================

extern volatile Uint16 RS485TXDrop;     // Timer for RS485 transmission drop
extern volatile Uint16 RS485RXTimeout;  // Timer for RS485 packet reset



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


/*
  Timer_Read routine

  This routine reads the current value of the 1mS timer.
*/
Uint16 Timer_Read(void);


/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
*/
void DelayMS(Uint16 Time);

#endif
