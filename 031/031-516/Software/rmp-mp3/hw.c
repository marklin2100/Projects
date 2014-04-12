#include "hw.h"

/*
 * LEDs
 */
void led_init(void) {
	LED_DDR |= (1 << LED1) | (1 << LED2);
	LED_PORT &= ~((1 << LED1)| (1 << LED2));
}

void led_on(char led) {
	LED_PORT |= (1 << led);
}

void led_off(char led) {
	LED_PORT &= ~(1 << led);
}

void led_toggle(char led) {
	LED_PORT ^= (1 << led);
}

/*
 * UART
 */
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void uart_init() {
	UBRR0L = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8); 

	UCSR0B = ((1<<TXEN0)|(1<<RXEN0));
	//UCSR0C = ((1<<UCSZ01)|(1<<UCSZ00));
}

void uart_putc(char c) {
		while((UCSR0A &(1<<UDRE0)) == 0);
		UDR0 = c;
}

void uart_puts(const char *s) {
	for (; *s; s++) {
		uart_putc(*s);
	}
}

void uart_puthex(uint8_t c) {
	static char hex[16] = "0123456789abcdef";
	uart_putc(hex[c >> 4]);
	uart_putc(hex[c & 0xf]);
}

void uart_puthex16(uint16_t c) {
	uart_puthex(c >> 8);
	uart_puthex(c & 0xff);
}

void uart_puthex32(uint32_t c) {
	uart_puthex(c >> 24);
	uart_puthex(c >> 16);
	uart_puthex(c >> 8);
	uart_puthex(c & 0xff);
}

/*
 * SPI
 */
void spi_init() {
	DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
	DDR_SPI &= ~(1 << DD_MISO);

	SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1) | (1 << SPI2X);
	SPSR = 0;
}

void spi_set_high_speed() {
	SPCR &= ~((1 << SPR0) | (1 << SPR1));
	SPCR |= (1 << SPI2X);
}

unsigned char spi_send(unsigned char c) {
	SPDR = c;
	while (!(SPSR & (1 << SPIF)));
	c = SPDR;
	return c;
}

