


#include "I2C_Master.h"
#include "EL_MAG.h"

#define TW_MASK_CR 0x0F
#define TW_MASK_CR_TWEA 0x4F

//setup the I2C hardware to ACK the next transmission
//and indicate that we've handled the last one.
//#define TWACK (TWCR |= (1<<TWINT))
#define TWACK (TWCR = (TWCR & TW_MASK_CR) | (1 << TWINT))

//setup the I2C hardware to NACK the next transmission
#define TWNACK (TWCR |= (TWCR & TW_MASK_CR) | (1 << TWINT) | (1 << TWEA))


//reset the I2C hardware (used when the bus is in a illegal state)
#define TWRESET (TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (1 << TWEA))

//Send a start signal on the I2C bus
//#define TWSTART (TWCR |= (1<<TWINT) |(1<<TWSTA))
#define TWSTART (TWCR = (TWCR & TW_MASK_CR) | (1<<TWINT) |(1<<TWSTA))

//Send a stop signal on the I2C bus
//#define TWSTOP (TWCR |= (1<<TWINT) |(1<<TWSTO))
#define TWSTOP (TWCR = (TWCR & TW_MASK_CR) | (1<<TWINT) |(1<<TWSTO))

//Repeat start will be transmitted Pro 0x28 pro pøíjem
#define TWREPSTART (TWCR = (TWCR & TW_MASK_CR_TWEA) | (1 << TWSTA) | (1 << TWINT))

//Data byte will be received and ACK will be returned
#define TWACK_REC (TWCR = (TWCR & TW_MASK_CR) | (1 << TWEA) | (1 << TWINT))





//slave address and SLA signals
#define I2C_SLAVE_ADDRESS 0x01
//uint8_t I2C_SLAVE_ADDRESS=0x02;

#define CHK(x,y) (x&(1<<y))


//global variables
#define BUFLEN_RECV 16
uint8_t r_index=0;
volatile uint8_t recv[BUFLEN_RECV]; //buffer to store received bytes

//Definování co chcu poslat...
#define BUFLEN_TRAN 16
uint8_t t_index=0;
//uint8_t tran[BUFLEN_TRAN]= {1,2,3,4,5,6,7,8,0,0,0,0,0,0,0,0xFF};
//variable to indicate if something went horribly wrong
uint8_t reset=0;


void I2C_Init()
{
	TWBR = 0x0A;
	TWCR = (1 << TWEN) | (1 << TWEA);
}

void I2C_Start()
{
	TWSTART;
}
void I2C_Stop()
{
	TWSTOP;
}

int handleI2C_Receive(unsigned char Slave)
{
	LED_RT_On();
	unsigned char SLA_R;
	SLA_R = ((Slave << 1) | TW_READ);
	
	//keep track of the modus (receiver or transmitter)
	int Hlaska=0;
	if(CHK(TWCR,TWINT)){
		switch(TW_STATUS){
			//start or rep start send, determine mode and send SLA R or W
			case 0x10:
				//RS232_Transmit_Char(0x10);
			case 0x08:
				//RS232_Transmit_Char(0x08);
				//reset buffer indices
				t_index =0;
				r_index =0;
				//send SLA R .
				TWDR = SLA_R;
				TWACK;
				break;
			
			case 0x28: //data acked by addressed receiver
				//RS232_Transmit_Char(0x28);
				TW_READ;
				//TWSTART;
				TWREPSTART;
				break;
			
			case 0x38: //arbitration lost, do not want
				//RS232_Transmit_Char(0x38);
				
				TWREPSTART;
				//TWACK;
				break;
				//-------------------------Master receiver mode-------------------
				//SLA_R acked, nothing to do for master, just wait for data
			
			case 0x40:
				//RS232_Transmit_Char(0x40);
				recv[0] = TWDR;
				r_index = 1;
				TWACK_REC;
				break;
				//SLA_R not acked, something went wrong, start over
			
			case 0x48:
				//RS232_Transmit_Char(0x48);
				//TWSTART;
				//TWACK;
				
				//Odeslan stop stav TWSTA = 0, TWINT = 1, TWSTO = 1
				//TWCR &= ~(1 << TWSTA);
				//TWCR |= (1 << TWINT) | (1 << TWSTO);
				TWCR = ((TWCR & TW_MASK_CR_TWEA) | (1 << TWSTO) | (1 << TWINT));
				Hlaska = 2;
				break;
				//non-last data acked (the last data byte has to be nacked)
					
			case 0x50:
				//RS232_Transmit_Char(0x50);
				//store it
				recv[r_index] = TWDR;
				r_index++;
				//if the next byte is not the last, ack the next received byte
				if(r_index < BUFLEN_RECV)
				{
					TWCR = ((TWCR & TW_MASK_CR) | (1 << TWEA) | (1 << TWINT));
					//TWCR &= ~((1 << TWSTO) | (1 << TWSTA));
					//TWCR |= (1<<TWINT) | (1<<TWEA);
				}
				//otherwise NACK the next byte
				else
				{
					//TWNACK;
					//TWCR &= ~((1 << TWSTO) | (1 << TWSTA) | (1 << TWEA));
					//TWCR |= (1 << TWINT);
					
					TWCR = ((TWCR & TW_MASK_CR) | (1 << TWINT));
					r_index = BUFLEN_RECV;
					
				}
			
				break;
			
			case 0x58: //last data nacked, as it should be
				//RS232_Transmit_Char(0x58);
				
				//Odeslan stop stav TWSTA = 0, TWINT = 1, TWSTO = 1
				TWCR = ((TWCR & TW_MASK_CR_TWEA) | (1 << TWSTO) | (1 << TWINT) | (1 << TWEN));
				//TWCR &= ~(1 << TWSTA);
				//TWCR |= (1 << TWINT) | (1 << TWSTO);
				
				//TW_WRITE;
				//TWSTART;
				Hlaska = 1;
			break;
			
			//--------------------- bus error---------------------------------
			case 0x00:
				//RS232_Transmit_Char(0x00);
				//TWRESET;
				//TWSTART;
				TWSTOP;
			break;
		}
	}
	LED_RT_Off();
	return Hlaska;
}

