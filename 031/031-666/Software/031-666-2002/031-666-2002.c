/*
  031-517-202 MP3 program

  This program is for the Lynx Innovation 031-517-202 MP3 controller board and
  MP3 player in the Beats 8' inline headphone display.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517
  It reads button presses from the keyboard controller and uses them to play MP3s.

  Created by David Annett 26 September 2012

  Copyright Lynx Innovation 2012

  History
  2014-02-27: version 1.3.0  changes
    1>Remove LED start test pattern
    2>Remove LED progress display
    3>remove software version number display
    4>Add error codes (TError),where possible,for points in start up that up that may have stalled before with a progress display
    5>Remove animation before/after error code display.
    6>Add output bay brightness control if bay have two pill or similar
 */

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


// Constants


// Times in 1mS ticks
#define LoopPeriod         100
#define VolStepDelay       250

// Loop at 10Hz              SSSm
#define PrevTimeDefault        10

// Types

// Vars

enum TConfig    Config;
enum TRamp      Ramp;
enum TBay       Bay;
enum TProducts  Product;
enum TInput     Input;
Uint8           BayProduct[NoBay+1];
enum TBay       HeadphoneBay;
Uint8           BayCount;
bool            InputPresent[RightTablet+1];
enum TI2SFormat InputFormat[RightTablet+1];
Uint8           BaySource[NoBay+1];
Uint8           FlashPhase;
Uint8           Key;
Uint8           LastKey;
Uint8           Track;
Uint8           Tracks;
Uint8           Volume;
Uint8           IdleVolume;
Uint8           MinVolume;
Uint8           DefaultVolume;
Uint8           MaxVolume;
Uint8           PreMuteVolume;  // Volume before pause button muted it, also flag of pause mute mode
Uint8           LastBayVolume[NoBay+1];
Uint16          IdleTime;
Uint8           PrevTime;
Uint8           CmdRxBuf[1];
Uint8           CommsIdleTime;
Uint8           LastCommsSize;
// Forward declares

void MainLoop(Uint8 PlayerStatus);
void SetBay (enum TBay Device);
enum TProducts ReadID(enum TBay Bay);



// Functions

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
  ConfigDevices routine

