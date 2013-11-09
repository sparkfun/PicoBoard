#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <math.h>
#include "main.h"
#include "adc.h"

//================================================================
//Define Global Variables
//================================================================
char channel[8]={RD, RC, RB, BUTTON, RA, LIGHT, SOUND, SLIDER};

//================================================================


int main (void)
{	
	char request=0;
	unsigned int sensor_value=0;
	char data_packet[2]="";

	//Initialize AVR I/O, UART and Interrupts
    ioinit();
	//Initialize the ADC
	adcInit(1,0,7);	//AVcc reference, Right Adjusted, 125kHz clock
	
	printf("Start\n");
	while(1){

		request=uart_getchar();
		
		//If we get the value 1 on the serial port, send the sensor values.
		if(request==SCRATCH_DATA_REQUEST)
		{	
			//Send the ID packet
			buildScratchPacket(data_packet, 15, 0x04);
			sendScratchPacket(data_packet);
			
			//Read/Report channel 0 (Resistance-D)
			sensor_value=adcRead(RD);
			buildScratchPacket(data_packet, 0, sensor_value);
			sendScratchPacket(data_packet);
			
			//Read/Report Channel 1 (Resistance-C)
			sensor_value=adcRead(RC);
			buildScratchPacket(data_packet, 1, sensor_value);
			sendScratchPacket(data_packet);

			//Read/Report Channel 2 (Resistance-B)
			sensor_value=adcRead(RB);
			buildScratchPacket(data_packet, 2, sensor_value);
			sendScratchPacket(data_packet);			

			//Read/Report Channel 3 (Button)
			if(PIND & (1<<BUTTON))sensor_value=0xffff;
			else sensor_value=0;
			buildScratchPacket(data_packet, 3, sensor_value);
			sendScratchPacket(data_packet);	
			
			//Read/Report Channel 4(Resistance-A)
			sensor_value=adcRead(RA);
			buildScratchPacket(data_packet, 4, sensor_value);
			sendScratchPacket(data_packet);		

			//Read/Report Channel 5(LIGHT)
			sensor_value=adcRead(LIGHT);
			buildScratchPacket(data_packet, 5, sensor_value);
			sendScratchPacket(data_packet);	

			//Read/Report Channel 6(Sound)
			sensor_value=adcRead(SOUND);
			buildScratchPacket(data_packet, 6, sensor_value);
			sendScratchPacket(data_packet);			
			
			//Read/Report Channel 7(Slider)
			sensor_value=adcRead(SLIDER);
			sensor_value=abs(1023-sensor_value);
			buildScratchPacket(data_packet, 7, sensor_value);
			sendScratchPacket(data_packet);		
			
		}
	}
	
	
    return (0);
}

//==================================================
//Core functions
//==================================================
//Function Description: 
//Inputs: 
//Outputs:
//Return:      
//usage: 
void ioinit(void)
{
	//All of Port C are inputs
	DDRC &= ~((1<<SLIDER)|(1<<RD)|(1<<RC)|(1<<SOUND)|(1<<RB)|(1<<RA)|(1<<LIGHT));
	//Don't enable any pull-ups!

	//Initialize Port D
	DDRD &= ~((1<<BUTTON)|(1<<RXI));	//Button and Rx pin are inputs
	PORTD |= (1<<BUTTON)|(1<<RXI);		//Enable the pull-ups for the Rx and Button pins

	// USART Baud rate: 38400 (With 16 MHz Clock)
    UBRR0H = (MYUBRR >> 8) & 0x7F;	//Make sure highest bit(URSEL) is 0 indicating we are writing to UBRRH
	UBRR0L = MYUBRR;
    UCSR0A = (1<<U2X0);					//Double the UART Speed
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);		//Enable Rx and Tx in UART
    UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);		//8-Bit Characters
    stdout = &mystdout; //Required for printf init
	cli();
	
	// Init timer 2
	//Set Prescaler to 8. (Timer Frequency set to 16Mhz)
	//Used for delay routines
	TCCR2B = (1<<CS20); 	//Divde clock by 1 for 16 Mhz Timer 2 Frequency	
		
}


static int uart_putchar(char c, FILE *stream)
{
  if (c == '\n')
    uart_putchar('\r', stream);
  
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

uint8_t uart_getchar(void)
{
    while( !(UCSR0A & (1<<RXC0)) );
	return(UDR0);
}

//General short delays
void delay_ms(uint16_t x)
{
    for (; x > 0 ; x--)
        delay_us(1000);
}

//General short delays
void delay_us(uint16_t x)
{    
    TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
    TCNT2= 240; //Setting counter to 239 will make it 16 ticks until TOV is set. .0625uS per click means 1 uS until TOV is set
    while(x>0){
		while( (TIFR2 & (1<<TOV2)) == 0);
		TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
		TCNT2=240;
		x--;
	}
} 

void buildScratchPacket(char * packet, int channel, int value){
	char upper_data=(char)((value&(unsigned int)0x380)>>7);	//Get the upper 3 bits of the value
	char lower_data=(char)(value&0x7f);	//Get the lower 7 bits of the value
	*packet++=((1<<7)|(channel<<3)|(upper_data));
	*packet++=lower_data;
}

void sendScratchPacket(char * packet){
	printf("%c", *packet++);
	delay_us(400);
	printf("%c", *packet++);
	delay_us(400);
}
