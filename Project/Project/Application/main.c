/*
 * Project.c
 *
 * Created: 2026-02-10 15:39:32
 * Author : ludvig
 */ 

// sänk max accel o decel på vapen
// ordna power supply lösning för motorer?
// pyton för ps5 kontroller
 
#include "include.h"


volatile uint8_t rx_input = 0;
volatile bool msg_in = false;
volatile uint8_t uart_command[3] = {0,0,0};
volatile uint8_t uart_command_counter = 0;


ISR (USART_RX_vect){ /*interrupt to get serial data*/
    rx_input = UDR0;
//     UDR0 = rx_input;
//     msg_in = true;
    if (uart_command_counter < 3) {
        if (61==rx_input || 62==rx_input) {
            uart_command[0] = rx_input;
        } else if (0==(rx_input>>6) || 1==(rx_input>>6)) {
            uart_command[1] = rx_input;
        } else if (2==(rx_input>>6) || 3==(rx_input>>6)) {
            uart_command[2] = rx_input;
        }
        
        uart_command_counter++;  
    }  
}
    

int main(void)
{
     
    i2c_init(); /*setesup i2c comication in atmega*/
    motron_init(); /*setsup motorcontroller with default settings*/
    uart_init(104); /*setup uart with 9600 baud*/
    sei();
    bool motor3_toggle = false;
    while (1) {   
        if (uart_command_counter >= 3) {
        
            if (61 == uart_command[0]) {
                set_motor_3_speed(800);
            } else if (62 == uart_command[0]) {
                set_motor_3_speed(0);
            }
            
            if (uart_command[1]>>6 == 1) {
                uart_command[1] = uart_command[1] & 0b00111111;
                set_motor_1_speed(uart_command[1]*13);                
            } else if (uart_command[1]>>6 == 0) {
                uart_command[1] = uart_command[1] & 0b00111111;
                set_motor_1_speed(uart_command[1]*(-13));                    
            }
            
            if (uart_command[2]>>6 == 3) {
                uart_command[2] = uart_command[2] & 0b00111111;
                set_motor_2_speed(uart_command[2]*13);                 
            } else if (uart_command[2]>>6 == 2) {
                 uart_command[2] = uart_command[2] & 0b00111111;
                 set_motor_2_speed(uart_command[2]*(-13));                  
            }
                        
            uart_command_counter = 0;
        }


//         if (msg_in){
//             
//             if (61 == rx_input) {
//                 set_motor_3_speed(800);
//             } else if (62 == rx_input) {
//                 set_motor_3_speed(0);
//             }
//             
//             if (rx_input!=61 && rx_input!= 62 && (rx_input>>6)==0) {
//                 rx_input = rx_input & 0b00111111;
//                 set_motor_1_speed(rx_input*13);
//             } else if (rx_input!=61 && rx_input!= 62 && (rx_input>>6)==1){
//                 rx_input = rx_input & 0b00111111;
//                 set_motor_1_speed(rx_input*(-13));
//             } else if (rx_input!=61 && rx_input!= 62 && (rx_input>>6)==2){
//                 rx_input = rx_input & 0b00111111;
//                 set_motor_2_speed(rx_input*13);
//             } else if (rx_input!=61 && rx_input!= 62 && (rx_input>>6)==3){
//                 rx_input = rx_input & 0b00111111;
//                 set_motor_2_speed(rx_input*(-13));
//             }
//             
//             
// 
//             
//             _delay_ms(250); /*fix thiis!*/
//             set_motor_1_speed(0);
//             set_motor_2_speed(0);
//             set_motor_3_speed(0);
// 
//             msg_in = false;
//         } 
              
    }
}