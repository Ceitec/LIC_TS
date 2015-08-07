/*
Program: Bronkhorst
Knihovna: EL_MAG.h
Vlastník: Lukáš Novák
*/


#include <avr/io.h>
#include "EL_MAG.h"


void EM_MAG1_ON(void)
{
	PORTA |= (1 << EM_MAG1_OVL);
	PORTA &= ~(1 << EM_MAG1_RED);
	PORTA |= (1 << EM_MAG1_GREEN);
}

void EM_MAG1_OFF(void)
{
	PORTA &= ~(1 << EM_MAG1_OVL);
	PORTA |= (1 << EM_MAG1_RED);
	PORTA &= ~(1 << EM_MAG1_GREEN);
}

void EM_MAG2_ON(void)
{
	PORTA |= (1 << EM_MAG2_OVL);
	PORTA &= ~(1 << EM_MAG2_RED);
	PORTA |= (1 << EM_MAG2_GREEN);
}

void EM_MAG2_OFF(void)
{
	PORTA &= ~(1 << EM_MAG2_OVL);
	PORTA |= (1 << EM_MAG2_RED);
	PORTA &= ~(1 << EM_MAG2_GREEN);
}

void RV_ON(void)
{
	PORTA |= (1 << ROT_VYVEV_OVL);
	PORTD |= (1 << ROT_VYVEV_GREEN);
	PORTD &= ~(1 << ROT_VYVEV_RED);
}

void RV_OFF(void)
{
	PORTA &= ~(1 << ROT_VYVEV_OVL);
	PORTD |= (1 << ROT_VYVEV_RED);
	PORTD &= ~(1 << ROT_VYVEV_GREEN);
}
/*
void LED_EMV1_On(void)
{
	PORTA |= (1 << PA2);
	PORTA &= ~(1 << PA3);
}

void LED_EMV1_Off(void)
{
	PORTA |= (1 << PA3);
	PORTA &= ~(1 << PA2);
}

void LED_EMV2_On(void)
{
	PORTA |= (1 << PA4);
	PORTA &= ~(1 << PA5);
}

void LED_EMV2_Off(void)
{
	PORTA |= (1 << PA5);
	PORTA &= ~(1 << PA4);
}
*/

void LED_Supply(void)
{
	PORTB |= (1 << LED_SUPPLY_ON);
	EM_MAG1_OFF();
	EM_MAG2_OFF();
	RV_OFF();
	//PORTA |= (1 << EM_MAG1_GREEN);
	//PORTA |= (1 << EM_MAG2_GREEN);
	//PORTD |= (1 << ROT_VYVEV_GREEN);
	//PORTD |= (1 << ROT_VYVEV_RED);
}

void LED_RT_On(void)
{
	PORTB |= (1 << PB4);
}

void LED_RT_Off(void)
{
	PORTB &= ~(1 << PB4);
}