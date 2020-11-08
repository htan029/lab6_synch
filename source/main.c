/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://youtu.be/l3s_Drg0-3s
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff(){
    TCCR1B = 0x00;
}

void TimerISR(){
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start, Lit, Wait} state;

unsigned char lights[4] = {0x01, 0x02, 0x04, 0x02};
unsigned char cnt;
unsigned char press;

void Tick(){
    switch(state){
        case Start: cnt = 0; state = Lit; break;
        case Lit: 
            if((~PINA) & 0x01)
                state = Wait;
            break;
        case Wait: 
            if((~PINA) & 0x01)
                state = Lit;
            break;
        default: break;
    };

    switch (state){
        case Start: break;
        case Lit:
            PORTB = lights[cnt];
            cnt = (cnt == 3) ? 0 : cnt+1;
            break;
        case Wait:
            break;
        default: break;
    };
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    state = Start;
    TimerSet(300);
    TimerOn();
    /* Insert your solution below */
    while (1) {
        Tick();
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
