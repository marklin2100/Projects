/*
  031-505-211 MP3 program

  This program is for the Lynx Innovation 031-505-211 MP3 controller board and
  MP3 player in the Monster headphone display.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-505
  It reads button presses from the touch controller and uses them to play MP3s.
  It also powers on the beatbox via IR.

  Created by David Annett 26 September 2012

  Copyright Lynx Innovation 2012
*/

//#include <avr/eeprom.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "Touch.h"
#include "IR.h"
#include "LampSerial.h"
#include "UART.h"
#include "MP3.h"


// Constants

// Keys

#define VolUp           1
#define VolDown         2
#define Beatbox         4
#define Headphones      5
#define NextTrack       7
#define PrevTrack       8

#define Tracks          6

// Times in 1mS ticks
#define LoopPeriod         100
#define VolStepDelay       333

// Loop at 10Hz              SSSm
#define BeatboxIdlePeriod     300
#define HeadphonesIdlePeriod  300

// Volume related
#define MinBBVolume     135
#define DefaultBBVolume 175
#define MaxBBVolume     255

#define MinHPVolume     165
#define DefaultHPVolume 200
#define MaxHPVolume     255

#define VolumeStep        5
#define VolumeRampStep    5

enum TRamp {NoRamp, RampDown, RampDefault};
enum TOutput {NoOut, HeadphonesOut, BeatboxOut};

// Types


// Vars

enum TRamp    Ramp;
enum TOutput  Output;
Uint8         Track;
Uint8         FlashPhase;
Uint8         Key;
Uint8         LastKey;
Uint8         Volume;
Uint8         TargetVolume;
Uint16        IdleTime;


// Forward declares

void MainLoop(Uint8 PlayerStatus);


// Functions

/*
  RefreshLamps routine

  This routine refreshes the state of the lamps
*/
void RefreshLamps(void)
{
  Uint8 Lamps = 0;


  // If idle do attract display

  if (Output == NoOut) {
    if (FlashPhase & 0b100)
      Lamps = 1 << (Headphones - 1);
    else
      Lamps = 1 << (Beatbox - 1);

  // else show genre and output

  } else {
    if (Output == HeadphonesOut)
      Lamps = 1 << (Headphones - 1);
    else
      Lamps = 1 << (Beatbox - 1);

    if (FlashPhase & 0b100)
      Lamps |= 1 << (NextTrack - 1);
    else
      Lamps |= 1 << (PrevTrack - 1);
  }

  // Overlay the volume buttons

  if ((Key == VolUp) | (Ramp == RampDefault))
    Lamps |= 1 << (VolUp - 1);
  else if ((Key == VolDown) | (Ramp == RampDown))
    Lamps |= 1 << (VolDown - 1);

  // Show the final result

  SetLamps(Lamps);
}



/*
  SetOutput routine

  This routine sets the output device.
*/
void SetOutput (enum TOutput Device)
{
  Output = Device;
  switch (Output) {
    case NoOut:
      HP1Off;
      HP2Off;
      SpkOff;
      break;

    case HeadphonesOut:
      HP1On;
      HP2On;
      SpkOff;
      break;

    case BeatboxOut:
      HP1Off;
      HP2Off;
      SpkOn;
      break;
  }
}



/*
  SetVolume routine

  This routine sets the volume.  If a minimum then turns off relay
*/
void SetVolume(Uint8 Volume)
{
  Uint8 MinVolume;


  MP3_Volume(Volume);

  if (Output == HeadphonesOut)
    MinVolume = MinHPVolume;
  else
    MinVolume = MinBBVolume;

  // Relays off at minimum volume

  if (Volume <= MinVolume) {
    HP1Off;
    HP2Off;
    SpkOff;
  } else {
    SetOutput(Output);
  }
}



/*
  SetDefaultOutput routine

  This routine sets the default output device.
*/
void SetDefaultOutput (void)
{
  MP3_Track(Track);
  SetOutput(BeatboxOut);
}



/*
  main routine

  Program entry point
*/
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
  Uint16 TempInt;


