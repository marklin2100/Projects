#ifndef boardcomms_h
#define boardcomms_h
/*****************************************************************************

    BoardComms module

    This module implements communications between boards via RS485.
    The comms is half duplex 38400 8N1.

    The packet format is binary:
    0 - 00000001      SOH, Start character
    1 - SSSSDDDD      Source and destination nibbles
    2 - Type          Message type
    3 - Parameter 1   First parameter, if needed
    4 - Parameter 2   Second parameter, if needed
    5 - Checkum       Simple sum of preceding bytes

    David Annett, david@lynxinnovation.com, 11 December 2012

    (c) 2012 Lynx Innovation

****************************************************************************/

//========================== Includes =========================================

#include "CommonTypes.h"
#include "Globals.h"

//========================== Constants ========================================

#define BCBaudRate  38400L
#define BaudRate1 (F_CPU/(16*BCBaudRate)-1)

#define BCMsgSize   6
#define SOH         0x01

#define CommsRetries  5

// Offset of different fields in the message packet

enum TBCP {
  BCPSOH,     // SOH (0x01) start of message marker
  BCPAddr,    // Addresses, high nibble source, low nibble destination
  BCPType,    // Type of message
  BCPParam1,  // Parameter 1
  BCPParam2,  // Parameter 2
  BCPChecksum // Checksum
};

// Addresses for different board types

enum TBCA {
  BCAMP3Contoller,      // 0 031-517-202
  BCAKeypadController,  // 1 031-517-201
  BDCLCD,               // 2 LCD panel with media player
  BCATabletLeft,        // 3 Tablet on left side of fixture
  BCATabletRight,       // 4 Tablet on right side of fixture
  BCALeftBay,           // 5 Left bay
  BCACenterBay,         // 6 Center bay
  BCARightBay           // 7 RIght bay
};
#define BCAOutput BCALeftBay

// Message types

enum TBCT {
  BCTAck,             //  0 Acknowledge, has no parameters.
  BCTNAck,            //  1 Negative acknowledge, first parameter is reason code, optional second parameter is reason details.
  BCTInquire,         //  2 Inquire about board status, has no parameters.
  BCTInquireAnswer,   //  3 Answer to BCTInquire message, first parameter is type ID (TProducts), second parameter is TI2SFormat is source device.
  BCTVolume,          //  4 Set the volume, first parameter is the volume, no second parameter.
  BCTBrightness,      //  5 Set the PWM brightness level of LED Strip or lamp, first parameter is the channel/lamp, the second is the brightness.
  BCTLamps,           //  6 Set all lamps.  First parameter is lamps 1 to 8, the second lamps 9 to 16 if present.
  BCTKey,             //  7 Key press event.  First parameter is key number, or zero if no key.
  BCTAudioFormat,     //  8 Audio data format.  First parameter TI2SFormat.
  BCTSlaveMode,       //  9 Command to put controller in slave mode so tablet can take control.  First parameter 0 = normal mode, 0001llrr set sources.
  BCTPlayTrack,       // 10 Play track.  First parameter is the track number.
  BCTLampBrightness,  // 11 Bits 7 - 4 = lamp (0b1111 = default), bit 3 = 1 is on brightness, 0 is off brightness, bits 2 - 0 is brightness level.
  BCTHeadphoneChGain, // 12 Set the differential gain of a headphone channel.  First parameter is channel number 1 to 5, second parameter is a signed value in dB.
  BCTHeadphoneChMax   // 13 Set the maximum volume of a headphone channel.  First parameter is channel number 1 to 5, second parameter is the maximum volume.
};

// NAck reasons

enum TBCN {
  BCNUnkownType,      // 0 Received message type is unknown
  BCNBadParam1,       // 1 The value of the first parameter is bad
  BCNBadParam2        // 2 The value of the second parameter is bad
};

enum TBay {
  LeftBay,            // 0 Left bay
  CenterBay,          // 1 Center bay
  RightBay,           // 2 Right bay
  NoBay               // 3 No bay or unknown bay
};

enum TProducts {
  FiveHeadphones,     // 0 Five headphones
  Pill,               // 1 Pill speaker
  Beatbox,            // 2 Beat Box speaker
  ReservedProduct1,   // 3 Reserved for future products
  ReservedProduct2,   // 4 Reserved for future products
  ReservedProduct3,   // 5 Reserved for future products
  ReservedProduct4,   // 6 Reserved for future products
  ReservedProduct5,   // 7 Reserved for future products
  MediaPLayer,        // 8 A media player source
  UnknownProduct      // 9 An unknown product
};

enum TI2SFormat {
  I2S16Bit,   // 0 As used by LCD media player
  I2S32Bit    // 1 As used by VS1053 in MP3 player
};

//========================== Types ============================================


//========================== Vars =============================================

extern volatile bool BCRXAvail;  // A complete valid message has been received, cleared when ReceiveBCMessage called.
extern volatile bool BCTXBusy;

extern volatile Uint8   BCRXPos;    // Position in RXBuf


//========================== Functions =======================================

/*
  BCMessageInit routine

  This routine prepares for board to board communications.  It is passed the
  address to use for sending
 */
void BCMessageInit(Uint8 OurAddress);


/*
  BCMessageSend routine

  This routine will send a board comms message.  If busy it will wait before
  sending.  It will fill in the header and checksum.  The user callers should
  set up address, type and parameters.
  If WaitForSend is true it will no return until message sent.
*/
void BCMessageSend(Uint8 *TxMsg, bool WaitForSend);


/*
  BCMessageReceive routine

  This routine will allow the reception a board comms message.
*/
void BCMessageReceive(Uint8 *RxMsg);

#endif