int handleI2C_Transmit(unsigned char* Pole, unsigned char Slave)
{
	LED_RT_On();
	unsigned char SLA_W;
	SLA_W = ((Slave << 1) | TW_WRITE);
	//keep track of the modus (receiver or transmitter)

	int Hlaska=0;

	if(CHK(TWCR,TWINT)){
		switch(TW_STATUS){
			//start or rep start send, determine mode and send SLA R or W
			case 0x10:
				//RS232_Transmit_Char(0x10);
			case 0x08:
				
				//RS232_Transmit_Char(0x08);
				//reset buffer indices
				t_index =0;
				r_index =0;
				//send SLA W or R depending on what mode we want.
				TWDR = SLA_W;
				TWACK;
				
			break;
			
			//--------------- Master transmitter mode-------------------------
			case 0x18: // SLA_W acked
				//RS232_Transmit_Char(0x18);
				//load first data
				//TWDR = tran[0];
				TWDR = Pole[t_index];
				t_index=1;
				TWACK;
			break;

			//SLA_W not acked for some reason (disconnected?), keep trying
			case 0x20:
				//Sem bude return napøíklad 2
				//RS232_Transmit_Char(0x20);
				//TWACK;
				/*TWCR |= (1 << TWSTA) | (1 << TWSTO);*/
				TWSTOP;
				Hlaska = 2;
			break;
			
			case 0x28: //data acked by addressed receiver
				//RS232_Transmit_Char(0x28);
				//load next byte if we're not at the end of the buffer
				if(t_index < BUFLEN_TRAN){
					//TWDR =  tran[t_index];
					TWDR = Pole[t_index];
					t_index++;
					TWACK;
					break;
				}	
				else
				{
					Hlaska = 1;
					t_index=0;					
					TWSTOP;
					break;
				}
				
			case 0x38: //arbitration lost, do not want
				//RS232_Transmit_Char(0x38);
				//data nacked, could be faulty buffer, could be dc, start over
			
			case 0x30:
				//RS232_Transmit_Char(0x30);
				TWSTOP;
				Hlaska = 2;
			break;
			
			case 0x48:
				//RS232_Transmit_Char(0x48);
				TWSTOP;
			break;
			
			//--------------------- bus error---------------------------------
			case 0x00:
				//RS232_Transmit_Char(0x00);
				TWSTOP;
			break;
		}
	}
	LED_RT_Off();
	return Hlaska;
	
}




