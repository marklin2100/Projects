#ifndef uart_h
#define uart_h
/*****************************************************************************

    UART module

    This module implements UART related software

    David Annett, david@annett.co.nz, 12 July 2008

****************************************************************************/

// ------- Includes ----------

#include "CommonTypes.h"


// ------- Constants ---------

#define PCBaudRate  115200L
#define BaudRate0 (F_CPU/(16*PCBaudRate)-1)

#define IMMaxSize 14

// Map uart_xxxx functions to UART_yyyy functions
#define uart_init       UART_Init
#define uart_putc       UART_TxChar
#define uart_puts       UART_TxStr
#define uart_puthex     UART_TxUint8
#define uart_puthex16   UART_TxUint16
#define uart_puthex32   UART_TxUint32


// ------- Types -------------


// ------- Vars --------------

extern volatile Uint8  TxUnsent;
extern volatile Uint8  RxAvail;
#ifdef  SecondUART
extern volatile Uint8  Tx1Unsent;
extern volatile Uint8  Rx1Avail;
#endif


// ------- Functions ---------

/*
    UART_Init routine

    This routine will init the serial ports ready for use.
*/
void UART_Init(void);


/*
    UART_Tx routine

    This routine will start the transmission of a buffer.
*/
void UART_Tx(Uint8 *TxBuff, Uint8 TxSize);


/*
    UART_Rx routine

    This routine will start the reception to a buffer.
*/
void UART_Rx(Uint8 *RxBuff, Uint8 RxSize);


/*
    UART_TxChar routine

    This routine will send an ASCII character.
*/
void UART_TxChar(char c);


/*
    UART_TxStr routine

    This routine will send a null terminated string.
*/
void UART_TxStr(const char *s);


/*
    UART_TxNewLine routine

    This routine will send a LF CR pair.
*/
void UART_TxNewLine(void);


/*
    UART_TxNum routine

    This routine will send a 32 bit int as a decimal string.
*/
void UART_TxNum(Uint32 c, Uint8 MinWidth);


/*
    UART_TxUint8 routine

    This routine will send a byte as a hex string.
*/
void UART_TxUint8(Uint8 c);


/*
    UART_TxUint16 routine

    This routine will send a 16 bit int as a hex string.
*/
void UART_TxUint16(Uint16 c);


/*
    UART_TxUint32 routine

    This routine will send a 32 bit int as a hex string.
*/
void UART_TxUint32(Uint32 c);

#ifdef  SecondUART
/*
    UART1_Tx routine

    This routine will start the transmission of a buffer on the second serial
    port.
    Unlike UART_Tx this version will return immediately unless it is still
    sending the last transmission.
*/
void UART1_Tx(Uint8 *TxBuff, Uint8 TxSize);


/*
    UART1_Rx routine

    This routine will start the reception to a buffer on the second serial port.
*/
void UART1_Rx(Uint8 *RxBuff, Uint8 RxSize);
#endif

#endif
