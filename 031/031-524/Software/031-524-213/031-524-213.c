#include <util/delay.h>
#include "Timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "CommonTypes.h"

#define PowerEnableTime 3000

#define Detection     PB2
#define DetectionPin  PINB

#define Ctrol         PB3
#define CtrolPort     PORTB

#define LED           PB4
#define LEDPort       PORTB

#define NOP asm("nop");
//Uint8 Active;
//Uint8 PowerEnable;
volatile bool PowerOnFlag;

int main(void)
{
  DDRB = ((1 << Ctrol) | (1 << LED));

  PINB = 0x00;
  PORTB = 0x00;

  Timer_Init();

  sei();      // Enable global interrupts

  PowerOnFlag = false;

  while(1)
  {
    if((GetBit(DetectionPin,Detection)))
    {
      if(!PowerOnFlag)
      {
        DelayMS(1000);
        if((GetBit(DetectionPin,Detection)))//wait for 1 seconds
        {
          SetBit(CtrolPort,Ctrol);
          SetBit(LEDPort,LED);
          DelayMS(3000);
          ClearBit(CtrolPort,Ctrol);
          ClearBit(LEDPort,LED);
          PowerOnFlag = true;
        }

      }
    }

    else if(!(GetBit(DetectionPin,Detection)))
    {
      if(PowerOnFlag)
      {
        PowerOnFlag = false;
      }
    }
  }
}


