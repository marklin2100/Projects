/*****************************************************************************

    SPI module

    This module implements SPI related software

  Created by David Annett 31 January 2012

  Copyright Lynx Innovation 2012

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"
#include "Globals.h"

// ------- Constants ---------


// ------- Types -------------


// ------- Vars --------------


// ------- Functions ---------



/*
  SPI_Init routine

  This routine prepares the SPI bus for use.
  //speed = xtal / 64
*/
void SPI_Init() {
  DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK);
  DDR_SPI &= ~(1 << DD_MISO);

  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (1 << SPR1);
//  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
  SPSR = 0;
}


//speed = xtal / 16
void SPI_Set_Middle_Speed() {
  SPCR &= ~((0 << SPR0) | (1 << SPR1));
  SPSR &= ~(1 << SPI2X);
}
/*
  SPI_Set_High_Speed routine

  This routine puts the SPI bus in high speed mode.
 */
void SPI_Set_High_Speed() {
  SPCR &= ~((1 << SPR0) | (1 << SPR1));
  SPSR &= ~(1 << SPI2X);
}

void SPI_Set_Super_Speed() {
  SPCR &= ~((1 << SPR0) | (1 << SPR1));
  SPSR |= (1 << SPI2X);
}


/*
  SPI_Send routine

  This routine sends a byte over the SPI buss.
*/
Uint8 SPI_Send(Uint8 c) {
  SPDR = c;
  while (!(SPSR & (1 << SPIF)));
  c = SPDR;
  return c;
}