*/
void ConfigDevices(void)
{
  Uint16 TempInt;
  Uint8  TempByte;


  // Set up lamp brightness

  for (TempInt = 0; TempInt < KBSettings; TempInt++) {
    TempByte = Settings.LampBrightness[TempInt];
    if (TempByte != 0xFF) {
      ExchangeBoardMsg(BCAKeypadController, BCTLampBrightness, TempByte, 0, BCTAck);
      BCMessageReceive(RxBuf);
    }
  }
  // Set up headphones
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
      MP3_Volume(250);
      UART_TxStr("MP3Ready ");
      UART_TxNum(MP3Ready, 1);
      UART_TxStr(", MP3Paused ");
      UART_TxNum(MP3Paused, 1);
      UART_TxNewLine();
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
  ReadID routine

  This routine reads the ID number of the currently selected bay
 */
enum TProducts ReadID(enum TBay IDBay)
{
  enum TProducts Result;


  if (ExchangeBoardMsg(IDBay, BCTInquire, 0, 0, BCTInquireAnswer)) {
    Result = RxBuf[BCPParam1];
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
    if (Result > UnknownProduct)
      Result = UnknownProduct;
    return Result;
  }

  return UnknownProduct;
}

/*
  RefreshLamps routine

  This routine refreshes the state of the lamps
 */
void RefreshLamps(void)
{
  Uint8 Lamps;

  Lamps = 0;
  // If idle do attract display

  if (!IdleTime) {
    Lamps = LeftLEDs | RightLEDs | DirectionLEDS;  // All LEDs on
    // else show genre and output

  } else {
      if (Bay == LeftBay)
        Lamps = LeftLEDs | DirectionLEDS;  // All LEDs on except right
      else if (Bay == RightBay)
        Lamps = RightLEDs | DirectionLEDS;  // All LEDs on except left
  }

  // If MP3 player not working turn off track select LEDs

  if (!InputPresent[MP3In]) {
    Lamps = Lamps & ~((1 << (PrevTrack - 1)) | (1 << (NextTrack - 1)));
  }
  // If key pressed turn it off it's lamp

  if (Key) {
    Lamps = Lamps & ~(1 << (Key - 1));
  }

  // Show the final result

  SetLamps(Lamps);

}


/*
  SetVolume routine

  This routine sets the volume.  If at minimum then turns off relay
 */
void SetVolume(Uint8 NewVolume)
{
  static enum TBay  LastBay = NoBay;
  static Uint8      LastVolume;


  if (BayProduct[Bay] == UnknownProduct)  // Don't set the volume of unknown products
    return;

  if ((Bay == LastBay) && (NewVolume == LastVolume)) // Suppress sending duplicate messages
    return;

  LastBay = Bay;
  LastVolume = NewVolume;

  if (NewVolume)        // If we are not muting the clear the premuted volume/flag
    PreMuteVolume = 0;

  if (Bay != NoBay) {
    UART_TxStr("SetVolume ");
    UART_TxNum(NewVolume, 3);
    UART_TxStr(" for bay ");
    UART_TxNum(Bay, 1);
    UART_TxNewLine();
    ExchangeBoardMsg(BCAOutput, BCTVolume, NewVolume, 0, BCTAck);
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  }
}



/*
  SetBay routine

  This routine sets the output device bay.
 */
void SetBay (enum TBay Device)
{
  enum TBay       PWMBay;
  enum TProducts  PWMProduct;


  if (Device > NoBay) // Ignore bogus requests
    return;
  // If idle scan thru all bays setting them to idle state


  PWMBay = Bay;
  for (Bay = LeftBay; Bay < NoBay; Bay++) {
    PWMProduct = BayProduct[Bay];
    if (PWMProduct != UnknownProduct)
      SetVolume(0);
  }
  Bay = PWMBay;

  // Set global vars to match the product selected

  Bay           = Device;
  Product       = BayProduct[Bay];
  IdleVolume    = Settings.ProdDef[Product].IdleVolume;
  MinVolume     = Settings.ProdDef[Product].MinimumVolume;
  DefaultVolume = Settings.ProdDef[Product].DefaultVolume;
  MaxVolume     = Settings.ProdDef[Product].MaximumVolume;

  UART_TxStr("Bay ");
  UART_TxNum(Bay, 1);
  UART_TxStr(" Idle ");
  UART_TxNum(IdleVolume, 1);
  UART_TxStr(" Min ");
  UART_TxNum(MinVolume, 1);
  UART_TxStr("  Def ");
  UART_TxNum(DefaultVolume, 1);
  UART_TxStr(" Max ");
  UART_TxNum(MaxVolume, 1);
  UART_TxStr(" Cur ");
  UART_TxNum(Volume, 1);
  UART_TxNewLine();

  // Select the bay

  if (Device == NoBay)
    return;

  // Deal with out of range values when changing output

  if (Volume > MaxVolume) {
    Volume = MaxVolume;
  } else if (IdleTime && (Volume < MinVolume)) {
    Volume = MinVolume;
  }
}



/*
  SetDefaultBay routine

  This routine sets the default output device bay.
 */
void SetDefaultBay (void)
{
  enum TBay DefaultBay;

  if (Settings.DefaultBay < NoBay)
    DefaultBay = Settings.DefaultBay;
  else if (HeadphoneBay < NoBay)
    DefaultBay = HeadphoneBay;
  else if (BayProduct[LeftBay] < UnknownProduct)
    DefaultBay = LeftBay;
  else if (BayProduct[RightBay] < UnknownProduct)
    DefaultBay = RightBay;
  else
    DefaultBay = CenterBay;

  SetBay(DefaultBay);
}



/*
  SetIdleState routine

  This routine sets the audio source, destination and volume to the idle state.
 */
void SetIdleState(void)
{
  enum TBay TempBay;


  UART_TxStr("Setting to idle\r\n");
  IdleTime = 0;

  // Work out default input for each bay

  SetDefaultBay();
  TempBay = Bay;

  // Work out the idle volume for each bay

  UART_TxStr("Setting idle volumes\r\n");
  for (Bay = LeftBay; Bay <= RightBay; Bay++) {
    LastBayVolume[Bay] = Settings.ProdDef[BayProduct[Bay]].DefaultVolume;
    UART_TxStr("Setting bay ");
    UART_TxNum(Bay, 1);
    UART_TxStr(" default volume ");
    UART_TxNum(LastBayVolume[Bay], 3);
    UART_TxNewLine();
    Volume = Settings.ProdDef[BayProduct[Bay]].IdleVolume;
    SetVolume(Volume);
  }
  Bay = TempBay;

  Volume = IdleVolume;
  UART_TxStr("Idle volume is ");
  UART_TxNum(Volume, 1);
  UART_TxNewLine();

  if (MP3Paused)      // If paused then unpause
    MP3_Pause(false);
}



/*
  ShowProducts routine

  This routine shows the products found in each bay.
 */
void ShowProducts(void)
{
  Uint8 Pos;
  UART_TxStr("\r\nOutputs are:");
  for (Pos = LeftBay; Pos <= RightBay; Pos++) {
    UART_TxChar(' ');
    UART_TxNum(BayProduct[Pos], 1);
  }
  UART_TxNewLine();
}



/*
  ProcessSelectKey routine

  This routine processes a select button depending on the current mode of
  operation.
 */
void ProcessSelectKey(Uint8 Key)
{
  Uint8 MappedKey;

  MappedKey = Key;  // Default to no key remapping
  // Handle settings over rides

    UART_TxStr("Product selection ");
    if (IdleTime &&                                             // If not idle
        (((MappedKey == LeftSelect)   && (Bay == LeftBay)) ||   // and selecting the current bay
            ((MappedKey == RightSelect)  && (Bay == RightBay)))) {
      Ramp = RampDown;                                    // Causes us to return to idle mode
      UART_TxStr("off\r\n");
      while (GetKey() == Key) {   // Wait for key up before starting idle process
        MP3_Process();
        RefreshLamps();
      }
      DelayMS(10);

    } else {                  // Selecting a new bay
      UART_TxNum(MappedKey, 1);
      UART_TxNewLine();
      if (IdleTime == 0) {    // If was idle
        if (IdleVolume > MinVolume)
          Volume = IdleVolume;
        if (TwoBaysOneSource) {
          MP3_Track(1);
          Track = 1;
        }
        IdleTime = Settings.IdlePeriod;
        Volume = MinVolume;
        Ramp = RampDefault;
      } else {
        if (HeadphoneBay == NoBay) {
          LastBayVolume[Bay] = Volume;  // Save the volume for if we return to bay
          UART_TxStr("Saving bay ");
          UART_TxNum(Bay, 1);
          UART_TxStr(" volume ");
          UART_TxNum(Volume, 3);
          UART_TxNewLine();
        } else {
          Volume = MinVolume;
          Ramp = RampDefault;
        }
      }
      if (MappedKey == LeftSelect) {
        SetBay(LeftBay);
      } else if (MappedKey == CenterSelect) {
        SetBay(CenterBay);
      } else {
        SetBay(RightBay);
      }
      if ((HeadphoneBay == NoBay) && (IdleTime != Settings.IdlePeriod)) {
        Volume = LastBayVolume[Bay];  // Restore the volume from last time bay was active
        UART_TxStr("Restoring bay ");
        UART_TxNum(Bay, 1);
        UART_TxStr(" volume ");
        UART_TxNum(Volume, 3);
        UART_TxNewLine();
        SetVolume(Volume);
      }
    }
}



/*
  ProcessTrackKey routine

  Process a key request to change track
 */
void ProcessTrackKey(Uint8 Key)
{
  if (!InputPresent[MP3In])     // If MP3 player is dead then ignore these keys
    return;

  MP3_Pause(false);             // Cancel any pause that may have been active

  if (IdleTime == 0) {          // If was idle
    Track = 1;
    Ramp = RampDefault;
    IdleTime = Settings.IdlePeriod;
    SetDefaultBay();

  } else if (Input != MP3In) {  // If was not MP3 input
    Track = 1;
    //    Ramp = RampDefault;

  } else {                      // MP3 is currently active input
    if (Key == NextTrack) {     // Work out next track
      if (++Track > Tracks)
        Track = 1;
    } else {
      if (PrevTime)
        if (!--Track)
          Track = Tracks;
      PrevTime = PrevTimeDefault;
    }
  }
  MP3_Track(Track);

  // Force to MP3 player and wait for key release
  RefreshLamps();
}



/*
  ProcessPlayKey routine

  This routine processes the play/pause button depending on the current mode of
  operation.
 */
void ProcessPlayKey(Uint8 Key)
{
  if (IdleTime == 0) {                // If was idle
    if (IdleVolume > 47){             // If not muted then we have headphones
      if (Input == MP3In) {
        MP3_Pause(!MP3Paused);        // otherwise toggle the paused state of MP3 engine
      } else {
        if (PreMuteVolume) {
          Volume = PreMuteVolume;
          SetVolume(Volume);
        } else {
          PreMuteVolume = Volume;
          Volume = 0;
          SetVolume(Volume);
        }
      }
    }else{
      ProcessTrackKey(NextTrack);   // The process the same as if next track key pressed
    }
  } else {
    if (Input == MP3In) {
      MP3_Pause(!MP3Paused);        // otherwise toggle the paused state of MP3 engine
    } else {
      if (PreMuteVolume) {
        Volume = PreMuteVolume;
        SetVolume(Volume);
      } else {
        PreMuteVolume = Volume;
        Volume = 0;
        SetVolume(Volume);
      }
    }
  }
}



/*
  ProcessVolumeKey routine

  This routine process a volume key press.
 */
void ProcessVolumeKey(Uint8 Key)
{
  if (IdleTime == 0) {    // If was idle
    IdleTime = Settings.IdlePeriod;
    SetDefaultBay();
    if(IdleVolume <= 47){  //no headphone up to default
      MP3_Track(1);
      Track = 1;
      Ramp = RampDefault;
    }
  }

  if (Ramp == RampDefault)
    return;

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
      wdt_reset();  // Volume up down long time press, so need feed watchdog
    }
    FlashPhase += 3;
  }
}


