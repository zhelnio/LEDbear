/*
 * irRemote.c
 *
 * Created: 09.03.2015 11:39:01
 * прошивка для управления RGB светодиодной лентой c IR пульта
 * устройство построено на базе atmega88pa + 8МГц кварц
 * вход: PD2 (INT0) - с ИК-приемника TSOP17
 * выходы ШИМ (RGB): PD6, PD5, PB1
 *
 * модуль декодирования ИК-команд
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

#define FIFO_SIZE	16
uint16_t fifo[FIFO_SIZE];
int8_t fifoIndex = -1;

//поместить принятую команду в очередь на обработку
void putIrCmd(uint16_t data)
{
	if(fifoIndex < FIFO_SIZE - 1)
		fifo[++fifoIndex] = data;
}

//получить команду из очереди принятых команд
uint16_t getIrCmd()
{
	uint16_t result;
	cli();
	if(fifoIndex < 0)
		result = 0;
	else
		result = fifo[fifoIndex--];
	sei();
	return result;
}

//кол-во принятых бит + 1
uint8_t volatile receiveMode = 0;

//обработка полученного бита сигнала
inline void irSignalFront(uint8_t period)
{
	static uint16_t irData = 0;
	
	//получен повтор предыдущей команды
	if(!receiveMode && (period >= 0xE2 && period <= 0xE9))
	{
		putIrCmd(0xFF);
		return;
	}
	
	//получено начало команды 
	if(!receiveMode && period >= 0x5D && period <= 0x70)
	{
		receiveMode = 1;
		irData = 0;
		return;
	}
	
	//получен очередной бит команды
	if(receiveMode)
	{
		irData = irData << 1;
		if(period >= 0x0F && period <= 0x12)
			irData++;
		receiveMode++;
	}
	
	//получен последний бит команды
	if(receiveMode == 33)
	{
		putIrCmd(irData);
		receiveMode = 0;
	}
}

//сброс приема команд 
//вызывается в прерывании при переполнении счетчика таймера
inline void irSignalTimeout(void)
{
	if(receiveMode)
	{
		receiveMode = 0;
		USART_Transmit('\n');
	}
}