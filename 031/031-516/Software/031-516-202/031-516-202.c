/*
  031-516-202 MP3 program

  This program is for the Lynx Innovation 031-516-202 MP3 controller board and
  MP3 player in the Beats Single/Dual displays.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-516
  It reads button presses from the keyboard controller and uses them to play MP3s.

  Created by David Annett 6 February 2013

  Copyright Lynx Innovation 2013
 */

//====================== Includes =============================================

#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "Keyboard.h"
#include "LampSerial.h"
#include "UART.h"
#include "BoardComms.h"
#include "MP3.h"
#include "Settings.h"
#include "vs.h"
#include "sd.h"

//====================== Constants ============================================

// Keys and lamps

#define VolUp           1
#define PrevTrack       2
#define VolDown         3
#define NextTrack       4
#define LeftSelect      6
#define RightSelect     7
#define PlayPause       5
#define MaxKey          5


// Times in 1mS ticks
#define LoopPeriod         100
#define VolStepDelay       250

// Loop at 10Hz              SSSm
#define PrevTimeDefault        10
extern bool MP3Paused;
static volatile bool gb_KeyCurrentTrackFlag;

//====================== Types ================================================


//====================== Vars =================================================

bool            LCDPresent;
enum TRamp      Ramp;
enum TProducts  Product;
enum TInput     Input;
enum TI2SFormat InputFormat[LCDIn+1];
volatile Uint8  FlashPhase;
Uint8           Key;
Uint8           LastKey;
Uint8           Track;
Uint8           Tracks;
Uint8           Volume;
Uint8           IdleVolume;
Uint8           MinVolume;
Uint8           DefaultVolume;
Uint8           MaxVolume;
Uint16          IdleTime;
Uint8           PrevTime;
Uint8           CmdRxBuf[1];
Uint8           CommsIdleTime;
Uint8           LastCommsSize;

// Forward declares

void MainLoop(Uint8 PlayerStatus);


//====================== Functions ============================================

/*
  SetBassTreble routine

  This routine writes the setting values to the MP3 bass/treble settings
  and displays them.
*/
void SetBassTreble(void)
{
  Uint16 Treble;


  // Write current settings to MP3 chip

  vs_set_bass(Settings.TrebleLevel, Settings.BassLevel,
              Settings.TrebleFreq,  Settings.BassFreq);

  // Show them to the user

  UART_TxStr("Bass   ");
  if (Settings.BassLevel) {
    UART_TxChar('+');
    UART_TxNum(Settings.BassLevel, 1);
    UART_TxStr("dB at ");
    UART_TxNum(Settings.BassFreq, 1);
    UART_TxStr("0Hz");
  } else {
    UART_TxStr("off");
  }

  UART_TxStr("\r\nTreble ");
  if (Settings.TrebleLevel == 0) {
    UART_TxStr("off\r\n");
  } else {
    if (!(Settings.TrebleLevel & 0b10000000)) {
      Treble = Settings.TrebleLevel;
      UART_TxStr("+");
    } else {
      Treble = 256 - Settings.TrebleLevel;
      UART_TxStr("-");
    }
    Treble = Treble * 3 / 2;
    UART_TxNum(Treble, 1);
    if (Settings.TrebleLevel & 0b1)
      UART_TxStr(".5");
    UART_TxStr("dB at ");
    UART_TxNum(Settings.TrebleFreq, 1);
    UART_TxStr("kHz\r\n");
  }
}



/*
  SerialControl routine

  This routine allows changing the settings via the serial port.
 */
void SerialControl(void)
{
  Uint8 Command;


  // See if new command available

  if (!RxAvail)
    return;

  // Grab it and kick of the next read

  Command = CmdRxBuf[0];
  UART_Rx(CmdRxBuf, 1);

  // Process the command

  switch (Command) {
    case 't':   // Test command
      SetBassTreble();
      break;

    default:
      if (!SettingsControl(Command)) {
        UART_TxStr("?");
        UART_TxNum(Command, 1);
        UART_TxStr("?\r\n");
      }
      break;
  }
}





/*
  RefreshLamps routine

  This routine refreshes the state of the lamps
 */
