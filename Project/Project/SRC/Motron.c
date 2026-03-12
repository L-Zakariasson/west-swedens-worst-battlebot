/*
 * Motron.c
 *
 * Created: 2026-03-12 10:15:12
 *  Author: ludvig
 */ 
#include "include.h"

uint8_t getCRC(uint8_t * message, uint8_t length) /*använder inte denna men e bra att ha om jag vill aktivera CRC. Cyclic redundancy check need at end of each message according to https://www.pololu.com/docs/0J84/all #3.7*/
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
    motron_send(max_accel_m1_forward, sizeof max_accel_m1_forward);                   /*max accel motor 1*/
    uint8_t max_accel_m1_reverse[] = {0x9c,motor1,0x0c,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m1_reverse, sizeof max_accel_m1_reverse);                   /*max accel motor 1*/
    uint8_t max_decel_m1_forward[] = {0x9c,motor1,0x0e,(accel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m1_forward, sizeof max_decel_m1_forward);                   /*max decel motor 1*/
    uint8_t max_decel_m1_reverse[] = {0x9c,motor1,0x10,(decel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m1_reverse, sizeof max_decel_m1_reverse);                   /*max decel motor 1*/
}

void max_accl_decel_m2(uint16_t accel, uint16_t decel){
    uint8_t max_accel_m2_forward[] = {0x9c,motor2,0x0a,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m2_forward, sizeof max_accel_m2_forward);                   /*max accel motor 2*/
    uint8_t max_accel_m2_reverse[] = {0x9c,motor2,0x0c,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m2_reverse, sizeof max_accel_m2_reverse);                   /*max accel motor 2*/
    uint8_t max_decel_m2_forward[] = {0x9c,motor2,0x0e,(accel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m2_forward, sizeof max_decel_m2_forward);                   /*max decel motor 2*/
    uint8_t max_decel_m2_reverse[] = {0x9c,motor2,0x10,(decel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m2_reverse, sizeof max_decel_m2_reverse);                   /*max decel motor 2*/
}

void max_accl_decel_m3(uint16_t accel, uint16_t decel){
    uint8_t max_accel_m3_forward[] = {0x9c,motor3,0x0a,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m3_forward, sizeof max_accel_m3_forward);                   /*max accel motor 3*/
    uint8_t max_accel_m3_reverse[] = {0x9c,motor3,0x0c,(accel & 0x7f),((accel>>7)& 0x7f)};
    motron_send(max_accel_m3_reverse, sizeof max_accel_m3_reverse);                   /*max accel motor 3*/
    uint8_t max_decel_m3_forward[] = {0x9c,motor3,0x0e,(accel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m3_forward, sizeof max_decel_m3_forward);                   /*max decel motor 2*/
    uint8_t max_decel_m3_reverse[] = {0x9c,motor3,0x10,(decel & 0x7f),((decel>>7)& 0x7f)};
    motron_send(max_decel_m3_reverse, sizeof max_decel_m3_reverse);                   /*max decel motor 2*/
}

void motron_init(void){
    motron_reinitialize();                                                    /*mc.reinitialize();*/
    
    uint8_t dissable_crc[] = {0x8b,0x04,0x7b,0x43};
    motron_send(dissable_crc,sizeof dissable_crc);                            /*dissables crc, inte jättesmart men gör det enklare*/
    
    uint8_t clear_reset_flags[3] = {0xa9,0x00,0x04};
    motron_send(clear_reset_flags, sizeof clear_reset_flags);                 /*mc.clearResetFlag();*/
    
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