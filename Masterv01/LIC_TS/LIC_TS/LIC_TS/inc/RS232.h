/*
Program: Bronkhorst
Knihovna: RS232.h
Vlastník: Lukáš Novák
*/


#ifndef RS232_H_
#define RS232_H_

void RS232_Init( unsigned int baud );

unsigned char RS232_Receive_Char( void );

void RS232_Receive_String( void );

void RS232_Transmit_Char( unsigned char data );

void RS232_Transmit_String(char* StringPtr);

void RS232_Transmit_String_CR(char* StringPtr);

void RS232_Transmit_String_Buffer(char* StringPtr, int TXStringPtr); /* Do StringPtr se zapise char(String) hodnota */
/* Do TXStringPtr se zapise velikost StringPtr */



#endif /* RS232_H_ */