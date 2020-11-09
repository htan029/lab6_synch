/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://youtu.be/S61b5_OuOmQ
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

enum States {Start, Init, Wait, Increment, Decrement, Wait2, Wait3, Reset} state;

unsigned char cnt;
unsigned char contPress;

void Tick(){
    switch(state){
        case Start: PORTB = cnt = 0x07; state = Wait; break;
        case Wait: 
            if(((~PINA) & 0x03) == 0x03){  
                PORTB = cnt = 0x00;
                state = Reset;
            } else if (((~PINA) & 0x03) == 0x01){
                state = Increment;
            } else if (((~PINA) & 0x03) == 0x02){
                state = Decrement;
            }
            contPress = 0;
            break;
        case Increment: state = Wait2; break;
        case Decrement: state = Wait3; break;
        case Wait2:
            if(((~PINA) & 0x03) == 0x03){
                state = Reset;
            } else if (((~PINA) & 0x03) == 0x01) {
                contPress++;
                if(contPress == 10){
                    contPress = 0;
                    state = Increment;
                }
            } 
            else state = Wait;
            break;
        case Wait3:
            if(((~PINA) & 0x03) == 0x03){
                state = Reset;
            } else if(((~PINA) & 0x03) == 0x02) {
                contPress++;
                if(contPress == 10){
                    contPress = 0;
                    state = Decrement;
                }
            }
            else state = Wait;
            break;
        case Reset:
            if(((~PINA) & 0x03) == 0x03) PORTB = cnt = 0x00;
            else state = Wait;
            break;
        default: break;
    };

    switch (state){
        case Start: break;
        case Wait: break;
        case Increment: //PORTC = cnt = (cnt == 0x09) ? 0x09 : cnt+1; break;
            if(cnt == 0x09){
                cnt = 0x09;
                PORTB = cnt;
            } else {
                cnt++;
                PORTB = cnt;
            }
            break;
        case Decrement: //PORTC = cnt = (cnt == 0x00) ? 0x00 : cnt-1; break;
            if(cnt == 0x00){
                cnt = 0x00;
                PORTB = cnt;
            } else {
                cnt--;
                PORTB = cnt;
            }
            break;
        case Wait2: break;
        case Wait3: break;
        default: break;
    };
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    state = Start;
    unsigned char tmpB = 0x00;
    TimerSet(100);
    TimerOn();
    /* Insert your solution below */
    while (1) {
        Tick();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}
