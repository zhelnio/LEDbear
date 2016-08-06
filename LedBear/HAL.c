/*
 * HAL.c
 *
 * Created: 08.03.2015 22:32:36
 *  Author: Stanislav Zhelnio
 *
 * прошивка для управления RGB светодиодной лентой c IR пульта
 * устройство построено на базе atmega88pa + 8МГц кварц
 * вход: PD2 (INT0) - с ИК-приемника TSOP17
 * выходы ШИМ (RGB): PD6, PD5, PB1
 *
 * модуль HAL
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

//инициализация переферии
inline void init(void)
{
	//настроить порты с ШИМ на выход
	DDRD = (1 << DDD5) | (1 << DDD6);
	DDRB = (1 << DDB1);
	
	//debug
	DDRB |= (1 << DDB3);
	
	//подтяжка на IR и ноль на базы управляющих транзисторов
	PORTD = (1 << PORTD2);
	PORTB = 0;

	//UART
	UBRR0 = 12;								//задаем скорость (9600, по таблице, из расчета на множитель скорости)
	UCSR0A = (1 << U2X0);					//множитель скорости передачи (удвоить)
	UCSR0B = (1 << TXEN0);					//разрешаем передачу
	UCSR0C = (1 << UCSZ00)| (1 << UCSZ01);	// асинхронный, без бита четности, 1 стоп, 8 бит передача,
	
	//IR detector init
	EICRA = (1 << ISC01);
	EIMSK = (1 << INT0);
	
	//таймер для вычисления периода импульса
	TCCR2A = 0;
	TCCR2B = (1 << CS22)| (1 << CS20);
	TIMSK2 = (1 << TOIE2);
	
	//таймеры для ШИМ на LED
	TCCR0A = (1 << WGM00) | (1 << WGM01);
	TCCR0B = /*(1 << CS00) |*/ (1 << CS01);
	OCR0A = 0;
	OCR0B = 0;
	
	TCCR1A = (1 << WGM10);
	TCCR1B = (1 << WGM12) |/* (1 << CS10) | */(1 << CS11);
	OCR1A = 0;
}

//отладочное подергивание ногой
inline void heartbeat(void)
{
	PORTB ^= (1 << PORTB3);
}

//***************** аппаратные функции управления каналом R *****************
//включить канал на максимум
inline void setFullR(void)
{
	OCR0A = 255;
	TCCR0A &= ~(1 << COM0A1);
	PORTD |= (1 << PORTD6);
}

//выключить канал
inline void setOffR(void)
{
	OCR0A = 0;
	TCCR0A &= ~(1 << COM0A1);
	PORTD &= ~(1 << PORTD6);
}

//установить заданную яркость канала (скважность ШИМ)
inline void setValR(uint8_t val)
{
	OCR0A = val;
	TCCR0A |= (1 << COM0A1);
}

//***************** аппаратные функции управления каналом G ****************
//включить канал на максимум
inline void setFullG(void)
{
	OCR0B = 255;
	TCCR0A &= ~(1 << COM0B1);
	PORTD |= (1 << PORTD5);
}

//выключить канал
inline void setOffG(void)
{
	OCR0B = 0;
	TCCR0A &= ~(1 << COM0B1);
	PORTD &= ~(1 << PORTD5);
}

//установить заданную яркость канала (скважность ШИМ)
inline void setValG(uint8_t val)
{
	OCR0B = val;
	TCCR0A |= (1 << COM0B1);
}

//***************** аппаратные функции управления каналом B ****************
//включить канал на максимум
inline void setFullB(void)
{
	OCR1AL = 255;
	TCCR1A &= ~(1 << COM1A1);
	PORTB |= (1 << PORTB1);
}

//выключить канал
inline void setOffB(void)
{
	OCR1AL = 0;
	TCCR1A &= ~(1 << COM1A1);
	PORTB &= ~(1 << PORTB1);
}

//установить заданную яркость канала (скважность ШИМ)
inline void setValB(uint8_t val)
{
	OCR1AL = val;
	TCCR1A |= (1 << COM1A1);
}

//***************** Функция передачи данных по USART *****************
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