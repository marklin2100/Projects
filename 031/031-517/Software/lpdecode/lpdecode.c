/*
  lpdecode program

  This program is a "Lynx Packet Decoder" for the comms used on the 
  Lynx Innovation 031-517 Beats headphone and speaker project.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517

  Typical usage:
    stty -F /dev/ttyUSB11 speed 38400
    cat /dev/ttyUSB11 | lpdecode

  Build:
    gcc -o lpdecode lpdecode.c

  Created by David Annett 1 Febuary 2013

  Copyright Lynx Innovation 2013
*/

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "BoardComms.h"


/* Return 1 if the difference is negative, otherwise 0.  */
int TimevalSubtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}



/*
  DecodeFrame routine

  This routine attempts to decode a frame and show it's contents.
*/
void DecodeFrame(Uint8 *Frame)
{
  Uint8 Pos;
  Uint8 Checksum;
  Uint8 TempByte;


  printf(" [");

  // Check the checksum is ok

  Checksum = 0;
  for (Pos = 0; Pos < BCPChecksum; Pos++)
    Checksum += Frame[Pos];

  if (Checksum != Frame[BCPChecksum])
    printf(" Bad checksum %02X <> %02X", Frame[BCPChecksum], Checksum);

  // Decode addresses

  printf(" %01X->%01X", Frame[BCPAddr] >> 4, Frame[BCPAddr] & 0b00001111); 

  // Decode type

  switch (Frame[BCPType]) {
    case BCTAck:
      printf(" Ack");
      break;

    case BCTNAck:
      printf(" NAck ");
      switch (Frame[BCPParam1]) {
        case BCNUnkownType:
          printf(", message type %d is unknown", Frame[BCPParam2]);
          break;

        case BCNBadParam1:
          printf(", value of the first parameter is bad");
          break;

        case BCNBadParam2:
          printf(", value of the second parameter is bad");
          break;
      }
      break;

    case BCTInquire:
      printf(" Inquire");
      break;

    case BCTInquireAnswer:
      printf(" Inquire answer, product %d", Frame[BCPParam1]);
      break;

    case BCTVolume:
      printf(" Volume %d", Frame[BCPParam1]);
      break;

    case BCTBrightness:
      printf(" Brightness, channel %d set to %d", Frame[BCPParam1], Frame[BCPParam2]);
      break;

    case BCTLamps:
      printf(" Lamps %02X%02X", Frame[BCPParam1], Frame[BCPParam2]);
      break;

    case BCTKey:
      if (Frame[BCPParam1])
        printf(" Key %d pressed", Frame[BCPParam1]);
      else
        printf(" Key released");
      break;

    case BCTAudioFormat:
      switch (Frame[BCPParam1]) {
        case I2S16Bit:
          printf(" Audio format, 16bit");
          break;

        case I2S32Bit:
          printf(" Audio format, 32bit");
          break;
      }
      break;

    case BCTSlaveMode:
      printf(" Slave mode %02X", Frame[BCPParam1]);
      break;

    case BCTPlayTrack:
      printf(" Lamps %02X%02X", Frame[BCPParam1], Frame[BCPParam2]);
      break;

    case BCTLampBrightness:
      TempByte = Frame[BCPParam1] >> 4;
      if (TempByte == 0b1111)
        printf(" All lamps ", TempByte);
      else
        printf(" Lamp %d ", TempByte);
      if (Frame[BCPParam1] & 0b00001000)
        printf("on");
      else
        printf("off");
      printf(" brightness %d", Frame[BCPParam1] & 0b00000111);
      break;

    case BCTHeadphoneChGain:
      printf(" Headphone gain ch %d to %d dB", Frame[BCPParam1], Frame[BCPParam2]);
      break;

    case BCTHeadphoneChMax:
      printf(" Headphone max vol ch %d to %d", Frame[BCPParam1], Frame[BCPParam2]);
      break;
  }
  printf("]");
}



/*
  main routine

  Main program entry point.
*/
int main(void)
{
  Uint8           RxByte;
  struct timeval  CurrentTime;
  struct timeval  LastTime;
  struct timeval  LapsedTime;
  Uint8           Pos;
  Uint8           Frame[6];


  printf("Lynx Packet Decoder (%s)\n", __DATE__);
  gettimeofday(&LastTime, NULL);
  Pos = 0;

  // Loop reading characters

  while (fread(&RxByte,1,1,stdin)) {  // Wait for next character

    // Find out how much time has lapsed and show it

    gettimeofday(&CurrentTime, NULL);
    TimevalSubtract(&LapsedTime, &CurrentTime, &LastTime);

    // See if interframe time

    if ((LapsedTime.tv_sec) || (LapsedTime.tv_usec > 1000)) {
      if (Pos >= 6)
        DecodeFrame(Frame);
      printf("\n%03ld.%06ld", LapsedTime.tv_sec, LapsedTime.tv_usec);
      Pos = 0;
    }

		// Show the byte

    printf(" %02X", RxByte);

    // Save in a frame

    if (Pos < 6)
      Frame[Pos] = RxByte;

    if ((Pos) || (RxByte == 1))
      Pos++;

    // Set up ready for the next byte

    LastTime.tv_sec = CurrentTime.tv_sec;
    LastTime.tv_usec = CurrentTime.tv_usec;
  }
}
