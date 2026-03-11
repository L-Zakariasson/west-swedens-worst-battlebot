/*
 * Project.c
 *
 * Created: 2026-02-10 15:39:32
 * Author : ludvig
 */ 

#define motron_addr 0b00100000   /*16 << 1 addr till motron är 16 men då sista biten i i2c är för acknowledge så behöver denna sjkutas in en för att adressen ska bli rätt*/
#define motor1 0x01
#define motor2 0x02
#define motor3 0x03 
#define command_set_speed 0xd1
#define F_CPU 16000000

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>



volatile uint8_t rx_input = 0;
volatile bool msg_in = false;
volatile bool motor3_toggle = false;

uint8_t getCRC(uint8_t * message, uint8_t length) /*använder inte denna men e bra att ha om jag vill aktivera CRC. Cyclic redundancy check need at end of each message according to https://www.pololu.com/docs/0J84/all#3.7*/
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= message[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1) { crc ^= 0x91; }
            crc >>= 1;
        }
    }
    return crc;
}

void i2c_start(void){
    TWCR = (1 << TWINT | (1<< TWSTA) | (1 << TWEN));
    while((TWCR & (1<<TWINT)) == 0);
}

void i2c_stop (void){
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void i2c_init (void){
    TWCR = 0x00;
    TWBR = 0x08;            /*clock speed, under 400kHz*/
    TWCR = 0x04;
}

void i2c_write (uint8_t data){
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
    //_delay_us(56);
}

void motron_send(uint8_t * cmd, uint8_t size){
    i2c_start();
    i2c_write(motron_addr);
    for (uint8_t i = 0; i<size; i++) {
        i2c_write(cmd[i]);
    }
    i2c_stop();
}

void motron_reinitialize(void){
    i2c_start();
    i2c_write(motron_addr);
    i2c_write(0x96);
    i2c_write(0x74);
    i2c_stop();
}

void max_accl_decel_m1(uint16_t accel, uint16_t decel){
    uint8_t max_accel_m1_forward[] = {0x9c,motor1,0x0a,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m1_forward, sizeof max_accel_m1_forward);                   /*max accel motor 1 140*/   
    uint8_t max_accel_m1_reverse[] = {0x9c,motor1,0x0c,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m1_reverse, sizeof max_accel_m1_reverse);                   /*max accel motor 1 140*/
    uint8_t max_decel_m1_forward[] = {0x9c,motor1,0x0e,(accel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m1_forward, sizeof max_decel_m1_forward);                   /*max decel motor 1 300*/    
    uint8_t max_decel_m1_reverse[] = {0x9c,motor1,0x10,(decel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m1_reverse, sizeof max_decel_m1_reverse);                   /*max decel motor 1 300*/
}

void max_accl_decel_m2(uint16_t accel, uint16_t decel){
    uint8_t max_accel_m2_forward[] = {0x9c,motor2,0x0a,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m2_forward, sizeof max_accel_m2_forward);                   /*max accel motor 2 140*/   
    uint8_t max_accel_m2_reverse[] = {0x9c,motor2,0x0c,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m2_reverse, sizeof max_accel_m2_reverse);                   /*max accel motor 2 140*/
    uint8_t max_decel_m2_forward[] = {0x9c,motor2,0x0e,(accel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m2_forward, sizeof max_decel_m2_forward);                   /*max decel motor 2 300*/    
    uint8_t max_decel_m2_reverse[] = {0x9c,motor2,0x10,(decel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m2_reverse, sizeof max_decel_m2_reverse);                   /*max decel motor 2 300*/
}

void max_accl_decel_m3(uint16_t accel, uint16_t decel){
    uint8_t max_accel_m3_forward[] = {0x9c,motor3,0x0a,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m3_forward, sizeof max_accel_m3_forward);                   /*max accel motor 3 140*/
    uint8_t max_accel_m3_reverse[] = {0x9c,motor3,0x0c,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m3_reverse, sizeof max_accel_m3_reverse);                   /*max accel motor 3 140*/
    uint8_t max_decel_m3_forward[] = {0x9c,motor3,0x0e,(accel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m3_forward, sizeof max_decel_m3_forward);                   /*max decel motor 2 300*/
    uint8_t max_decel_m3_reverse[] = {0x9c,motor3,0x10,(decel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m3_reverse, sizeof max_decel_m3_reverse);                   /*max decel motor 2 300*/
}

void motron_init(void){
    motron_reinitialize();                                                    /*mc.reinitialize();*/
    
    uint8_t dissable_crc[] = {0x8b,0x04,0x7b,0x43};
    motron_send(dissable_crc,sizeof dissable_crc);                            /*dissables crc, inte jättesmart men gör det enklare*/     
    
    uint8_t clear_reset_flags[3] = {0xa9,0x00,0x04};
    motron_send(clear_reset_flags, sizeof clear_reset_flags);                 /*mc.clearResetFlag(); in arduino*/
    
    max_accl_decel_m1(800,800);
    max_accl_decel_m2(800,800);
    max_accl_decel_m3(800,800);
}

void set_motor_all_speeds(int16_t motor1_speed, int16_t motor2_speed, int16_t motor3_speed){
    
    uint8_t motor1speed[] = {command_set_speed,motor1,(motor1_speed & 0x7f),((motor1_speed>>7)&0x7f)};
    motron_send(motor1speed, sizeof motor1speed);
    
    uint8_t motor2speed[] = {command_set_speed,motor2,(motor2_speed & 0x7f),((motor2_speed>>7)&0x7f)};
    motron_send(motor2speed, sizeof motor2speed);
    
    uint8_t motor3speed[] = {command_set_speed,motor3,(motor3_speed & 0x7f),((motor3_speed>>7)&0x7f)};
    motron_send(motor3speed, sizeof motor3speed);
};

void set_motor_1_speed(int16_t motor1_speed){
    uint8_t motor1speed[] = {command_set_speed,motor1,(motor1_speed & 0x7f),((motor1_speed>>7)&0x7f)};
    motron_send(motor1speed, sizeof motor1speed);
}

void set_motor_2_speed(int16_t motor2_speed){
    uint8_t motor2speed[] = {command_set_speed,motor2,(motor2_speed & 0x7f),((motor2_speed>>7)&0x7f)};
    motron_send(motor2speed, sizeof motor2speed);    
}

void set_motor_3_speed(int16_t motor3_speed){
    uint8_t motor3speed[] = {command_set_speed,motor3,(motor3_speed & 0x7f),((motor3_speed>>7)&0x7f)};
    motron_send(motor3speed, sizeof motor3speed);
}

void uart_init(uint8_t baud){
    UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
    UBRR0L = baud;    
}

ISR (USART_RX_vect){
    rx_input = UDR0;
    UDR0 = rx_input;
    msg_in = true;    
}
    

int main(void)
{
     
    i2c_init(); /*setesup i2c comication in atmega*/
    motron_init(); /*setsup motorcontroller with default settings*/
    uart_init(104);
    sei();
    
    while (1) {   

        if (msg_in){
            
            if (0x4B == rx_input || 0x6B == rx_input) {
                motor3_toggle = !motor3_toggle;
            }
            
            if (0x57 == rx_input || 0x77 == rx_input) {
                set_motor_1_speed(800);
                set_motor_2_speed(800);
            } else if (0x53 == rx_input || 0x73 == rx_input) {
                set_motor_1_speed(-800);
                set_motor_2_speed(-800);
            } else if (0x41 == rx_input || 0x61 == rx_input) {
                set_motor_1_speed(800);
                set_motor_2_speed(-800);
            } else if (0x44 == rx_input || 0x64 == rx_input) {
                set_motor_1_speed(-800);
                set_motor_2_speed(800);
            }
            
            _delay_ms(250);
            //set_motor_all_speeds(0,0,0);
            set_motor_1_speed(0);
            set_motor_2_speed(0);

            msg_in = false;
        } 
        
        if (!motor3_toggle) {
            set_motor_3_speed(0);
        } else {
            set_motor_3_speed(800);
        }
              
    }
}

