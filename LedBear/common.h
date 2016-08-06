/*
 * HAL.h
 *
 * Created: 08.03.2015 22:32:44
 * прошивка для управления RGB светодиодной лентой c IR пульта
 * устройство построено на базе atmega88pa + 8МГц кварц
 * вход: PD2 (INT0) - с ИК-приемника TSOP17
 * выходы ШИМ (RGB): PD6, PD5, PB1
 *
 */ 

#ifndef COMMON_H_
#define COMMON_H_


//HAL.c
void init(void);
void heartbeat(void);

void setFullR(void);
void setOffR(void);
void setValR(uint8_t);

void setFullG(void);
void setOffG(void);
void setValG(uint8_t);

void setFullB(void);
void setOffB(void);
void setValB(uint8_t);

void USART_Transmit(char);
void USART_Transmit_len(char *, uint8_t);
void USART_Transmit_str(char *);

//RGB.c
void incValueR(void);
void decValueR(void);
void switchValueR(void);

void incValueG(void);
void decValueG(void);
void switchValueG(void);

void incValueB(void);
void decValueB(void);
void switchValueB(void);

void switchAll(void);
void switchAllMemo(void);

void setNextHalfSin();
void setPaletteMode(uint8_t);
void checkAndChangePalette(void);

void loadRGBSettings(void);
void saveRGBSettings(void);

//irRemote.c
//список команд ИК-пульта и соответствующих им кодов
typedef enum
{
	cmdChMinus = 0xA25D,	cmdCh = 0x629D,		cmdChPlus = 0xE21D,		cmdPrev = 0x22DD,
	cmdNext = 0x02FD,		cmdPlay = 0xC23D,	cmdVolMinus = 0xE01F,	cmdVolPlus = 0xA857,
	cmdEq = 0x906F,			cmd0 = 0x6897,		cmd100 = 0x9867,		cmd200 = 0xB04F,
	cmd1 = 0x30CF,			cmd2 = 0x18E7,		cmd3 = 0x7A85,			cmd4 = 0x10EF,
	cmd5 = 0x38C7,			cmd6 = 0x5AA5,		cmd7 = 0x42BD,			cmd8 = 0x4AB5,
	cmd9 = 0x52AD,			cmdRepeat = 0xFF,	cmdNone = 0x00
} TIrCmd;

void putIrCmd(uint16_t data);
uint16_t getIrCmd();

void irSignalFront(uint8_t period);
void irSignalTimeout(void);

#endif /* COMMON_H_ */