void RefreshLamps(void)
{
#define DirectionLEDS 0b11111
#define LeftLEDs  (1 << (LeftSelect - 1))
#define RightLEDs (1 << (RightSelect - 1))

  Uint8 Lamps;


  Lamps = 0;

  // If idle do attract display

  if (!IdleTime) {
    Lamps = LeftLEDs | RightLEDs | DirectionLEDS;  // All LEDs on

    // else show genre and output

  } else {
    if (Input == MP3In)
      Lamps = LeftLEDs | DirectionLEDS;  // All LEDs on except right
    else
      Lamps = RightLEDs | DirectionLEDS;  // All LEDs on except left
  }

  // If key pressed turn it off it's lamp

  if (Key) {
    Lamps = Lamps & ~(1 << (Key - 1));
  }/* else if (MP3Paused && (FlashPhase & 0b100)) {
    Lamps = Lamps & ~(1 << (PlayPause - 1)); // If paused flash play LED, otherwise on
  }*/

  // Show the final result

  SetLamps(Lamps);
}



/*
  SetVolume routine

  This routine sets the volume.  If at minimum then turns off relay
 */
void SetVolume(Uint8 NewVolume)
{
  static int LastVolume = 0xff;


  UART_TxStr("SetVolume ");
  UART_TxNum(NewVolume, 3);
  UART_TxStr("\r\n");

  if (NewVolume == LastVolume) { // Suppress sending duplicate messages
    return;
  }
  LastVolume = Volume;

  Volume = NewVolume;
  ExchangeBoardMsg(BCAOutput, BCTVolume, Volume, 0, BCTAck);
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
}



/*
  SetInput routine

  This routine sets the input for a bay.
 */
void SetInput(enum TInput NewInput)
{
  enum TInput         MuxInput;
  static enum TInput  LastInput = 0xff;


  MuxInput = NewInput;

  UART_TxStr("SetInput ");
  UART_TxNum(NewInput, 1);

  // Override any input request to a missing source

  if ((MuxInput == LCDIn) && !LCDPresent) {
    UART_TxStr(" overriding as ");
    MuxInput = MP3In;
    UART_TxNum(MuxInput, 1);
  }
  UART_TxStr("\r\n");

  // Ignore requests for same source

  if (MuxInput == LastInput) {
    return;
  }
  LastInput = MuxInput;

  // Handle format changes

  if ((Input > LCDIn) || (InputFormat[Input] != InputFormat[MuxInput])) {
    ExchangeBoardMsg(BCAOutput, BCTAudioFormat, InputFormat[MuxInput], 0, BCTAck);
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  }

  // Change the actual stream

  Input = MuxInput;
  SetMux(MuxInput);

  DelayMS(100);  // Give the audio board a chance to sync to new stream
}



/*
  SetIdleState routine

  This routine sets the audio source, destination and volume to the idle state.
 */
void SetIdleState(void)
{
  UART_TxStr("Setting to idle\r\n");

  // Work out default input for each bay

  if (Settings.IdleInput <= LCDIn)
    SetInput(Settings.IdleInput);
  else if (LCDPresent)
    SetInput(LCDIn);
  else
    SetInput(MP3In);

  // Work out the idle volume for each bay
  Volume = IdleVolume;
  SetVolume(Volume);
  MP3_Pause(false);

  IdleTime = 0;
}



/*
  ShowProducts routine

  This routine shows the products found in each bay.
 */
void ShowProducts(void)
{
  if (!LCDPresent)
    UART_TxStr("No ");
  UART_TxStr("LCD present");
  UART_TxStr("\r\nOutput is ");
  UART_TxNum(Product, 1);
  UART_TxStr("\r\n");
}



/*
  ProcessSelectKey routine

  This routine processes a select button depending on the current mode of
  operation.
 */
void ProcessSelectKey(Uint8 Key)
{
  if (Key == LeftSelect)
    SetInput(MP3In);      // Left key selects MP3
  else
    SetInput(LCDIn);      // Right key selects LCD video
  MP3_Track(1);
  Track = 1;
  if (Volume < DefaultVolume) {
    Ramp = RampDefault;
  }
}



/*
  ProcessPlayKey routine

  This routine processes the play button depending on the current mode of
  operation.
 */
void ProcessPlayKey(Uint8 Key)
{
  if (IdleTime == 0) {      // If was idle
    if(IdleVolume <= 47){ //47 == mute, it si pill or beats box
      Ramp = RampDefault;     // then ramp to default volume current audio source
    }else{
      MP3_Pause(!MP3Paused); //if it is headphones so paused
    }

    if(Product != FiveHeadphones)
    {
      if(!gb_KeyCurrentTrackFlag)//press key and paly current track;if over then paly track 1
      {
        MP3_Track(1);
        Track = 1;
      }
    }
  } else {
    MP3_Pause(!MP3Paused);  // otherwise toggle the paused state of MP3 engine
  }

  gb_KeyCurrentTrackFlag = true;
}



/*
  ProcessTrackKey routine

  Process a key request to change track
 */
