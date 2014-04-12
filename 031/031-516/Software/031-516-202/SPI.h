#ifndef spi_h
#define spi_h
/*****************************************************************************

    SPI module

    This module implements SPI related software

  Created by David Annett 31 January 2012

  Copyright Lynx Innovation 2012

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"


// ------- Constants ---------

// Re-map spi_xxxx to SPI_yyyy functions

#define spi_init            SPI_Init
#define spi_set_high_speed  SPI_Set_High_Speed
#define spi_send            SPI_Send

// ------- Types -------------


// ------- Vars --------------


// ------- Functions ---------

/*
  SPI_Init routine

  This routine prepares the SPI bus for use.
*/
void SPI_Init();

/*
  SPI_Set_High_Speed routine

  This routine puts the SPI bus in high speed mode.
*/
void SPI_Set_Middle_Speed();
void SPI_Set_High_Speed();

void SPI_Set_Super_Speed();

/*
  SPI_Send routine

  This routine sends a byte over the SPI buss.
*/
Uint8 SPI_Send(Uint8 c);

#endif
