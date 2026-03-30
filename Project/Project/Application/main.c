/*
 * Project.c
 *
 * Created: 2026-02-10 15:39:32
 * Author : ludvig
 */ 

 
#include "include.h"


volatile uint8_t rx_input = 0;
volatile uint8_t uart_command[3] = {0,0,0};
volatile uint8_t uart_command_counter = 0;


ISR (USART_RX_vect){ /*interrupt to get serial data*/
    rx_input = UDR0;
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
     
    i2c_init(); /*setup i2c communication in atmega*/
    motron_init(); /*setup motor controller with default settings*/
    uart_init(104); /*setup uart with 9600 baud*/
    sei();
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
              
    }
}