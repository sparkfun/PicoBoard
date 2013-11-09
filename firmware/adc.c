//ADC Library for the AtMega368
//Written by Ryan Owens

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include "adc.h"
#include "main.h"

//Function: adcRead(char) - Reads the ADC count value of a specified ADC channel
//Inputs: char channel - The ADC channel to be read
//Outputs: None
//Return: The adc count of the channel
//Usage: adcRead(4);
unsigned int adcRead(char channel)
{
	unsigned int adc_value=0;
	
	ADMUX = (ADMUX & 0xF0);	//Mask OFF the previous ADC channel
	ADMUX |= (channel & 0x0F);		//Set the new ADC channel
	//ADCSRA |= ((1<<ADEN)|(1<<ADSC));	//Set the ADC enable and start bit to start the conversion
													//Keep the ADC clock the same.
	sbi(ADCSRA, ADSC);	//Start a conversion
	//while(ADCSRA & (1<<ADSC));	//Wait for the conversion to finish
	while(bit_is_set(ADCSRA, ADSC));
	
	adc_value=ADCL;				//Get the lowest 8 bits of the 10 bit conversion
	adc_value |= (ADCH << 8);	//Get the upper 2 bits of the 10 bit conversion
	
	return adc_value;			//Send the value back to the calling function
}

//Function: adcVoltage(unsigned int) - Converts an ADC count to millivolts assuming a 5V reference voltage
//Inputs: unsigned int adc_value - The ADC count to be converted
//Outputs: None
//Return: The voltage in millivolts of the ADC count
//Usage: adcVoltage(3500);
unsigned long adcVoltage(unsigned int adc_value)
{
	unsigned long voltage=0;
	
	//Convert the ADC count to a voltage in mV assuming the reference voltage is 5 V
	voltage = (adc_value * 5000);
	voltage = voltage/1024;
	
	return voltage;
	
}

//Function: adcInit(char, char, char) - Initializes the Analog to Digital Converter with the input settings
//Inputs: reference - The reference voltage for the ADC (0, 1 or 3 accepted)
//		  align - The desired adjustment for the ADC result (0 or 1 accepted)
//		  clock - The division factor for to produce the ADC clock (0 - 7 accepted)
//Outputs: None
//Return: None
//Usage: adcInit(3, 0, 7);
void adcInit(char reference, char align, char clock)
{
	ADMUX = (reference << REFS0);	//Shift the reference voltage into the ADMUX register
	ADMUX |= (align << ADLAR);		//Shift the left adjust into the ADMUX register
	
	//ADCSRA = clock;		//Set the ADC clock prescaler
	
	// set a2d prescale factor to 128
	// 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
	// XXX: this will not work properly for other clock speeds, and
	// this code should use F_CPU to determine the prescale factor.
	sbi(ADCSRA, ADPS2);
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);

	// enable a2d conversions
	sbi(ADCSRA, ADEN);	
}