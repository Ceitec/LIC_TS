/*
Program: Bronkhorst
Knihovna: EL_MAG.h
Vlastník: Lukáš Novák
*/

//Definování elmag ventilù

#define EM_MAG1_OVL			PINA7
#define EM_MAG1_GREEN		PINA2
#define EM_MAG1_RED			PINA3
#define EM_MAG2_OVL			PINA6
#define EM_MAG2_GREEN		PINA4
#define EM_MAG2_RED			PINA5
#define ROT_VYVEV_OVL		PINA1
#define ROT_VYVEV_GREEN		PIND4
#define ROT_VYVEV_RED		PIND3
#define LED_SUPPLY_ON		PINB3



#ifndef EL_MAG_H_
#define EL_MAG_H_

//Nastavení elektromagnetických ventilù
void EM_MAG1_ON(void);
void EM_MAG1_OFF(void);
void EM_MAG2_ON(void);
void EM_MAG2_OFF(void);
void RV_ON(void);
void RV_OFF(void);
void LED_RT_On(void);
void LED_RT_Off(void);

// Nastavení LED svìtel
void LED_Supply(void);

/*void LED_RV_On(void);
void LED_RV_Off(void);
void LED_EMV1_On(void);
void LED_EMV1_Off(void);
void LED_EMV2_On(void);
void LED_EMV2_Off(void);
void LED_Supply(void);
*/
#endif /* EL_MAG_H_ */