void ProcessTrackKey(Uint8 Key)
{
  if (IdleTime == 0) {    // If was idle
    Ramp = RampDefault;
    if(Product != FiveHeadphones)
    {
      if(!gb_KeyCurrentTrackFlag)//press key and paly current track;but not over
      {
        if (Key == NextTrack) // Start at track 1
          Track = 0;
        else
          Track = 1;
      }
    }
  }

  gb_KeyCurrentTrackFlag = true;


  if(MP3Paused == true){
    MP3_Pause(!MP3Paused);
  }
  // Work out next track

  if (Key == NextTrack) {
    if (++Track > Tracks) {
      if (LCDPresent)
        Track = 0;
      else
        Track = 1;
    }
  } else {
    if (PrevTime) {
      if ((LCDPresent && Track) ||
          (!LCDPresent && (Track > 1)))
        Track--;
      else
        Track = Tracks;
    }
    PrevTime = PrevTimeDefault;
  }

  if (Track) {
    MP3_Track(Track);
    SetInput(MP3In);
  } else {
    SetInput(LCDIn);
  }

  RefreshLamps();
}



/*
  ProcessVolumeKey routine

  This routine process a volume key press.
 */
void ProcessVolumeKey(Uint8 Key)
{
  if (IdleTime == 0) {    // If was idle
    if (Settings.DefaultInput <= LCDIn)
      SetInput(Settings.DefaultInput);
    else if (LCDPresent) {
      SetInput(LCDIn);
    } else {
      SetInput(MP3In);
    }
    if(Product != FiveHeadphones){
      if(!gb_KeyCurrentTrackFlag)//press key and paly current track;if over then paly track 1
      {
        MP3_Track(1);
        Track = 1;
      }
      Ramp = RampDefault;
    }
  }

  gb_KeyCurrentTrackFlag = true;

  if (Ramp == RampDefault)
    return;

  if(MP3Paused == true){
    MP3_Pause(!MP3Paused);
  }

  while (GetKey()) {      // Handle key held repeats

    // Do the actual volume change

    if (Key == VolUp) {
      if (Volume < MaxVolume) {
        if (Volume + Settings.VolumeStep <= MaxVolume)
          Volume += Settings.VolumeStep;
        else
          Volume = MaxVolume;
        SetVolume(Volume);
      }
    } else {
      if (Volume >= MinVolume + Settings.VolumeStep)
        Volume -= Settings.VolumeStep;
      else
        Volume = MinVolume;
      SetVolume(Volume);
    }

    // Wait the repeat time keeping the MP3 chip feed and comms running

    Timer_Clear();
    while (Timer_Read() < VolStepDelay) {
      MP3_Process();
      CheckForBoardMsg();
      RefreshLamps();
      wdt_reset();//volune up down long time press so need feed watchdog
    }
    FlashPhase += 3;
  }
}



void ResetSlaveDevice(void)
{
  // For reset the slave board maybe volume very high so have noise
  // so first set volume is 0

  ExchangeBoardMsg(BCAOutput, BCTVolume, 0, 0, BCTAck);
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg

  //reset slave device :keyboard LCD OutPut
  ExchangeBoardMsg(BCAOutput, BCTReset, 0, 0, BCTAck);
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
}


//this is for maybe 10seconds need to send find device and send volume and so on
void PollDevice(void)
{
  static volatile Uint8 times = 0;

  times++;
  switch(times){
    case 5://send the current play volume
      ExchangeBoardMsg(BCAOutput, BCTVolume, Volume, 0, BCTAck);//)){
        //UART_TxStr("Output device lost\r\n");
        //ShowError(ErrorNoOutPut, true);
      //}
      BCMessageReceive(RxBuf);    // Kick off receive of next frame
      break;
    case 10:
      if(!sdok){
        //SD_PowerOff();
        UART_TxStr("SD card can't detect\r\n");
        ShowError(ErrorNoMP3, true);
      }
      break;
    default:
      if(times > 10)
        times = 0;
      break;
  }
}