void SearchDevices(void)
{
  Uint8 TempInt;
  UART_TxStr("Searching for devices\r\n");

  // Look for input devices

  InputPresent[MP3In]      = MP3Ready && Tracks;
  InputFormat[MP3In]       = I2S32Bit;

  // Look for output devices

  wdt_reset();
  HeadphoneBay = NoBay;
  BayCount = 0;
  for(Bay = LeftBay; Bay <= RightBay; Bay++) {
    if (ExchangeBoardMsg(BCAOutput, BCTInquire, 0, 0, BCTInquireAnswer)) {
      BayCount++;
      UART_TxStr("Found bay ");
      UART_TxNum(Bay, 1);
      UART_TxStr(" = ");
      BayProduct[Bay] = RxBuf[BCPParam1];
      if (BayProduct[Bay] == FiveHeadphones)
        HeadphoneBay = Bay;
      UART_TxNum(BayProduct[Bay], 1);
      UART_TxNewLine();
    }
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  }
  Bay = LeftBay;
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  wdt_reset();

  // Allow E2 overwrite of detected defaults
  if (Settings.LeftProduct != UnknownProduct) {
    UART_TxStr("E2 replaced left product ");
    UART_TxNum(BayProduct[LeftBay], 1);
    UART_TxStr(" with ");
    UART_TxNum(Settings.LeftProduct, 1);
    UART_TxNewLine();
    BayProduct[LeftBay] = Settings.LeftProduct;
  }

  if (Settings.CenterProduct != UnknownProduct) {
    UART_TxStr("E2 replaced center product ");
    UART_TxNum(BayProduct[CenterBay], 1);
    UART_TxStr(" with ");
    UART_TxNum(Settings.CenterProduct, 1);
    UART_TxNewLine();
    BayProduct[CenterBay] = Settings.CenterProduct;
  }

  if (Settings.RightProduct != UnknownProduct) {
    UART_TxStr("E2 replaced right product ");
    UART_TxNum(BayProduct[RightBay], 1);
    UART_TxStr(" with ");
    UART_TxNum(Settings.RightProduct, 1);
    UART_TxNewLine();
    BayProduct[RightBay] = Settings.RightProduct;
  }

  ShowProducts();


  // Determine operational mode

  // TODO: Check for override in E2 config

  wdt_reset();
  Config = ThreeBaysTwoSources;   // Default mode
  if (!InputPresent[LCDIn]) {
    Config = ThreeBaysOneSource;
    if (!InputPresent[MP3In])   // If no valid inputs, display error and reboot
      ShowError(ErrorNoSource, true);
    if(BayCount < 3)
      Config = TwoBaysOneSource;
    if(BayCount < 2)
      Config = OneBayOneSource;
    if (!BayCount)             // If no valid outputs, display error and reboot
      ShowError(ErrorNoBays, true);
  } else {
    if (BayCount < 3)
      Config = TwoBaysTwoSources;
    if (BayCount < 2)
      Config = OneBayTwoSources;
    if (!BayCount)             // If no valid outputs, display error and reboot
      ShowError(ErrorNoBays, true);
  }

  UART_TxStr("Mode: ");
  switch (Config) {
    case ThreeBaysTwoSources: UART_TxStr("Three bays, two sources\r\n"); break;
    case ThreeBaysOneSource:  UART_TxStr("Three bays, one source\r\n"); break;
    case TwoBaysTwoSources:   UART_TxStr("Two bays, two sources\r\n"); break;
    case TwoBaysOneSource:    UART_TxStr("Two bays, one source\r\n"); break;
    case OneBayTwoSources:    UART_TxStr("One bay, two sources\r\n"); break;
    case OneBayOneSource:     UART_TxStr("One bay one source\r\n");break;
  }
}


