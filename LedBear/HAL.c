/*
 * HAL.c
 *
 * Created: 08.03.2015 22:32:36
 *  Author: Stanislav Zhelnio
 *
 * �������� ��� ���������� RGB ������������ ������ c IR ������
 * ���������� ��������� �� ���� atmega88pa + 8��� �����
 * ����: PD2 (INT0) - � ��-��������� TSOP17
 * ������ ��� (RGB): PD6, PD5, PB1
 *
 * ������ HAL
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

//������������� ���������
inline void init(void)
{
	//��������� ����� � ��� �� �����
	DDRD = (1 << DDD5) | (1 << DDD6);
	DDRB = (1 << DDB1);
	
	//debug
	DDRB |= (1 << DDB3);
	
	//�������� �� IR � ���� �� ���� ����������� ������������
	PORTD = (1 << PORTD2);
	PORTB = 0;

	//UART
	UBRR0 = 12;								//������ �������� (9600, �� �������, �� ������� �� ��������� ��������)
	UCSR0A = (1 << U2X0);					//��������� �������� �������� (�������)
	UCSR0B = (1 << TXEN0);					//��������� ��������
	UCSR0C = (1 << UCSZ00)| (1 << UCSZ01);	// �����������, ��� ���� ��������, 1 ����, 8 ��� ��������,
	
	//IR detector init
	EICRA = (1 << ISC01);
	EIMSK = (1 << INT0);
	
	//������ ��� ���������� ������� ��������
	TCCR2A = 0;
	TCCR2B = (1 << CS22)| (1 << CS20);
	TIMSK2 = (1 << TOIE2);
	
	//������� ��� ��� �� LED
	TCCR0A = (1 << WGM00) | (1 << WGM01);
	TCCR0B = /*(1 << CS00) |*/ (1 << CS01);
	OCR0A = 0;
	OCR0B = 0;
	
	TCCR1A = (1 << WGM10);
	TCCR1B = (1 << WGM12) |/* (1 << CS10) | */(1 << CS11);
	OCR1A = 0;
}

//���������� ������������ �����
inline void heartbeat(void)
{
	PORTB ^= (1 << PORTB3);
}

//***************** ���������� ������� ���������� ������� R *****************
//�������� ����� �� ��������
inline void setFullR(void)
{
	OCR0A = 255;
	TCCR0A &= ~(1 << COM0A1);
	PORTD |= (1 << PORTD6);
}

//��������� �����
inline void setOffR(void)
{
	OCR0A = 0;
	TCCR0A &= ~(1 << COM0A1);
	PORTD &= ~(1 << PORTD6);
}

//���������� �������� ������� ������ (���������� ���)
inline void setValR(uint8_t val)
{
	OCR0A = val;
	TCCR0A |= (1 << COM0A1);
}

//***************** ���������� ������� ���������� ������� G ****************
//�������� ����� �� ��������
inline void setFullG(void)
{
	OCR0B = 255;
	TCCR0A &= ~(1 << COM0B1);
	PORTD |= (1 << PORTD5);
}

//��������� �����
inline void setOffG(void)
{
	OCR0B = 0;
	TCCR0A &= ~(1 << COM0B1);
	PORTD &= ~(1 << PORTD5);
}

//���������� �������� ������� ������ (���������� ���)
inline void setValG(uint8_t val)
{
	OCR0B = val;
	TCCR0A |= (1 << COM0B1);
}

//***************** ���������� ������� ���������� ������� B ****************
//�������� ����� �� ��������
inline void setFullB(void)
{
	OCR1AL = 255;
	TCCR1A &= ~(1 << COM1A1);
	PORTB |= (1 << PORTB1);
}

//��������� �����
inline void setOffB(void)
{
	OCR1AL = 0;
	TCCR1A &= ~(1 << COM1A1);
	PORTB &= ~(1 << PORTB1);
}

//���������� �������� ������� ������ (���������� ���)
inline void setValB(uint8_t val)
{
	OCR1AL = val;
	TCCR1A |= (1 << COM1A1);
}

//***************** ������� �������� ������ �� USART *****************
void USART_Transmit(char data )
{
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = data;					
}

void USART_Transmit_len(char *data, uint8_t len)
{
	while(len--)
		USART_Transmit(*(data++));
}

void USART_Transmit_str(char *data)
{
	while(*data)
		USART_Transmit(*(data++));
}