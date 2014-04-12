/*****************************************************************************

    LampSerial module

    This module implements lamp functions connected via RS485 comms to another
    processor on a remote board.
    The design is used with both lamps and LEDs.  The term lamps is used in
    both cases.

    David Annett 25 Jan 2013

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"
#include "Globals.h"
#include "LampSerial.h"


// ------- Constants ---------



// ------- Types -------------



// ------- Vars --------------



// ------- Macros ------------


// ------- Functions ---------

/*
  SetLamps routine

  This routine sets the Lamps as a bit pattern.
*/
void SetLamps(Uint8 NewLamps)
{
  static Uint8 LastLamps;


  if (NewLamps != LastLamps) {
    ExchangeBoardMsg(BCAKeypadController, BCTLamps, NewLamps, 0, BCTAck);
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
    LastLamps = NewLamps;
  }
}


/*
  SetLamp routine

  This routine sets the Lamps.  0 = off, 1 to 8 for the lamp, all others off.
*/
void SetLamp(Uint8 Lamp)
{
  if ((Lamp > 0) && (Lamp <= 16))
    SetLamps(1 << (Lamp - 1));  // Set the bit pattern for the Lamps
  else
    SetLamps(0);
}
