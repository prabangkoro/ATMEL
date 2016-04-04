/*
 * outputPWM.cpp
 *
 * Created: 3/31/2016 12:41:05 PM
 *  Author: Babang
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define ADCPin		0

ISR(TIMER0_COMPA_vect){
	PORTB = _BV(PORTB0);
}

void ADC_Init(void){
	// 5v as reference, ADC0 as Input
	ADMUX = (1<<REFS0);
	// ADC enable feature
	// prescaller 128 : F_ADC = 125kHz
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(ADPS0);
}

int ADC_Read(unsigned char _ADCPIN){
	// bitmask
	_ADCPIN &= 0x07;
	// select pin
	ADMUX = (ADMUX&0x07)|_ADCPIN;
	
	// start single convertion
	ADCSRA |= (1<<ADSC);
	// wait until reading complete
	while(ADCSRA & (1<<ADSC));
	
	// return read value
	return ADC;
}

void PWM_Init(void){
	// using OC1 as pwm pin : D9 / PB1 : fast pwm
	// using digital pin PB0 : Phase correct pwm
	DDRB |= (1<<DDB1)|(1<<DDB0);
	
	// fast pwm mode 5
	// top : 0xFF , non-inverting, prescaller 1024
	TCCR1A |= (1<<COM1A1)|(1<<WGM10);
	TCCR1B |= (1<<WGM12)|(1<<CS12)|(1<<CS10);
	
	// Phase Correct-pwm , mode 1
	// non-inverting, prescaller 1024
	TCCR0A |= (1<<WGM00);
	TCCR0B |= (1<<CS02)|(1<<CS00);
	
}

void PWM_Adjust(int _adc){
	
	// calculate duty cycle
	float DC = _adc/1023.0*100.0;
	
	// PWM for PB1 : Fast-PWM 
	// TOP = 0xFF
	// Prescaller 1024, non-inverting
	OCR1A = (int)(DC*(0xFF+1)/100 - 1);
	
	// PWM for PB0 : Phase Correct-PWM
	OCR0A = (int)(DC*0xFF/100.0);
	
}

int main(void)
{
	ADC_Init();
	PWM_Init();
	int ADCReadValue;
	while(1)
	{
	//TODO:: Please write your application code 
		ADCReadValue = ADC_Read(ADCPin);
		PWM_Adjust(ADCReadValue);
		_delay_ms(100);
	}
}

