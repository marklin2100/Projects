#ifndef __HW_H__
#define __HW_H__

#include <avr/io.h>
#include <util/delay.h>

/*
 * Board definitions
 */

/* LEDs */
#define LED_DDR	DDRA
#define LED_PORT	PORTA
enum {
	LED1 = PA5,
	LED2 = PA6
};
/* UART */
#define USART_BAUDRATE 9600
/* SPI */
#define DDR_SPI DDRB
#define DD_SS PB4
#define DD_MOSI PB5
#define DD_MISO PB6
#define DD_SCK	PB7
/* SD */
/* pin to select/deselect card */
#define SD_PORT	PORTB
#define SD_SS	PB4

/* pin to detect card in the socket */
#define SD_CD_PIN PIND
#define SD_CD PD4
 
void led_init(void);
void led_on(char led);
void led_off(char led);

void uart_init();
void uart_putc(char c);
void uart_puts(const char *s);
void uart_puthex(uint8_t c);
void uart_puthex16(uint16_t c);
void uart_puthex32(uint32_t c);

void spi_init();
unsigned char spi_send(unsigned char c);

#endif /* __HW_H__ */
