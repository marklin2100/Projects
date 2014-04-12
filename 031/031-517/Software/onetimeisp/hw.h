#ifndef __HW_H__
#define __HW_H__

#include <avr/io.h>

#define RED       0
#define GREEN     1
static volatile   uint8_t ledstatus = RED;
/*
 * LED: green and red
 */
static void led_off() {
  PORTB &= ~((1 << PB3) | (1 << PB4));
}

static void led_red() {
  led_off();
  PORTB |= (1 << PB3);
}

static void led_green() {
  led_off();
  PORTB |= (1 << PB4);
}

static void led_init() {
  DDRB |= (1 << PB3) | (1 << PB4);
  led_off();
}

static void led_blink(){
  if(ledstatus == RED){
    led_green();
    ledstatus = GREEN;
  }else if(ledstatus == GREEN){
    led_red();
    ledstatus = RED;
  }

}

/*
 * UART
 */
#ifndef NDEBUG
#define USART_BAUDRATE 4800
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

static void uart_init() {
  UBRR0L = BAUD_PRESCALE;
  UBRR0H = (BAUD_PRESCALE >> 8);
  UCSR0B = (1<<TXEN0);
}

static void uart_putc(char c) {
    while((UCSR0A &(1<<UDRE0)) == 0);
    UDR0 = c;
}

static void uart_puts(const char *s) {
  for (; *s; s++) {
    uart_putc(*s);
  }
}

static void uart_puthex(uint8_t c) {
  static char hex[16] = "0123456789abcdef";
  uart_putc(hex[c >> 4]);
  uart_putc(hex[c & 0xf]);
}

static void uart_putnum(const char *s, uint32_t n) {
  uart_puts(s);
  uart_puts("0x");
  if (n >> 24) uart_puthex(n >> 24);
  if (n >> 16) uart_puthex(n >> 16);
  if (n >> 8) uart_puthex(n >> 8);
  uart_puthex(n);
  uart_puts("\r\n");
}

#else
static void uart_init() {}
static void uart_putc(char c) {}
static void uart_puts(const char *s) {}
static void uart_puthex(uint8_t c) {}
static void uart_putnum(const char *s, uint32_t n) {}
#endif

#endif /* __HW_H__ */
