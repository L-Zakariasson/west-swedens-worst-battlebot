/*
 * Include.h
 *
 * Created: 2026-03-12 10:11:40
 *  Author: ludvi
 */ 


#ifndef INCLUDE_H_
#define INCLUDE_H_

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

uint8_t getCRC(uint8_t * message, uint8_t length);
void motron_send(uint8_t * cmd, uint8_t size);
void motron_reinitialize(void);
void max_accl_decel_m1(uint16_t accel, uint16_t decel);
void max_accl_decel_m2(uint16_t accel, uint16_t decel);
void max_accl_decel_m3(uint16_t accel, uint16_t decel);
void motron_init(void);
void set_motor_all_speeds(int16_t motor1_speed, int16_t motor2_speed, int16_t motor3_speed);
void set_motor_1_speed(int16_t motor1_speed);
void set_motor_2_speed(int16_t motor2_speed);
void set_motor_3_speed(int16_t motor3_speed);

void i2c_start(void);
void i2c_stop (void);
void i2c_init (void);
void i2c_write (uint8_t data);
void uart_init(uint8_t baud);



#endif /* INCLUDE_H_ */