/*
S: START condition
Rs: REPEATED START condition
R: Read bit (high level at SDA)
W: Write bit (low level at SDA)
A: Acknowledge bit (low level at SDA)
A: Not acknowledge bit (high level at SDA)
Data: 8-bit data byte
P: STOP condition
SLA: Slave Address
*/
/*		//Start condition
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
//Cekam dokud se nevynuluje TWINT a tudiz
while (!(TWCR & (1 << TWINT)));

//TW_START
if ((TW_STATUS == TW_START) | (TW_STATUS == TW_REP_START))
{
PORTB |= (1 << PB1);
TWDR = SLAVE;
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
_delay_ms(1);
}
else
{
PORTB |= (1 << PB2);
ERR = 1;
_delay_ms(1);
}

while (!(TWCR & (1 << TWINT)));

if (TW_STATUS == TW_MT_SLA_ACK)
{
PORTB |= (1 << PB3);
TWDR = DATA;
TWCR = (1 << TWINT) | (1 << TWEN);
_delay_ms(1);
}
else
{
PORTB |= (1 << PB4);
ERR = 1;
_delay_ms(1);
}
while (!(TWCR & (1 << TWINT)));

if (TW_STATUS == TW_MT_DATA_ACK)
{
PORTB |= (1 << PB5);
TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
_delay_ms(1);
}
else
{
PORTB |= (1 << PB6);
ERR = 1;
_delay_ms(1);
}

PORTB ^= (1 << PB0);
_delay_ms(500);
*/

//Tak tady dolu je funkèní pro odeslání a pøíjem
/*
void handleI2C_master(){
	unsigned char SLA_W;
	SLA_W = ((I2C_SLAVE_ADDRESS << 1) | TW_WRITE);
	unsigned char SLA_R;
	SLA_R = ((I2C_SLAVE_ADDRESS << 1) | TW_READ);
	//keep track of the modus (receiver or transmitter)
	static uint8_t mode;
	
	if(CHK(TWCR,TWINT)){
		switch(TW_STATUS){
			//start or rep start send, determine mode and send SLA R or W
			case 0x10:
				//RS232_Transmit_Char(0x10);
			case 0x08:
				//RS232_Transmit_Char(0x08);
				//reset buffer indices
				t_index =0;
				r_index =0;
				//send SLA W or R depending on what mode we want.
				if(mode == TW_WRITE)
				{
					TWDR = SLA_W;
				}
				else TWDR = SLA_R;
				{
					TWACK;
				}
			break;
			
			//--------------- Master transmitter mode-------------------------
			case 0x18: // SLA_W acked
				//RS232_Transmit_Char(0x18);
				//load first data
				//TWDR = tran[0];
				//t_index=1;
				//TWACK;
			break;

			//SLA_W not acked for some reason (disconnected?), keep trying
			
			case 0x20:
			
				//RS232_Transmit_Char(0x20);
				//TWCR =0;
				//TWSTART;
			break;
			
			case 0x28: //data acked by addressed receiver
				//RS232_Transmit_Char(0x28);
				//load next byte if we're not at the end of the buffer
				if(t_index < BUFLEN_TRAN){
					TWDR =  tran[t_index];
					t_index++;
					TWACK;
				break;
				}
				//otherwise, switch mode and send a start signal
				else {
					mode = TW_READ;
					TWSTART;
				break;
				}
				
			case 0x38: //arbitration lost, do not want
				//RS232_Transmit_Char(0x38);
				//data nacked, could be faulty buffer, could be dc, start over
			
			case 0x30:
			
				//RS232_Transmit_Char(0x30);
				TWCR = 0;
				TWSTART;
			break;
			//-------------------------Master receiver mode-------------------
			//SLA_R acked, nothing to do for master, just wait for data
	
			case 0x40:
				//RS232_Transmit_Char(0x40);
				TWACK;
			break;
			//SLA_R not acked, something went wrong, start over
			
			case 0x48:
				//RS232_Transmit_Char(0x48);
				TWSTART;
			break;
			//non-last data acked (the last data byte has to be nacked)
			
			case 0x50:
				//RS232_Transmit_Char(0x50);
				//store it
				recv[r_index] = TWDR;
				r_index++;
				//if the next byte is not the last, ack the next received byte
				if(r_index < BUFLEN_RECV){
					TWACK;
				}
				//otherwise NACK the next byte
				else {
					TWNACK;
					r_index =BUFLEN_RECV;
				}
			break;
			
			case 0x58: //last data nacked, as it should be
				//RS232_Transmit_Char(0x58);
				//switch to other mode, and send start signal
				mode = TW_WRITE;
				TWSTART;
			break;
			
			//--------------------- bus error---------------------------------
			case 0x00:
				//RS232_Transmit_Char(0x00);
				TWRESET;
				TWSTART;
			break;
		}
	}
}
*/