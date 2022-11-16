/*
 * Mini_Project2.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Abdelrahman Ali Mohamed Ali
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Global Variables //
unsigned char g_sec1 = 0;
unsigned char g_sec2 = 0;
unsigned char g_min1 = 0;
unsigned char g_min2 = 0;
unsigned char g_hour1 = 0;
unsigned char g_hour2 = 0;
unsigned char flag = 0;

// Function Prototypes //
void Timer1_init();
void INT0_init_Reset();
void INT1_init_Pause();
void INT2_init_Resume();
void ioInitialization();
void handleDisplay();
void displayStopWatch ();


int main(void) {

	ioInitialization();           //initialization of ports
	INT0_init_Reset();  		 //interrupt0 initialization
	INT1_init_Pause();   		//interrupt1 initialization
	INT2_init_Resume();  	   //interrupt2 initialization
	Timer1_init();            //initialization of timer1


	while (1) {
		if ( flag == 1 ){
			handleDisplay();
			flag = 0 ;
		}
		displayStopWatch ();
	}
}

// ISR of timer1 //
ISR(TIMER1_COMPA_vect) {
	flag = 1 ;
}

// ISR of interrupt0 //
ISR(INT0_vect) {      // reseting all the variables values to 0 to reset the stop watch
		g_sec1 = 0 ;
		g_sec2 = 0 ;
		g_min1 = 0 ;
		g_min2 = 0 ;
		g_hour1 = 0 ;
		g_hour2 = 0 ;
}

// ISR of interrupt1 //
ISR(INT1_vect) {
	TCCR1B &= ~(1<<CS10) &~(1<<CS11) &~(1<<CS12); // Clock disabled
}

// ISR of interrupt2 //
ISR(INT2_vect) {
		TCCR1B |= (1<<CS10) | (1<<CS12) ; // Clock enabled again
}

// Initialization of timer1 //
void Timer1_init() {
	TCNT1 = 0;     // initial value to count from
	OCR1A = 977;  // compare value to compare with to get a time of ~ 1 sec
	TCCR1A = (1 << FOC1A) ;  //enabled because it's not PWM mode
	TCCR1B = (1 << CS10) | (1 << CS12)| (1 << WGM12);  // enable CTC ( compare mode with prescalar 1024 )
	TIMSK |= (1 << OCIE1A); // Output Compare A Match Interrupt Enable
	SREG = (1 << 7);  // enable global interrupts
}

// Initialization of interrupt0 //
void INT0_init_Reset() {
	DDRD &= ~(1 << PD2);        // PD2 is input pin
	PORTD |= (1 << PD2);       // enable internal pull up
	MCUCR |= (1 << ISC01);    //falling edge of INT0
	GICR |= (1 << INT0);     // INT0 enable
	SREG |= (1 << 7);       //global interrupt enable

}

// Initialization of interrupt1 //
void INT1_init_Pause() {
	DDRD &= ~(1 << PD3);                    // PD3 is input pin
	MCUCR |= (1 << ISC11) | (1 << ISC10);  //rising edge of INT1
	GICR |= (1 << INT1);                  // INT1 enable
	SREG |= (1 << 7);                    //global interrupt enable

}

// Initialization of interrupt2 //
void INT2_init_Resume() {
	DDRB &= ~(1 << PB2);        // PB2 is input pin
	PORTB |= (1 << PB2);       // enable internal pull up
	MCUCSR &= ~(1 << ISC2);   //falling edge of INT2
	GICR |= (1 << INT2);  	 // INT2 enable
	SREG |= (1 << 7);       //global interrupt enable

}

void ioInitialization() {
	DDRC |= 0x0F;      //First 4 pins of PortC are output pins
	DDRA |= 0x3F;     //First 6 pins of PortA are output pins ( Enable and Disable )
	PORTC &= 0xF0;   // show 0 at first
}

// Function to handle the time shown on the seven segment //
void handleDisplay(){
		g_sec1++;
		if (g_sec1 > 9) {
			g_sec1 = 0;
			g_sec2++;
		}
		if (g_sec2 > 5) {
			g_sec2 = 0;
			g_min1++;
		}
		if (g_min1 > 9) {
			g_min1 = 0;
			g_min2++;
		}
		if (g_min2 > 5) {
			g_min2 = 0;
			g_hour1++;
		}
		if (g_hour1 > 9) {
			g_hour1 = 0;
			g_hour2++;
		}
		if ( (g_hour2 == 2) && (g_hour1 == 4) ) {
			g_hour1 = 0;
			g_hour2 = 0;
		}
}

// Function to control the display of each seven segment using multiplexed method //
void displayStopWatch (){

	PORTA = (PORTA & 0xC0) | (1<<PA0);
	PORTC = (PORTC & 0xF0) | (g_sec1);
	_delay_us(500);
	PORTA = (PORTA & 0xC0) | (1<<PA1);
	PORTC = (PORTC & 0xF0) | (g_sec2);
	_delay_us(500);
	PORTA = (PORTA & 0xC0) | (1<<PA2);
	PORTC = (PORTC & 0xF0) | (g_min1);
	_delay_us(500);
	PORTA = (PORTA & 0xC0) | (1<<PA3);
	PORTC = (PORTC & 0xF0) | (g_min2);
	_delay_us(500);
	PORTA = (PORTA & 0xC0) | (1<<PA4);
	PORTC = (PORTC & 0xF0) | (g_hour1);
	_delay_us(500);
	PORTA = (PORTA & 0xC0) | (1<<PA5);
	PORTC = (PORTC & 0xF0) | (g_hour2);
	_delay_us(500);

}
