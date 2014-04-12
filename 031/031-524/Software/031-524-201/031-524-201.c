/*
  031-517-202 MP3 program

  This program is for the Lynx Innovation 031-517-202 MP3 controller board and
  MP3 player in the Beats 8' inline headphone display.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517
  It reads button presses from the keyboard controller and uses them to play MP3s.

  Created by Mark Lin 26 September 2013

  Copyright Lynx Innovation 2013
 */

#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "UART.h"
#include "BoardComms.h"
#include "MP3.h"
#include "vs.h"
#include <avr/eeprom.h>

// Constants

// Keys and lamps
// Times in 1mS ticks
#define LoopPeriod         100
#define VolStepDelay       250

// Loop at 10Hz              SSSm
#define PrevTimeDefault        10

// Types
extern volatile Uint16 CountUp2;
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
bool            BayNotSourceButtons;
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


volatile Uint16 timer;
// Forward declares

void MainLoop(Uint8 PlayerStatus);


//====================== Constants ============================================

#define EELocID         0x0000
#define EELocCustomer   0x0004
#define EELocProject    0x0006
#define EELocPartNumber 0x0008
#define EELocVersion    0x000A
#define EELocData       0x0010

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
  SetVolume routine

  This routine sets the volume.  If at minimum then turns off relay
 */
void SetVolume(Uint8 NewVolume)
{

    UART_TxStr("SetVolume ");
    UART_TxNum(NewVolume, 3);
    UART_TxNewLine();
    ExchangeBoardMsg(BCAOutput, BCTVolume, NewVolume, 0, BCTAck);
    BCMessageReceive(RxBuf);      // Finished with it so get ready for next msg
}


void SwitchToMP3(void)
{
  UART_TxStr("\r\nCountUp2 = ");
  UART_TxNum(CountUp2,5);
  UART_TxNewLine();
  Input = MP3In;
  SetVolume(83);
  DelayMS(200);
  UART_TxStr("Switch to Mp3\r\n");
  ClearBit(SelAPort, Sel1A);
  SetBit(SelAPort, Sel0A);
  //To MP3 in

}

void SwitchToTablet(void)
{
  UART_TxStr("\r\nCountUp2 = ");
  UART_TxNum(CountUp2,5);
  UART_TxStr("\r\nSwitch to Tablet\r\n");
  Input = LeftTablet;
  ClearBit(SelAPort, Sel0A);
  ClearBit(SelAPort, Sel1A);
}

// Both blocks bad, load defaults and save them

/*
  ReadSetting routine

  This routine reads a setting
 */
Uint8 ReadSetting(Uint8 Number)
{
  Uint8 *Ptr;


  Ptr = (Uint8 *)&Settings;
  if (Number < sizeof(Settings))
    return Ptr[Number + EELocData];

  return 0;
}


/*
  PrintSettings routine

  This routine prints a settings.
 */
void PrintSetting(Uint8 Number)
{
  UART_TxChar('[');
  UART_TxNum(Number, 3);
  UART_TxStr("]=");
  UART_TxNum(ReadSetting(Number), 3);
  UART_TxNewLine();
}

/*
  PrintSettings routine

  This routine prints the current values of all settings.
 */
void PrintSettings(void)
{
  Uint8 Pos;


  UART_TxNum(Settings.Customer, 3);
  UART_TxChar('-');
  UART_TxNum(Settings.Project, 3);
  UART_TxChar('-');
  UART_TxNum(Settings.PartNumber, 3);
  UART_TxChar('_');
  UART_TxNum(Settings.Version, 1);
  UART_TxNewLine();
  for (Pos = 0; Pos < sizeof(Settings) - EELocData; Pos++)
    PrintSetting(Pos);
}


/*
  SaveEEPROMSetting

  This routine saves the settings to the EEPROM
 */
void SaveEEPROMSetting(void)
{
  UART_TxStr("Saving settings\r\n");

  memcpy_PF(&Settings, &DefaultSettings, sizeof(Settings));
  // Save the primary block

  eeprom_update_block((const void *) &Settings, (void *) 0x00, 0x80);
  // Save the backup block
  eeprom_update_block((const void *) &Settings, (void *) 0x80, 0x80);
}
/*
  main routine

  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{

  // Set up the I/O lines

  DDRA = PortDirA;
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;

  PINA = PortPullUpA;
  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;


  SetBit(DongleLEDPort,DongleLED);
  //set the channel is the keyboard.
  //mp3 init finish ,after reset the slave board have volume;
  SetBit(SelAPort, Sel0A);
  SetBit(SelAPort, Sel1A);

  // Init the peripherals

  Timer_Init();                               // Set up timers
  UART_Init();
  BCMessageInit(BCAMP3Contoller);
  BCMessageReceive(RxBuf);      //
  // Set up key vars

  UART_Rx(CmdRxBuf, 1);
  SlaveMode     = false;

  sei();      // Enable global interrupts

  // Print product build banner

  UART_TxStr("\r\n================================\r\n031-524-201_");
  UART_TxChar(0x30+PISettingsVersion/10);
  UART_TxChar(0x30+PISettingsVersion%10);
  UART_TxChar(' ');
  UART_TxStr(__TIME__);
  UART_TxChar(' ');
  UART_TxStr(__DATE__);
  UART_TxStr("\r\n================================\r\n");

  DelayMS(10);                     // Allow some time for keypad to boot up
  // Prepare MP3 decoder for work
  SaveEEPROMSetting();
  PrintSettings();
  Timer_Clear();
  SD_PWR_ON();
  DelayMS(200);
  MP3_Init();
  UART_TxStr("MP3 init time = ");
  UART_TxNum(Timer_Read(), 1);
  UART_TxStr("mS\r\n");

  for (Tracks = 1; Tracks <= 99; Tracks++) {
    if (!MP3_OpenFile(Tracks))
      break;
  }
  Tracks--;

  SetBassTreble();  // Set the audio curve

  // Determine what is connected

  MP3_Volume(250);//in the end turn on volume
  MP3_Track(1);
  UART_TxStr("Start up complete\r\n");

  //
  // Enter the main loop
  //
  CountUp2 = 0;
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


  FlashPhase++;

  if (PrevTime)
    PrevTime--;

  // See if we need to start playback of next track

  if (PlayerStatus == 0) {
    if (++Track > Tracks)
      Track = 1;
    MP3_Track(Track);
  }

  // Process an incoming comms

  CheckForBoardMsg();

  // Check for settings changes
  timer = CountUp2;
  if(timer > 3000)
  {
    if(Input == LeftTablet)
    {
      SwitchToMP3();
      Input = MP3In;
    }
    CountUp2 = 6000;
  }
  else
  {
    if(Input == MP3In)
    {
      Input = LeftTablet;
      SwitchToTablet();
    }
  }
}