void ShowVersion(void){

   SetLamps(0);
   DelayMS(1000);
   SetLamp(PlayPause);
   DelayMS(200);
   SetLamps(0);
   DelayMS(200);

   SetLamps(SWVerMajor);
   DelayMS(1000);

   wdt_reset();
   SetLamp(PlayPause);
   DelayMS(200);
   SetLamps(0);
   DelayMS(200);

   SetLamp(PlayPause);
   DelayMS(200);
   SetLamps(0);
   DelayMS(200);

   SetLamps(SWVerMinor);
   DelayMS(1000);

   wdt_reset();
   SetLamp(PlayPause);
   DelayMS(200);
   SetLamps(0);
   DelayMS(200);

   SetLamp(PlayPause);
   DelayMS(200);
   SetLamps(0);
   DelayMS(200);

   SetLamp(PlayPause);
   DelayMS(200);
   SetLamps(0);
   DelayMS(200);

   SetLamps(SWVerFix);
   DelayMS(1000);
   SetLamps(0);
   DelayMS(1000);
   wdt_reset();
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

  PINA = PortPullUpA;
  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;

  wdt_enable(WDTO_8S);//wdt 8s

  // Init the peripherals

  Timer_Init();                               // Set up timers
  UART_Init();
  BCMessageInit(BCAMP3Contoller);

  // Set up key vars
  UART_Rx(CmdRxBuf, 1);
  Track         = 1;
  IdleTime      = 0;
  FlashPhase    = 0;
  Volume        = 0;
  Ramp          = NoRamp;
  LastKey       = 0;
  Product       = UnknownProduct;
  Input         = MP3In;
  SlaveMode     = false;
  gb_KeyCurrentTrackFlag = false;
  sei();      // Enable global interrupts

  // Print product build banner
  UART_TxStr("\r\n======================================\r\n031-516-202 ");
  UART_TxChar('0' + SWVerMajor);
  UART_TxChar('.');
  UART_TxChar('0' + SWVerMinor);
  UART_TxChar('.');
  UART_TxChar('0' + SWVerFix);
  UART_TxChar(' ');
  UART_TxStr(__TIME__);
  UART_TxChar(' ');
  UART_TxStr(__DATE__);
  UART_TxStr("\r\n======================================\r\n");
  SetMux(MP3In);


  // Test the LEDs while bays boot up

  DelayMS(10);                     // Allow some time for keypad to boot up
  UART_TxStr("Testing LEDs\r\n");
  for (TempInt = 1; TempInt <= 2; TempInt++)
    for (Key = 1; Key <= MaxKey; Key++) {
      SetLamp(Key);
      DelayMS(250);
      wdt_reset(); // feed the watchdog
    }

  // Show version number
  ShowVersion();

  SetLamp(VolUp);
  ResetSlaveDevice();
  LoadEEPROMSetting();
  UART_TxStr("Settings:\r\n");
  PrintSettings();

  // Prepare MP3 decoder for work

  //en MP3 init time flag if MP3_Init time > 10 seconds need restart


  wdt_reset(); // feed the watchdog
  SetLamp(NextTrack);
  Timer_Clear();
#if SDPowerCs == 1
  SD_PowerOnx(0);
  SD_PowerOnx(1);
  DelayMS(200);
  set_sd_channel(0);
  MP3_Init();
#elif SDPowerCs == 2
  for(TempInt = 0; TempInt <=1;)
   {
     if(TempInt == 1){
       SD_PowerOffx(0);
       SD_PowerOnx(1);
     }
     else{
       SD_PowerOffx(1);
       SD_PowerOnx(0);
     }
     DelayMS(200);
     set_sd_channel(TempInt);
     MP3_Init();
     if(sdok)
       break;
     TempInt++;
   }
#elif SDPowerCs == 3
  SD_PowerOnx(0);
  SD_PowerOnx(1);
   for(TempInt = 0; TempInt <=1;)
   {
     DelayMS(200);
     set_sd_channel(TempInt);
     MP3_Init();
     if(sdok)
       break;
     TempInt++;
   }
#endif
  MP3_Volume(250);
  UART_TxStr("MP3 init time = ");
  UART_TxNum(Timer_Read(), 1);
  UART_TxStr("mS\r\n");
  // Find the last track on the card

  wdt_reset(); //feed the watchdog
  SetLamp(VolDown);
  for (Tracks = 1; Tracks <= 99; Tracks++)
    if (!MP3_OpenFile(Tracks))
      break;
  Tracks--;

  SetBassTreble();  // Set the audio curve

  wdt_reset(); // feed the watchdog
  // Determine what is connected

  SetLamp(PrevTrack);
  UART_TxStr("Searching for devices\r\n");

  // Look for input devices

  InputFormat[MP3In] = I2S32Bit;
  LCDPresent         = ExchangeBoardMsg(BDCLCD, BCTInquire, 0, 0, BCTInquireAnswer);
  InputFormat[LCDIn] = RxBuf[BCPParam2];
  BCMessageReceive(RxBuf);

  // Look for output device

  if (ExchangeBoardMsg(BCAOutput, BCTInquire, 0, 0, BCTInquireAnswer)) {
    Product = RxBuf[BCPParam1];
    UART_TxStr("Found output device ");
    UART_TxNum(Product, 1);
    IdleVolume    = Settings.ProdDef[Product].IdleVolume;
    MinVolume     = Settings.ProdDef[Product].MinimumVolume;
    DefaultVolume = Settings.ProdDef[Product].DefaultVolume;
    MaxVolume     = Settings.ProdDef[Product].MaximumVolume;
  } else {
    UART_TxStr("No output device found");
    ShowError(ErrorNoOutPut, true);
  }
  UART_TxStr("\r\n");
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg

  // Allow E2 overwrite of detected defaults

   if (Settings.ForceProduct != UnknownProduct) {
     UART_TxStr("E2 replaced product ");
     UART_TxNum(Product, 1);
     UART_TxStr(" with ");
     UART_TxNum(Settings.ForceProduct, 1);
     UART_TxNewLine();
     Product = Settings.ForceProduct;
   }

  ShowProducts();
  MP3_Track(1);

  SetIdleState();
  UART_TxStr("Start up complete\r\n");

  // Enter the main loop
  Timer_Clear();
  for( ; ; ) {              // Run forever
    if (SlaveMode) {        // In slave mode only handle slave mode messages
      CheckForBoardMsg();

    } else {                //  Not in slave mode so feed the MP3 engine

      if (!MP3_Process())
        MainLoop(0);        // Tell the main loop we have stopped playback

      //  Call the main loop if it is due

      if (Timer_Read() >= LoopPeriod) {     // Run the main loop at 10Hz
        Timer_Clear();
        MainLoop(1);        // Tell the main loop we are still playing a track
      }
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
  // Update state info

  wdt_reset();//feed the watchdog

  FlashPhase++;

  if (PrevTime)
    PrevTime--;

  // See if we need to start playback of next track



  if (PlayerStatus == 0) {
    if (IdleTime == 0) {    // If was idle all aways play track 1
      Track = 1;
      gb_KeyCurrentTrackFlag = false;
    }else{
      if (++Track > Tracks)
      Track = 1;
    }
    MP3_Track(Track);
  }

  /*if (!MP3Ready)       // If MP3 player has died
    ShowError(ErrorNoMP3, true);              // Display error then reboot
  */
  // Process an incoming comms

  CheckForBoardMsg();

  // Check for settings changes

  SerialControl();

  // Poll keys

  Key = GetKey();
  /*
  if (Key)
    ClearBit(LogoLEDPort, LogoLED);
  else
    SetBit(LogoLEDPort, LogoLED);
   */

  if (LastKey != Key) {     // Until we know the keyboard is good show it's output
    UART_TxStr("Key ");
    UART_TxNum(Key, 1);
    UART_TxNewLine();
  }

  //  Process key   touched

  switch (Key){
    case LeftSelect:
    case RightSelect:
      if (Key != LastKey)
        ProcessSelectKey(Key);
      break;

    case NextTrack:
    case PrevTrack:
      if (Key != LastKey)
        ProcessTrackKey(Key);
      break;

    case VolUp:
    case VolDown:
      ProcessVolumeKey(Key);
      break;

    case PlayPause:
      if (Key != LastKey)
        ProcessPlayKey(Key);
      break;

    default:
      break;
  }
  LastKey = Key;

  //  Handle volume ramping

  switch (Ramp) {
    case RampDown:
      UART_TxStr("Ramping down\r\n");
       if ((Volume >= MinVolume) &&
           (Volume >= IdleVolume + Settings.VolumeRampStep))
         Volume -= Settings.VolumeRampStep;
      else
        Volume = IdleVolume;
      SetVolume(Volume);
      if (Volume <= IdleVolume) {
        Ramp = NoRamp;
        SetIdleState();
      }
      break;

    case RampDefault:
      UART_TxStr("Ramping to default\r\n");
      if (Volume < DefaultVolume) {
        if (Volume + Settings.VolumeRampStep <= DefaultVolume)
          Volume += Settings.VolumeRampStep;
        else
          Volume = DefaultVolume;
        SetVolume(Volume);
      } else if (Volume > DefaultVolume) {
        if (Volume >= DefaultVolume + Settings.VolumeRampStep)
          Volume -= Settings.VolumeRampStep;
        else
          Volume = DefaultVolume;
        SetVolume(Volume);
      } else {
        Ramp = NoRamp;
      }
      break;

    default:
      break;
  }

  //  Handle time out

  if (Key) {                          // See if currently not idle
    IdleTime = Settings.IdlePeriod;
  } else if (IdleTime && !--IdleTime) {
    Ramp = RampDown;
  }

  PollDevice();

  RefreshLamps();
}