// Set up the I/O lines

  DDRA = PortDirA;
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;
  SetBit(SWInPort,SWIn);  // Turn on pull up for soft reset switch

// Init the peripherals

  SetOutput(NoOut);
  Timer_Init();                               // Set up timers
  UART_Init();

// Final set up

  Track       = 1;
  IdleTime    = 0;
  FlashPhase  = 0;
  Volume      = 0;
  Ramp        = NoRamp;
  LastKey     = 0;

  sei();      // Enable global interrupts

  Timer_Clear();
  MP3_Init();
  SetVolume(0);
  UART_TxStr("MP3 init time = ");
  UART_TxNum(Timer_Read());
  UART_TxStr("mS\r\n");

// Test the LEDs

  UART_TxStr("Testing...");
  for (TempInt = 1; TempInt <= 2; TempInt++)
    for (Key = 1; Key <= 8; Key++) {
      SetLamp(Key);
      DelayMS(200);
    }
  SetLamp(NoLamp);
  UART_Tx((uint8 *)"\r\nReady\r\n", 9);
  MP3_Track(1);

// Blast power on to Beatbox, twice

  ToshibaBlast(0b000010100000);
  ToshibaBlast(0b000010100000);

//
// Enter the main loop
//

  Timer_Clear();
  for( ; ; ) {              // Run forever

//  Feed the MP3 engine

    if (!MP3_Process())
      MainLoop(0);          // Tell the main loop we have stopped playback

//  Call the main loop if it is due

    if (Timer_Read() >= LoopPeriod) {     // Run the main loop at 10Hz
      Timer_Clear();
      MainLoop(1);          // Tell the main loop we are still playing a track
    }

//  Poll soft reset switch

    if(!GetBit(SWInPin, SWIn)) {
      ToshibaBlast(0b000010100000);
      ToshibaBlast(0b000010100000);
      while(!GetBit(SWInPin, SWIn));
    }
  }
  return 0;
}



