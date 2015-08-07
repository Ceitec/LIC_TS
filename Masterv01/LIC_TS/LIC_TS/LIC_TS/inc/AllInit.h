/*
Autor:	Lukas
Soubor:	Hlavicka s daty
*/


#ifndef AllInit_H_
#define AllInit_H_

#define F_CPU 14745600UL

///////////////////////////////////////////////////////////////////////////
// Definování rychlosti sériové linky pro rychlou editaci #CPU 14745600UL.
///////////////////////////////////////////////////////////////////////////

#define	RS232_2400		383
#define RS232_4800		191
#define RS232_9600		95
#define RS232_14400		63
#define RS232_19200		47
#define RS232_28800		31
#define RS232_38400		23
#define RS232_57600		15
#define RS232_76800		11
#define RS232_115200	7
#define RS232_234000	3

//Definovani TRUE a FALSE
#define TRUE	1
#define	FALSE	0

//Deklarace procedur
void HB_LED_RXC(); /* Negace diody pro indikaci komunikace*/
void Text_to_Buffer(char Buffer); /*Uklada do Bufferu chary */
void Null_Buffer(); /* Vynuluje buffer */


#endif /* AllInit_H_ */