void Port_Init(void){
  // Set up the I/O lines

  DDRA = PortDirA;
  DDRB = PortDirB;
  SD_PowerOn();
  DDRC = PortDirC;
  DDRD = PortDirD;

  PINA = PortPullUpA;
  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;

}

/*
 * the variables initialization
 */
void VarInit(void){
  Track         = 1;
  IdleTime      = 0;
  FlashPhase    = 0;
  Volume        = 0;
  PreMuteVolume = 0;
  Ramp          = NoRamp;
  LastKey       = 0;
  for(Bay = LeftBay; Bay <= NoBay; Bay++) {
    BayProduct[Bay] = UnknownProduct;
    BaySource[Bay]  = 0xff;
  }
  Bay           = LeftBay;
  SlaveMode     = false;
  SlaveModePara = 0;
  LastError = 0;            // Indicate no errors yet
}

/*
 * PrintSW_Version
 * print the software version and compile time and project name
 */
void PringSW_Version(void){
  // Print product build banner
  wdt_reset();
  UART_TxStr("\r\n======================================\r\n031-517-202 ");
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
}


void ClearBayVolume(void){
  wdt_reset();
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  ExchangeBoardMsg(BCALeftBay, BCTVolume, 0, 0, BCTAck);
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  ExchangeBoardMsg(BCACenterBay, BCTVolume, 0, 0, BCTAck);
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
  ExchangeBoardMsg(BCARightBay, BCTVolume, 0, 0, BCTAck);
  BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
}

