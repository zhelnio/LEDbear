/*
 * RGB.c
 *
 * Created: 08.03.2015 22:51:19
 *  Author: Stanislav Zhelnio
 *
 * �������� ��� ���������� RGB ������������ ������ c IR ������
 * ���������� ��������� �� ���� atmega88pa + 8��� �����
 * ����: PD2 (INT0) - � ��-��������� TSOP17
 * ������ ��� (RGB): PD6, PD5, PB1
 *
 * ������ ��������� ������
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
#include "common.h"

//������������� �������� �������� � ������ �������������� ������
#define MAX_PALLETE_MODE 10

//����� �������� ��������� �������
#define SCALE_SIZE	14
uint8_t logValue[] = {0,1,2,3,4,6,10,16,26,40,64,102,161,255};

//�������� ��������� �����������:
// - ������� RGB-�������
// - �������� �������� � ������ �������������� ������
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t paletteMode;
} TRGBSettings;

TRGBSettings rgbEeprom EEMEM;
TRGBSettings rgb;

uint8_t oldValueR = 0;
uint8_t oldValueG = 0;
uint8_t oldValueB = 0;

//*************** ������� �������� ������ R *************** 
//��������������� ��������� ������� ������
void setValueR(uint8_t newVal)
{
	if(newVal == rgb.r)
		return;
		
	rgb.paletteMode = 0;
	rgb.r = newVal;
	
	if(newVal == 0)
		setOffR();	//���������
	else if(newVal > SCALE_SIZE)
		setFullR(); //�������� �� ��������
	else
		setValR(logValue[newVal - 1]);	//���������� �������� ��������
}

//��������-���������
void switchValueR(void)
{
	uint8_t newVal;
	if(rgb.r)
	{
		newVal = 0;
		oldValueR = rgb.r;
	}
	else if(oldValueR)
		newVal = oldValueR;
	else
		newVal = SCALE_SIZE;
	setValueR(newVal);
}

//��������� ������� �� �������
void incValueR(void)
{
	uint8_t newVal = (rgb.r == SCALE_SIZE) ? 0 : rgb.r + 1;
	setValueR(newVal);
	oldValueR = rgb.r;
}

//��������� ������� �� �������
void decValueR(void)
{
	uint8_t newVal = (rgb.r == 0)? SCALE_SIZE : rgb.r - 1;
	setValueR(newVal);
	oldValueR = rgb.r;
}

//*************** ������� �������� ������ G *************** 
//��������������� ��������� ������� ������
void setValueG(uint8_t newVal)
{
	if(newVal == rgb.g)
		return;
	
	rgb.paletteMode = 0;
	rgb.g = newVal;
	
	if(newVal == 0)
		setOffG();	//���������
	else if(newVal > SCALE_SIZE)
		setFullG(); //�������� �� ��������
	else
		setValG(logValue[newVal - 1]);	//���������� �������� ��������
}

//��������-���������
void switchValueG(void)
{
	uint8_t newVal;
	if(rgb.g)
	{
		newVal = 0;
		oldValueG = rgb.g;
	}
	else if(oldValueG)
		newVal = oldValueG;
	else
		newVal = SCALE_SIZE;
	setValueG(newVal);
	
}

//��������� ������� �� �������
void incValueG(void)
{
	uint8_t newVal = (rgb.g == SCALE_SIZE) ? 0 : rgb.g + 1;
	setValueG(newVal);
	oldValueG = rgb.g;
}

//��������� ������� �� �������
void decValueG(void)
{
	uint8_t newVal = (rgb.g == 0)? SCALE_SIZE : rgb.g - 1;
	setValueG(newVal);
	oldValueG = rgb.g;
}

//*************** ������� �������� ������ B *************** 
//��������������� ��������� ������� ������
void setValueB(uint8_t newVal)
{
	if(newVal == rgb.b)
		return;
	
	rgb.paletteMode = 0;
	rgb.b = newVal;
	
	if(newVal == 0)
		setOffB();	//���������
	else if(newVal > SCALE_SIZE)
		setFullB(); //�������� �� ��������
	else
		setValB(logValue[newVal - 1]);	//���������� �������� ��������
}

//��������-���������
void switchValueB(void)
{
	uint8_t newVal;
	if(rgb.b)
	{
		newVal = 0;
		oldValueB = rgb.b;
	}
	else if(oldValueB)
		newVal = oldValueB;
	else
		newVal = SCALE_SIZE;
	setValueB(newVal);
}

//��������� ������� �� �������
void incValueB(void)
{
	uint8_t newVal = (rgb.b == SCALE_SIZE) ? 0 : rgb.b + 1;
	setValueB(newVal);
	oldValueB = rgb.b;
}

//��������� ������� �� �������
void decValueB(void)
{
	uint8_t newVal = (rgb.b == 0)? SCALE_SIZE : rgb.b - 1;
	setValueB(newVal);
	oldValueB = rgb.b;
}


//���������-�������� �� ��������
void switchAll()
{
	if(rgb.r || rgb.g || rgb.b)
	{
		setValueR(0);
		setValueG(0);
		setValueB(0);
	}
	else
	{
		setValueR(SCALE_SIZE);
		setValueG(SCALE_SIZE);
		setValueB(SCALE_SIZE);
	}
}

//���������-�������� ��� ����
void switchAllMemo()
{
	if(rgb.r || rgb.g || rgb.b)
	{
		setValueR(0);
		setValueG(0);
		setValueB(0);
	}
	else if(oldValueR || oldValueG || oldValueB)
	{
		setValueR(oldValueR);
		setValueG(oldValueG);
		setValueB(oldValueB);
	}
	else
	{
		setValueR(SCALE_SIZE);
		setValueG(SCALE_SIZE);
		setValueB(SCALE_SIZE);
	}
}

//������� �������� ����������
//������������ ��� ������� ��������� �����
//����� 170 ��������, ���������� 1/3 ������� - ������� ��������
uint8_t half_sin[] = {
	0,5,9,14,19,24,28,33,38,42,47,51,56,61,65,70,74,79,83,88,
	92,96,101,105,109,114,118,122,126,130,134,138,142,146,150,154,157,161,165,168,
	172,175,179,182,185,188,192,195,198,201,203,206,209,212,214,217,219,222,224,226,
	228,230,232,234,236,238,239,241,243,244,245,247,248,249,250,251,251,252,253,253,
	254,254,255,255,255,255,255,255,255,254,254,253,253,252,251,251,250,249,248,247,
	245,244,243,241,239,238,236,234,232,230,228,226,224,222,219,217,214,212,209,206,
	203,201,198,195,192,188,185,182,179,175,172,168,165,161,157,154,150,146,142,138,
	134,130,126,122,118,114,109,105,101,96,92,88,83,79,74,70,65,61,56,51,
	47,42,38,33,28,24,19,14,9,5,0
};

//���������� �������� ���������� � ������ �������� �� ����
uint8_t getHalfSinValue(uint8_t counter, uint8_t phase)
{
	counter += phase;
	if(counter >= sizeof(half_sin))
		return 0;
	else 
		return half_sin[counter];
}

//������������� ��������� ������� ����� �� ��������
//���������� - ������ ����� ������������ �� ������� ����������
//�� ��������� �� 1/3 ������� �� �����������
void setNextHalfSin(void)
{
	static uint8_t counter = 0;
	setValR(getHalfSinValue(counter,0));
	setValG(getHalfSinValue(counter,85));
	setValB(getHalfSinValue(counter,170));
	counter++;
}

//���������� �������� ��� ����� ��������
//��� ������� �������� ������� ������ �� ������������
inline void setPaletteMode(uint8_t mode)
{
	rgb.paletteMode = (mode <= MAX_PALLETE_MODE) ? mode : MAX_PALLETE_MODE;
}

//�������� ������������� ������� ��������
//� �� �����, � ������, ���� ��������� ����������� ��������
inline void checkAndChangePalette()
{
	static uint8_t paletteModeStepsPeriod = 0;

	if(rgb.paletteMode)
	{
		if(!paletteModeStepsPeriod)
		{
			paletteModeStepsPeriod = rgb.paletteMode;
			setNextHalfSin();
		}
		paletteModeStepsPeriod--;
	}
}

//������������ ��������� �� ������
inline void loadRGBSettings(void)
{
	TRGBSettings data;
	eeprom_read_block(&data,&rgbEeprom,sizeof(TRGBSettings));
	setValueR(data.r);
	setValueG(data.g);
	setValueB(data.b);
	setPaletteMode(data.paletteMode);
}

//��������� ��������� � ������
inline void saveRGBSettings(void)
{
	eeprom_update_block(&rgb,&rgbEeprom,sizeof(TRGBSettings));
}
