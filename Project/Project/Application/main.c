/*
 * Project.c
 *
 * Created: 2026-02-10 15:39:32
 * Author : ludvig
 */ 
#include "include.h"


volatile uint8_t rx_input = 0;
volatile bool msg_in = false;
volatile bool motor3_toggle = false;


ISR (USART_RX_vect){ /*interrupt to get serial data*/
    rx_input = UDR0;
    UDR0 = rx_input;
    msg_in = true;    
}
    

int main(void)
{
     
    i2c_init(); /*setesup i2c comication in atmega*/
    motron_init(); /*setsup motorcontroller with default settings*/
    uart_init(104); /*setup uart with 9600 baud*/
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