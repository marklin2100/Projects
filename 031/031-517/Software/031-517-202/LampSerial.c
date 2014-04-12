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

extern Uint8 BayProduct[];
extern enum TBay       Bay;
extern Uint16          IdleTime;

// ------- Constants ---------



// ------- Types -------------



// ------- Vars --------------



// ------- Macros ------------


// ------- Functions ---------

/*
  SetLamps routine

  This routine sets the Lamps as a bit pattern.
*/
bool SetLamps(Uint8 NewLamps)
{
  static Uint8 LastLamps = 255;
  bool status;

  if(SlaveMode)
    return false;

  status = true;
  if (NewLamps != LastLamps) {
    status = ExchangeBoardMsg(BCAKeypadController, BCTLamps, NewLamps, 0, BCTAck);
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
    LastLamps = NewLamps;
  }
  return status;
}

/*
bool SetBayBrightness(void){
  static enum TBay LastBay = LeftBay;
  Uint8 Brightness;
  bool status;
  if(SlaveMode)
    return false;

  if(IdleTime == 0){
    if(Bay != LastBay){
      if(BayProduct[LeftBay] < UnknownProduct){
        Brightness = Settings.ProdDef[BayProduct[LeftBay]].IdleBrightness;
        ExchangeBoardMsg(BCAOutput + LeftBay, BCTBrightness, 1, Brightness, BCTAck);
        BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
      }

      if(BayProduct[CenterBay] < UnknownProduct){
        Brightness = Settings.ProdDef[BayProduct[CenterBay]].IdleBrightness;
        ExchangeBoardMsg(BCAOutput + CenterBay, BCTBrightness, 1, Brightness, BCTAck);
        BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
      }

      if(BayProduct[RightBay] < UnknownProduct){
        Brightness = Settings.ProdDef[BayProduct[RightBay]].IdleBrightness;
        ExchangeBoardMsg(BCAOutput + RightBay, BCTBrightness, 1, Brightness, BCTAck);
        BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
      }
    }
  }
  else if(Bay != LastBay){
    if(BayProduct[LeftBay] < UnknownProduct){
        if(Bay == LeftBay){
          Brightness = Settings.ProdDef[BayProduct[LeftBay]].SelectedBrightness;
        }
        else if(Bay == NoBay){
          Brightness = Settings.ProdDef[BayProduct[LeftBay]].IdleBrightness;
        }
        else{
          Brightness = Settings.ProdDef[BayProduct[LeftBay]].UnselectedBrightness;
        }
        ExchangeBoardMsg(BCAOutput + LeftBay, BCTBrightness, 1, Brightness, BCTAck);
        BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
      }

      if(BayProduct[CenterBay] < UnknownProduct){
        if(Bay == CenterBay){
          Brightness = Settings.ProdDef[BayProduct[CenterBay]].SelectedBrightness;
        }
        else if(Bay == NoBay){
          Brightness = Settings.ProdDef[BayProduct[CenterBay]].IdleBrightness;
        }
        else{
          Brightness = Settings.ProdDef[BayProduct[CenterBay]].UnselectedBrightness;
        }
        ExchangeBoardMsg(BCAOutput + CenterBay, BCTBrightness, 1, Brightness, BCTAck);
        BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
      }

      if(BayProduct[RightBay] < UnknownProduct){
        if(Bay == RightBay){
          Brightness = Settings.ProdDef[BayProduct[RightBay]].SelectedBrightness;
        }
        else if(Bay == NoBay){
          Brightness = Settings.ProdDef[BayProduct[RightBay]].IdleBrightness;
        }
        else{
          Brightness = Settings.ProdDef[BayProduct[RightBay]].UnselectedBrightness;
        }
        ExchangeBoardMsg(BCAOutput + RightBay, BCTBrightness, 1, Brightness, BCTAck);
        BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
      }
    LastBay = Bay;
  }
  return status;
}
*/
/*
  SetLamp routine

  This routine sets the Lamps.  0 = off, 1 to 8 for the lamp, all others off.
*/
bool SetLamp(Uint8 Lamp)
{
  bool  status;

  if(SlaveMode)
    return false;

  if ((Lamp > 0) && (Lamp <= 16))
    status = SetLamps(1 << (Lamp - 1));  // Set the bit pattern for the Lamps
  else
    status = SetLamps(0);

  return status;
}
