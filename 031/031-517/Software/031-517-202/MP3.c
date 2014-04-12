/*****************************************************************************

    MP3 module

    This module implements MP3 related software.

    The MP3 chip is a VS1003 or VS1053.

    See http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.RMP-MP3

    David Annett, david@lynxinnovation.com, 18 Jan 2012

    (c) 2012 Lynx Innovation

****************************************************************************/

//========================== Includes ========================================

#include "CommonTypes.h"
#include "Globals.h"
#include "MP3.h"
#include "UART.h"
#include "SPI.h"
#include "sd.h"
#include "vs.h"
#include "diet.h"
#include "Timer.h"


//========================== Constants =======================================



//========================== Types ===========================================



//========================== Public vars =====================================

bool MP3Ready      = 0;
bool MP3Paused     = 0;


//========================== Private vars ====================================

diet_file_t f;

static int playing = 0;


//========================== Private functions ===============================



//========================== Functions =======================================

/*
  MP3_Init routine

  This routine will prepare the MP3 player hardware for use.
*/
bool MP3_Init(void)
{
	int r;


  UART_TxStr("MP3_Init, Initializing SPI\r\n");
  wdt_reset();
  SPI_Init();
  SPI_Send(0xff);   // AVR should send dummy byte to initialise SPI

  MP3ResetAssert;   // Pulse the VS1053 chip reset line
  DelayMS(500);
  MP3ResetNegate;

  UART_TxStr("Initializing SD card module\r\n");
  if (sd_init() != 0) {
    UART_TxStr("Failed to initialize SD card\r\n");
  }

  UART_TxStr("Initialize sound chip VS1053\r\n");
  wdt_reset();
  vs_init();
#ifdef I2S
  UART_TxStr("Enabling I2S\r\n");
  wdt_reset();
  vs_set_i2s();
#endif
  wdt_reset();
  UART_TxUint16(vs_test_memory());
  UART_TxStr("\r\nMP3 memory test done\r\n");

#ifdef VS10XX_FLAC
  DelayMS(100);
  UART_TxStr("Loading FLAC support\r\n");
  wdt_reset();
  vs_load_plugin_flac();
#endif

// Enable streaming and SDI_NEW
  DelayMS(10);
  UART_TxStr("Using SDI_NEW streaming\r\n");
  wdt_reset();
  vs_sci_write(0x00, 0x0820);

// Set volume

  UART_TxStr("Set audio volume\r\n");
  vs_sci_write(0x0b, 0x0000);

// Set SCI_CLOCK
  UART_TxStr("Set audio clock\r\n");
//  vs_sci_write(0x03, 0x6800);
  vs_sci_write(0x03, 0x8800);


  UART_TxStr("Set high speed for SPI bus\r\n");
  SPI_Set_High_Speed();

  wdt_reset();
  r = diet_init();
  if (r != 0) {
    UART_TxStr("Failed to initialize FAT driver: ");
    UART_TxUint16(r);
    UART_TxNewLine();
    return false;
  }

/*
  Uint16 TempInt;
  char   buf[512];
//  vs_test_sine_on();
  Timer_Clear();
  for (TempInt = 0; TempInt < 4000; TempInt++)
    sd_read_block(0, buf);
  UART_TxStr("Read time = ");
  UART_TxNum(Timer_Read());
  UART_TxStr("mS\r\n");
//  vs_test_sine_off();
*/

#ifdef I2S
  UART_TxStr("Enabling I2S\r\n");
  wdt_reset();
  vs_set_i2s();
#endif
  MP3Ready = true;
  return true;
}



/*
  MP3_OpenFile routine

  This routine will open the file of the requested track.  The track number
  should be in the range 1 to 99.  It can be used for testing tracks exist.
*/
bool MP3_OpenFile(Uint8 Track)
{
  char Filename[7];


  if (!MP3Ready)
    return false;

  Filename[0] = '0' + (Track / 10) % 10;
  Filename[1] = '0' + Track % 10;
  Filename[2] = '.';
  Filename[6] = 0;
#ifdef VS10XX_FLAC
  Filename[3] = 'F';
  Filename[4] = 'L';
  Filename[5] = 'A';
  if (diet_open(Filename, &f) == -1) {
#endif
    Filename[3] = 'M';
    Filename[4] = 'P';
    Filename[5] = '3';
    if (diet_open(Filename, &f) == -1) {
      Filename[3] = 'W';
      Filename[4] = 'A';
      Filename[5] = 'V';
      if (diet_open(Filename, &f) == -1) {
        UART_TxStr("Can't open ");
        UART_TxNum(Track, 2);
        UART_TxNewLine();
        return false;
      }
    }
#ifdef VS10XX_FLAC
  }
#endif
  UART_TxStr(Filename);
  UART_TxNewLine();
  return true;
}



/*
  MP3_Track routine

  This routine will start playback of the requested track.  The track number
  should be in the range 1 to 99.
*/
bool MP3_Track(Uint8 Track)
{
  if (!MP3Ready)
    return false;

  if (playing) {
    UART_TxStr("Cancel previous track\r\n");
    vs_cancel();
    playing = 0;
  }

  UART_TxStr("MP3_Track ");
  if (MP3_OpenFile(Track)) {
    playing = 1;
    MP3_Process();
    return true;
  }else {
    return false;
  }
}



/*
  MP3_Volume routine

  This routine will set the output volume of the playback.
*/
void MP3_Volume(Uint8 Volume)
{
  Uint8 NewVolume;


  if (!MP3Ready)
    return;

  UART_TxStr("MP3_Volume ");
  UART_TxNum(Volume, 3);
  UART_TxNewLine();
  NewVolume = ~Volume;
  vs_set_volume(NewVolume, NewVolume);
}



/*
  MP3_Process routine

  This routine must be called regularly to keep the MP3 chip feed with data
  or else playback will stop.  It will return true if still not end of track.
*/
bool MP3_Process(void)
{
  uint8_t *buf;
  int n;
  int ret;



  if (!MP3Ready)
    return false;

  if (MP3Paused)
    return true;

  if (!sdok) {          // Check for SD card failure
    MP3Ready = false;
    return false;
  }

  SPI_Set_Super_Speed();

  n = diet_read(&f, &buf);

  SPI_Set_High_Speed();

  if (f.eof) {
    if(playing)
      UART_TxStr("End of track\r\n");
    ret = vs_finish();
    if (ret) {
      UART_TxStr("vs_finished returned: ");
      UART_TxNum(ret, 1);
      UART_TxNewLine();
      vs_reset_soft();
    }
    playing = 0;
    return false;
  }

  if(n == 0)
  {
    return false;//current data error
  }

  vs_sdi_write(buf, n);

  return true;
}



/*
  MP3_Pause routine

  This routine will pause playback if argument is true, otherwise resumes
  playback.
*/
void MP3_Pause(bool Pause)
{
  MP3Paused = Pause;
}
