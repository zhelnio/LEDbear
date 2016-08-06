/*
 * main.c
 *
 * Created: 07.03.2015 12:52:56
 *  Author: Stanislav Zhelnio
 *
 * прошивка для управления RGB светодиодной лентой c IR пульта
 * устройство построено на базе atmega88pa + 8МГц кварц
 * вход: PD2 (INT0) - с ИК-приемника TSOP17
 * выходы ШИМ (RGB): PD6, PD5, PB1 
 *
 */ 

#define	BAUDRATE	9600 // Скорость обмена данными
#define	F_CPU		1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "common.h"

//обработка полученной ИК команды
void ParseCmd(TIrCmd cmd)
{
	static TIrCmd oldCmd = cmdNone;
	if(cmd == cmdRepeat)
		cmd = oldCmd;
	else
		oldCmd = cmd;
	
	switch(cmd)
	{
		case cmdRepeat:		USART_Transmit_str("cmdRepeat\n");	break;
		case cmdChMinus:	USART_Transmit_str("cmdChMinus\n");	decValueR();	break;
		case cmdCh:			USART_Transmit_str("cmdCh\n");		incValueR();	break;
		case cmdChPlus:		USART_Transmit_str("cmdChPlus\n");	switchValueR();	break;
		case cmdPrev:		USART_Transmit_str("cmdPrev\n");	decValueG();	break;
		case cmdNext:		USART_Transmit_str("cmdNext\n");	incValueG();	break;
		case cmdPlay:		USART_Transmit_str("cmdPlay\n");	switchValueG();	break;
		case cmdVolMinus:	USART_Transmit_str("cmdVolMinus\n"); decValueB();	break;
		case cmdVolPlus:	USART_Transmit_str("cmdVolPlus\n");	incValueB();	break;
		case cmdEq:			USART_Transmit_str("cmdEq\n");		switchValueB();	break;
		case cmd0:			USART_Transmit_str("cmd0\n");		setPaletteMode(1);	break;
		case cmd100:		USART_Transmit_str("cmd100\n");		switchAllMemo(); break;
		case cmd200:		USART_Transmit_str("cmd200\n");		switchAll();	break;
		case cmd1:			USART_Transmit_str("cmd1\n");		setPaletteMode(2);	break;
		case cmd2:			USART_Transmit_str("cmd2\n");		setPaletteMode(3);	break;
		case cmd3:			USART_Transmit_str("cmd3\n");		setPaletteMode(4);	break;
		case cmd4:			USART_Transmit_str("cmd4\n");		setPaletteMode(5);	break;
		case cmd5:			USART_Transmit_str("cmd5\n");		setPaletteMode(6);	break;
		case cmd6:			USART_Transmit_str("cmd6\n");		setPaletteMode(7);	break;
		case cmd7:			USART_Transmit_str("cmd7\n");		setPaletteMode(8);	break;
		case cmd8:			USART_Transmit_str("cmd8\n");		setPaletteMode(9);	break;
		case cmd9:			USART_Transmit_str("cmd9\n");		setPaletteMode(10);	break;
		
		case cmdNone:
		default:
			USART_Transmit_len((char*)&cmd,sizeof(cmd));
			USART_Transmit('\n');
	}
	saveRGBSettings();
}

const uint8_t oneStepDelay = 4;				//продолжительность паузы основного цикла программы
const uint8_t commandCheckStepsPeriod = 13;	//период проверки (в циклах) наличия полученных ИК-команд

int main(void)
{
	init();				//инициализация железа
	loadRGBSettings();	//восстановить настройки отображения из памяти

	sei();
	
	USART_Transmit_str("Ok\n");
	
	uint8_t cmdCheck = 0;
    while(1)
    {
		//отладочный меандр
		heartbeat();
		_delay_ms(oneStepDelay);
		
		//проверка наличия команды
		if(!cmdCheck)
		{
			cmdCheck = commandCheckStepsPeriod;
			
			uint16_t data = getIrCmd();
			if(data)
				ParseCmd((TIrCmd)data);
		}
		cmdCheck--;
		
		//проверка необходимости сменить паллитру и ее смена
		checkAndChangePalette();
    }
}

//на фронте сигнала смотрим сколько отсчетов таймера
//прошло с прошлого фронта, на основании этой информации
//декодируем полученную ИК-команду
ISR(INT0_vect)
{
	uint8_t iRdelay = TCNT2;
	TCNT2 = 0;
	
	irSignalFront(iRdelay);
}

//сброс данных ИК-приемника при переполнении таймера
//при получении каждого бита счетчик сбрасывается в ноль
//если счетчик переполнен, значит ждать уже бесполезно
ISR(TIMER2_OVF_vect)
{
	irSignalTimeout();
}