/*
  main routine

  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
  Uint16 TempInt;


  // Init the peripherals
  Port_Init();
  Timer_Init();                               // Set up timers
  UART_Init();
  UART_Rx(CmdRxBuf, 1);
  InitKey();
  BCMessageInit(BCAMP3Contoller);
  VarInit();
  wdt_enable(WDTO_8S);//watch dog 8s
  sei();      // Enable global interrupts
  PringSW_Version();
  ClearBayVolume();

  // Load EEPROM settings
  LoadEEPROMSetting();
  UART_TxStr("Settings:\r\n");
  PrintSettings();


  if(!SetLamp(0)){
    SlaveMode = true;
  }
  //wait for tablet ready wait 60 seconds
  if(SlaveMode)
  {
    UART_TxStr("Wait for tablet ready\r\n");
    for(TempInt = 0; TempInt < 6000; TempInt++)
    {
      DelayMS(10);
      CheckForBoardMsg();
      wdt_reset();
    }
  }

  // Prepare MP3 decoder for work
  wdt_reset();       // Feed the watchdog
  DelayMS(100);
  Timer_Clear();
  MP3_Init();
  MP3_Volume(250);
  UART_TxStr("MP3 init time = ");
  UART_TxNum(Timer_Read(), 1);
  UART_TxStr("mS\r\n");

  // Find the last track on the card
  wdt_reset();
  for (Tracks = 1; Tracks <= 99; Tracks++) {
    if (!MP3_OpenFile(Tracks))
      break;
  }
  Tracks--;
  SetBassTreble();  // Set the audio curve

  // Determine what is connected
  wdt_reset();
  SearchDevices();

  if(!SlaveMode)
  {
    ConfigDevices();    // Configure connected devices
    SetIdleState();
    MP3_Track(1);
    if (!InputPresent[MP3In])       // If no MP3 player display error
      ShowError(ErrorNoMP3, false);
  }
  UART_TxStr("Start up complete\r\n");

  //
  // Enter the main loop
  //

  Timer_Clear();
  SlaveModeTimerClear();
  for( ; ; ) {              // Run forever
    if (SlaveMode) {        // In slave mode only handle slave mode messages
      CheckForBoardMsg();
      wdt_reset();  // Update the watchdog
    } else {                //  Not in slave mode so feed the MP3 engine

      if (MP3Ready && !MP3_Process())
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
  wdt_reset();  // Update the watchdog

  FlashPhase++;

  if (PrevTime)
    PrevTime--;

  // Check for failure of MP3 player

  if (InputPresent[MP3In] && !MP3Ready)       // If MP3 player has died
    ShowError(ErrorNoMP3, true);              // Display error then reboot


  // See if we need to start playback of next track

  if (InputPresent[MP3In] && (PlayerStatus == 0)) {
    if (++Track > Tracks) {
      Track = 1;
      if (!IdleTime) {
        UART_TxStr("\r\nAllowing SD card to recover\r\n");
        DelayMS(3000);
      }
    }
    if(!MP3_Track(Track)){
      UART_TxStr("\r\nCan't open track, waiting for watchdog reset\r\n");
      DelayMS(10000);   // Force the watchdog to activate
    }
  }

  // Process an incoming comms

  CheckForBoardMsg();

  // Check for settings changes

  SerialControl();

  // Poll keys

  Key = GetKey();

  if(Config==OneBayOneSource){//only one bay,so not need left and right bay
    if((Key==LeftSelect) || (Key==RightSelect)){
      Key = 0;
    }
  }else if((Config==OneBayTwoSources) && (!MP3Ready)){ //
    if((Key==LeftSelect) || (Key==RightSelect) || (Key==PrevTrack) || (Key==NextTrack)){
      Key = 0;
    }
  }else if(!MP3Ready){
    if((Key==PrevTrack) || (Key==NextTrack)){ //if Mp3 not ready so next and prev key not need
      Key = 0;
    }
  }


  if (!Settings.PauseKeyEnabled && (Key == PlayPause))  // Handle disabled
    Key = LastKey;

  if (LastKey != Key) {     // Until we know the keyboard is good show it's output
    UART_TxStr("Key ");
    UART_TxNum(Key, 1);
    UART_TxNewLine();
  }

  // Handle error code display request

  if (Key && (LastKey != Key) && KonamiCheck(Key))     // Until we know the keyboard is good show it's output
    ShowError(LastError, false);


  // Handle un-pausing

  if (Key && (Key != PlayPause)) { // && InputPresent[MP3In]
    if (MP3Paused)                // If paused then unpause
      MP3_Pause(false);
    if (PreMuteVolume) {          // If muted then unmute
      Volume = PreMuteVolume;
      SetVolume(Volume);
    }
  }

  // Process key touched

  switch (Key){
    case LeftSelect:
    case RightSelect:
      if (Key != LastKey){
        if(Config!=OneBayOneSource)//no need to select
          ProcessSelectKey(Key);
      }
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

  //  Handle time out

  if (Key) {                          // See if currently not idle
    IdleTime = Settings.IdlePeriod;
  } else if (IdleTime && !--IdleTime) {
    UART_TxStr("Idle timeout\r\n");
    Ramp = RampDown;
  }

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

  if(!SlaveMode){
    RefreshLamps();
  }
}