/*
  MainLoop routine

  This routine should be called ever LoopPeriod.  The PlayerStatus should be 0
  if stopped or 1 is playing.
*/
void MainLoop(Uint8 PlayerStatus)
{
  Uint8 MaxVolume;
  Uint8 MinVolume;

// Update state info

  FlashPhase++;

// Set the volume limits

  if (Output == HeadphonesOut) {
    MaxVolume = MaxHPVolume;
    MinVolume = MinHPVolume;
  } else {
    MaxVolume = MaxBBVolume;
    MinVolume = MinBBVolume;
  }

// Deal with out of range values when changing output

  if (Volume > MaxVolume) {
    Volume = MaxVolume;
    SetVolume(Volume);
  } else if (Volume < MinVolume) {
    Volume = MinVolume;
    SetVolume(Volume);
  }

// See if we need to start playback of next track

  if (PlayerStatus == 0) {
    if (++Track > Tracks)
      Track = 1;
    MP3_Track(Track);
  }

// Poll keys

  Key = GetKey();

//  Process key touched

  switch (Key){
    case Beatbox:
      if (Key != LastKey) {
        if (Output == BeatboxOut) {
          Ramp = RampDown;
        } else {
          Volume = MinBBVolume;
          SetVolume(Volume);
          if (Output == NoOut) {
            Track = 1;
            SetDefaultOutput(); // Will set genre and start playback
          }
          SetOutput(NoOut);
          Timer_Clear();
          while (Timer_Read() < 500) { // Wait half a second before unmuting
            if (!MP3_Process())        //  Feed the MP3 engine
              if (++Track > Tracks)
                Track = 1;
            MP3_Track(Track);
          }
          Ramp = RampDefault;
          SetOutput(BeatboxOut);
        }
      }
      break;

    case Headphones:
      if (Key != LastKey) {
        if (Output == HeadphonesOut) {
          Ramp = RampDown;
        } else {
          Volume = MinHPVolume;
          SetVolume(Volume);
          if (Output == NoOut) {
            Track = 1;
            SetDefaultOutput(); // Will set genre and start playback, output overridden below
          }
          SetOutput(NoOut);
          Timer_Clear();
          while (Timer_Read() < 500) { // Wait half a second before unmuting
            if (!MP3_Process())        //  Feed the MP3 engine
              if (++Track > Tracks)
                Track = 1;
            MP3_Track(Track);
          }
          Ramp = RampDefault;
          SetOutput(HeadphonesOut);
        }
      }
      break;

    case NextTrack:
        if (Output == NoOut) {    // If was idle
          Ramp = RampDefault;
          SetOutput(BeatboxOut);
        }
        if (++Track > Tracks)
          Track = 1;
        MP3_Track(Track);
        RefreshLamps();
        while (GetKey() == Key) {
          DelayMS(100);
          MP3_Process();
        }
      break;

    case PrevTrack:
        if (Output == NoOut) {    // If was idle
          Ramp = RampDefault;
          SetOutput(BeatboxOut);
        }
        if (!--Track)
          Track = Tracks;
        MP3_Track(Track);
        RefreshLamps();
        while (GetKey() == Key) {
          DelayMS(100);
          MP3_Process();
        }
      break;

    case VolUp:
      Ramp = NoRamp;
      RefreshLamps();
      while (GetKey() == VolUp) {
        if (Volume < MaxVolume) {
          if (Volume + VolumeStep <= MaxVolume)
            Volume += VolumeStep;
          else
            Volume = MaxVolume;
          SetVolume(Volume);
        }
        Timer_Clear();
        while (Timer_Read() < VolStepDelay)
          MP3_Process();
      }
      break;

    case VolDown:
      Ramp = NoRamp;
      RefreshLamps();
      while (GetKey() == VolDown) {
        if (Volume >= MinVolume + VolumeStep)
          Volume -= VolumeStep;
        else
          Volume = MinVolume;
        SetVolume(Volume);
        Timer_Clear();
        while (Timer_Read() < VolStepDelay)
          MP3_Process();
      }
      break;

    default:
      break;
  }
  LastKey = Key;

//  Handle volume ramping

  switch (Ramp) {
    case RampDown:
      if (Volume >= MinVolume + VolumeRampStep)
        Volume -= VolumeRampStep;
      else
        Volume = MinVolume;
      SetVolume(Volume);
      if (Volume <= MinVolume) {
        Ramp = NoRamp;
        SetOutput(NoOut);
      }
      break;

    case RampDefault:
      if (Output == HeadphonesOut)
        TargetVolume = DefaultHPVolume;
      else
        TargetVolume = DefaultBBVolume;

      if (Volume < TargetVolume) {
        if (Volume + VolumeRampStep <= TargetVolume)
          Volume += VolumeRampStep;
        else
          Volume = TargetVolume;
        SetVolume(Volume);
      } else if (Volume > TargetVolume) {
        if (Volume >= TargetVolume + VolumeRampStep)
          Volume -= VolumeRampStep;
        else
          Volume = TargetVolume;
        SetVolume(Volume);
      } else {
        Ramp = NoRamp;
      }
      break;

    default:
      break;
  }

//  Handle time out

  if (Key) {   // See if currently not idle
    if ((Output == NoOut) && (!((Key == Beatbox) || (Key == Headphones)))) {
      Track = 1;
      SetDefaultOutput();
    }
    if (Output ==  BeatboxOut)
      IdleTime = BeatboxIdlePeriod;
    else
      IdleTime = HeadphonesIdlePeriod;
  } else if (Output != NoOut) {              // Idle, see if it has been too long
    if (IdleTime && !--IdleTime) {
      if (((Output == HeadphonesOut) && (Volume > MinHPVolume)) ||  // Only ramp down if above minimum
          ((Output != HeadphonesOut) && (Volume > MinBBVolume))) {
        Ramp = RampDown;
      } else {
        Ramp = NoRamp;
        SetOutput(NoOut);
      }
    }
  }

  RefreshLamps();
}
