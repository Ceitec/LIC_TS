

#include <util/twi.h>




void I2C_Stop();
void I2C_Start();
void I2C_Init();
void handleI2C_master();
int handleI2C_Transmit(unsigned char* Pole, unsigned char Slave);
int handleI2C_Receive(unsigned char Slave);