#ifndef commontypes_h
#define commontypes_h
/*---------------------------------------------------------------------------

  CommonTypes module

  This module contains size defined types commonly used.

  Created by David Annett 24 August 2008

  Copyright Reel Skill Games 2008
---------------------------------------------------------------------------*/

// Defines that control includes


// Includes

#include <stdbool.h>


// Type defintions

typedef unsigned char       byte;
typedef unsigned char       Uchar;
typedef unsigned char       Byte;
typedef unsigned int        Word;
typedef char                Int8;
typedef int                 Int16;
typedef long int            Int32;
typedef unsigned char       Uint8;
typedef unsigned int        Uint16;
typedef unsigned long int   Uint32;
typedef char                int8;
typedef int                 int16;
typedef long int            int32;
typedef unsigned char       uint8;
typedef unsigned int        uint16;
typedef unsigned long int   uint32;


// Usefull macros

#define SetBits(port,mask)   ((port) |= (mask))
#define ClearBits(port,mask) ((port) &= (~(mask)))
#define SetBit(port,bit)     ((port) |= (1<<bit))
#define ClearBit(port,bit)   ((port) &= (~(1<<bit)))
#define GetBit(port,bit)     ((port) & (1<<bit))


#endif
