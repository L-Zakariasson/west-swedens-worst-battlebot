/*
 * Drivers.c
 *
 * Created: 2026-03-12 10:17:34
 *  Author: ludvig
 */ 

#include "include.h"

void i2c_init (void){
    TWCR = 0x00;
    TWBR = 0x08;            /*clock speed, under 400kHz*/
    TWCR = 0x04;
}

void i2c_start(void){
    TWCR = (1 << TWINT | (1<< TWSTA) | (1 << TWEN));
    while((TWCR & (1<<TWINT)) == 0);
}

void i2c_write (uint8_t data){
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
    //_delay_us(56);
}

void i2c_stop (void){
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void uart_init(uint8_t baud){
    UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
    UBRR0L = baud;
}