
/*

Program: Bronkhorst
Knihovna: RS232.c
Vlastník: Lukáš Novák

*/

#include <util/delay.h>
#include <avr/io.h>
#include "RS232.h"


//////////////////////////////////////////////////////////////////////
/*		Inicializaèní procedura obsahující nastavení odesílání,		*/
/*		pøíjem dat a povolení pøerušení pøíjmu po sériové lince.	*/
/*			Dále nastavení R/T pomocí 8 bitù a 1 stop bit.			*/
//////////////////////////////////////////////////////////////////////


void RS232_Init( unsigned int baud )
{
	
	UBRRH = (unsigned char)(baud>>8);							//Registr pro nastavení vrchních bytù rychlosti sériové linky.
	UBRRL = (unsigned char)baud;								//Registr pro nastavení spodních bytù rychlosti sériové linky.
	UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);					//Povolení v registru odesílání, pøíjmu a povolení pøerušení pøíjmu
	UCSRC = (1<<URSEL) | (0<<USBS) | (1<<UCSZ0)| (1<<UCSZ1);	//Nastavení R/T 8 bitù data, 1 stop bitu, asynchronní pøenos a bez parity.
}

//////////////////////////////////////////////////////////////////////////
/*				Procedura pro pøíjem jednoho znaku a ukládání			*/
/*					z registru UDR do vrácené promìnné.					*/
//////////////////////////////////////////////////////////////////////////

unsigned char RS232_Receive_Char( void )
{
  unsigned char data;
  while ( !(UCSRA & (1<<RXC)) );	//Èekání dokud není dokonèen pøenos dat
  data=UDR;							//Ètení dat z registru UDR a ukládání do promìnné
  return data;						//Vrácení pøijmutého znaku
}

//////////////////////////////////////////////////////////////////////////
/*  		Procedura pro prijem vice znaku po sériové lince.			*/
//////////////////////////////////////////////////////////////////////////

void RS232_Receive_String( void )
{
	int RxBPtr=0;
	RxBPtr = 256;
	char RxB[RxBPtr];
	while (RS232_Receive_Char() != 0x0A)
	{
		RxB[RxBPtr] = RS232_Receive_Char();
		RS232_Transmit_Char(RxB[RxBPtr]);
		RxBPtr++;
		
	}
}

//////////////////////////////////////////////////////////////////////////
/*		Procedura pro odesílání jednoho znaku po sériové lince.			*/
//////////////////////////////////////////////////////////////////////////

void RS232_Transmit_Char( unsigned char data )
{
  while ( !( UCSRA & (1<<UDRE)) );	//Èekání dokud není prázdný buffer
  UDR = data;						//Nastavení znaku do registru
}

//////////////////////////////////////////////////////////////////////////
/*			Rozšíøení procedury pro odesílání jednoho znaku,			*/
/*						 na odesílání pole znakù.						*/
//////////////////////////////////////////////////////////////////////////

void RS232_Transmit_String(char* StringPtr){
	
	while(*StringPtr != 0x00)			//Opakování cyklu dokud se nevyprázdní buffer
	{
		RS232_Transmit_Char(*StringPtr);		//Využití procedury pro odeslání jednoho znaku
		StringPtr++;					//Posunutí o další znak
	}
	
}

//////////////////////////////////////////////////////////////////////////
/*				 Upravená procedura odesílání pole znakù				*/
/*			s pøidaným ukonèovacím znakem <CR> pro AT pøíkazy.			*/
//////////////////////////////////////////////////////////////////////////

void RS232_Transmit_String_CR(char* StringPtr){
	
	while(*StringPtr != 0x00)			//Opakování cyklu dokud se nevyprázdní buffer
	{
		RS232_Transmit_Char(*StringPtr);		//Využití procedury pro odesílání jednoho znaku
		StringPtr++;					//Posunutí o další znak
	}
	RS232_Transmit_Char(0x0D);				//Na konci odeslán znak <CR>
}


void RS232_Transmit_String_Buffer(char* StringPtr, int TXStringPtr)
{
	
//	while(*StringPtr != 0x0A)			//Opakování cyklu dokud se nevyprázdní buffer
	while(TXStringPtr != 0)
	{
		RS232_Transmit_Char(*StringPtr);		//Využití procedury pro odesílání jednoho znaku
		StringPtr++;					//Posunutí o další znak
		TXStringPtr--;
	}
}

