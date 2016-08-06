/*
 * irRemote.c
 *
 * Created: 09.03.2015 11:39:01
 * �������� ��� ���������� RGB ������������ ������ c IR ������
 * ���������� ��������� �� ���� atmega88pa + 8��� �����
 * ����: PD2 (INT0) - � ��-��������� TSOP17
 * ������ ��� (RGB): PD6, PD5, PB1
 *
 * ������ ������������� ��-������
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

#define FIFO_SIZE	16
uint16_t fifo[FIFO_SIZE];
int8_t fifoIndex = -1;

//��������� �������� ������� � ������� �� ���������
void putIrCmd(uint16_t data)
{
	if(fifoIndex < FIFO_SIZE - 1)
		fifo[++fifoIndex] = data;
}

//�������� ������� �� ������� �������� ������
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

//���-�� �������� ��� + 1
uint8_t volatile receiveMode = 0;

//��������� ����������� ���� �������
inline void irSignalFront(uint8_t period)
{
	static uint16_t irData = 0;
	
	//������� ������ ���������� �������
	if(!receiveMode && (period >= 0xE2 && period <= 0xE9))
	{
		putIrCmd(0xFF);
		return;
	}
	
	//�������� ������ ������� 
	if(!receiveMode && period >= 0x5D && period <= 0x70)
	{
		receiveMode = 1;
		irData = 0;
		return;
	}
	
	//������� ��������� ��� �������
	if(receiveMode)
	{
		irData = irData << 1;
		if(period >= 0x0F && period <= 0x12)
			irData++;
		receiveMode++;
	}
	
	//������� ��������� ��� �������
	if(receiveMode == 33)
	{
		putIrCmd(irData);
		receiveMode = 0;
	}
}

//����� ������ ������ 
//���������� � ���������� ��� ������������ �������� �������
inline void irSignalTimeout(void)
{
	if(receiveMode)
	{
		receiveMode = 0;
		USART_Transmit('\n');
	}
}