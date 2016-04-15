/*
 * GccApplication1.c
 *
 * Created: 22.4.2015 21:00:39
 *  Author: Lukas
 */ 


#include <avr/io.h>
#include "inc/AllInit.h"
#include <util/delay.h>


#include "inc/I2C_Master.h"
#include <avr/interrupt.h>
#include "inc/common_defs.h"
#include "inc/defines.h"
#include "inc/timer.h"
#include "inc/uart_types.h"
#include "inc/uart_tri_0.h"
#include "inc/Tribus_types.h"
#include "inc/Tribus.h"
#include <stdlib.h>
#include "inc/EL_MAG.h"




volatile byte timer0_flag = 0; // T = 10ms
byte led_timer = 0;
uint8_t Tx_Pole[16]={0x00,0x00,0x00,0,0,0,0,0,0,0,0,0,0,0,0,0x00};
int Vraceno=0;

#define I2C_Slave1	0x01
#define I2C_Slave2	0x02
extern uint8_t recv[16]; //buffer to store received bytes


void send_data(void)
{
	uart0_put_data((byte *) &TB_bufOut);
}

//----------------------------------------------------------
ISR(TIMER1_CAPT_vect) {
	// T = 10ms
	timer0_flag = true;
}

//----------------------------------------------------------
void process_timer_100Hz(void)
{
	if (timer0_flag) { // T = 10ms
		timer0_flag = false;
		uart0_ISR_timer();
		if (led_timer > 0) {
			led_timer--;
			if (led_timer == 0) {
				PORTB ^= (1 << PB4);
			}
		}
	}
}

void try_receive_data(void)
{
	byte i;
	byte *ptr;
	
	if (uart0_flags.data_received) {
		ptr = uart0_get_data_begin();
		for (i=0; i<9; i++) {
			TB_bufIn[i] = *ptr;
			ptr++;
		}
		uart0_get_data_end();
		uart0_flags.data_received = FALSE;
		if (TB_Read() == 0) {
			switch (TB_Decode())
			{
				case TB_CMD_SIO:
					if(TB_bufIn[TB_BUF_MOTOR] == 2) // Elektromagnetický ventil è.1
					{
						switch (TB_bufIn[TB_BUF_TYPE])
						{
						case 0:
							//Nastaveni Elektromagnetického ventilu è.1
							if (TB_out.b0 != 0)
							{
								EM_MAG1_ON();
							} 
							else
							{
								EM_MAG1_OFF();
							}
							break;
						case 1:	
							//Nastaveni Elektromagnetického ventilu è.2
							if (TB_out.b1 != 0)
							{
								EM_MAG2_ON();
							}
							else
							{
								EM_MAG2_OFF();
							}
							break;
						case 2:
							//Nastaveni Rotaèní vyveva
							if (TB_out.b2 != 0)
							{
								RV_ON();
							}
							else
							{
								RV_OFF();
							}
							break;
						}
					}
					return;
				
				case TB_CMD_VENTIL:
					//Set point
						Tx_Pole[0] = TB_bufIn[TB_BUF_TYPE]; // MSB // Poslaní pøíkazu pro SET POINT/Measurament/FullClose/FullOpen
						Tx_Pole[1] = (uint8_t) TB_bufIn[6];
						Tx_Pole[2] = (uint8_t) TB_bufIn[7]; // LSB
						Tx_Pole[15] = 0xFF;
						
						//Komunikace po sériové lince Transmiter
						I2C_Start();
						//Odeslání dat po I2C do slave podle bufferu kterému chcee co poslat.
						while(((Vraceno = handleI2C_Transmit(Tx_Pole, TB_bufIn[TB_BUF_MOTOR])) == 0))
						{
						}
						/*switch(Vraceno)
						{
							case 1:
								TB_SendAck(TB_I2C_SEND_OK, 0);
							break;
							case 2:
								TB_SendAck(TB_I2C_SLAVE_NOT_RESPOND, TB_bufIn[TB_BUF_MOTOR]);
							break;
							default:
								TB_SendAck(TB_NOT_KNOW, 0);
							break;
						}
						*/
						I2C_Stop();
						
						_delay_ms(50);
						//Komunikace po sériové lince Transmiter
						
						I2C_Start();
						//Odeslání dat po I2C do slave podle bufferu kterému chcee co poslat.
						while(((Vraceno = handleI2C_Receive(TB_bufIn[TB_BUF_MOTOR])) == 0))
						{
							
						}
						long int Hodnota=0;
						Hodnota = recv[0];
						Hodnota <<= 8;
						Hodnota = Hodnota | recv[1];
						Hodnota <<= 8;
						Hodnota = Hodnota | recv[2];
						Hodnota <<= 8;
						Hodnota +=  recv[3];
						switch(Vraceno)
						{
							case 1:
								TB_SendAck(TB_I2C_REC_OK, Hodnota);
								break;
							case 2:
								TB_SendAck(TB_I2C_SLAVE_NOT_RESPOND, TB_bufIn[TB_BUF_MOTOR]);
							break;
								default:
								TB_SendAck(TB_NOT_KNOW, 0);
							break;
						}
						I2C_Stop();
						
					}
					return;
				
			}
		}
}

int main(void)
{
	DDRA |= (1 << DDA7) | (1 << DDA6) | (1 << DDA5) | (1 << DDA4) | (1 << DDA3) | (1 << DDA2) | (1 << DDA1);
	DDRB |= (1 << DDB4) | (1 << DDB3);
	DDRC |= (1 << DDC7) | (1 << DDC6) | (1 << DDC5) | (1 << DDC4);
	DDRD |= (1 << DDD4) | (1 << DDD3) | (1 << DDD2);
	
	//DDRA = 0b11111110;
	//DDRB = 0b00011000;
	//DDRC = 0b11110000;
	//DDRD = 0b00011110;
	
	
	//RS232_Init(RS232_9600);
	
	I2C_Init();
	
	timer_init();
	uart0_init();
	TB_Callback_setBaud = &uart0_set_baud;
	TB_Callback_TX = &send_data;
	TB_Init((void*) 0x10); // addr in eeprom with settings
	
	//Zapnutí led diody pro indikaci zapnutí desky
	LED_Supply();
	
	sei();
	
    while(1)
    {
		//RS232_Transmit_Char(0x01);
		//handleI2C_master();
		
		process_timer_100Hz();
		uart0_process();
		try_receive_data();
		/*
		I2C_Start();
		while(((Vraceno = handleI2C_Transmit(Tx_Pole, 0x02)) == 0))
		{
			
		}
		*/
    }
}
