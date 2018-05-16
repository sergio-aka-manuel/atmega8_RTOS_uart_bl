/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: sa100
 *
 * Created on May 14, 2018, 11:23 AM
 */

#include "messages_disp.h"      /* RTOS: dispatcher */
#include "main.h"



//------------------------------------------------------------------------------

/**
 * MSG_DEFAULT_TASK Handler
 */
uint8_t defaultTask(msg_par par) {

    return 1;
}

/**
 * MSG_LED1_CONTROL Handler
 */
uint8_t led1_ControlTask(msg_par par) {
    PORTC ^= 0x20;

    return 1;
}

/**
 * MSG_LED2_CONTROL Handler
 */
uint8_t led2_ControlTask(msg_par par) {
    PORTC ^= 0x10;

    return 1;
}

/**
 * MSG_LED3_CONTROL Handler
 */
uint8_t led3_ControlTask(msg_par par) {
    PORTC ^= 0x08;

    return 1;
}

int main(void) {
    /* GPIO: init */
    //DDRB |= 0xFF;

    PORTC &= ~(0x39);
    DDRC |= 0x39;

    /** Watchdog Timer Control Register – WDTCR
     * 2486AA–AVR–02/2013 ATmega8/L datasheet pp. 43-45
     */
    wdt_reset();
    WDTCR = _BV(WDCE) | _BV(WDE);
    WDTCR |= _BV(WDP0); //32K oscillator cycles

    if (MCUCSR & _BV(WDRF) /*WDRF: Watchdog Reset Flag*/) {
        MCUCSR &= ~(_BV(WDRF)); //FIXME!: reset flag?
        PORTC |= 0x01; //error led
    }

    /* USART: init */
    UBRRH = HI(UBRR); //set baud rate
    UBRRL = LO(UBRR);
    
    
    UCSRB |= _BV(RXEN) | _BV(TXEN) | _BV(RXCIE); //enable rx, tx and rx complete interrupt
    UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0); //8bits data, parity none, 1bit stop
    
    
    /* RTOS: init */
    initMessages();

    /* first task */
    setHandler(MSG_LED1_CONTROL, &led1_ControlTask);
    setTimer(MSG_LED1_CONTROL, 0, 100/*ms*/);

    /* second task */
    setHandler(MSG_LED2_CONTROL, &led2_ControlTask);
    setTimer(MSG_LED2_CONTROL, 0, 300/*ms*/);

    /* third task */
    setHandler(MSG_LED3_CONTROL, &led3_ControlTask);
    setTimer(MSG_LED3_CONTROL, 0, 700/*ms*/);

    /* enable interrupts */
    sei();

    /* static led */
    //PORTC |= 0x08;


    for (;;) {
        dispatchMessage();
        wdt_reset();

        /** TODO:
         * - UART
         * - EEPROM 
         * - bootloader (UART)
         * - ...
         */
    }
}

/**
 * RXCIE: USART RX Complete Interrupt handler
 */
ISR(USART_RXC_vect) {
    uint8_t buf = UDR;
    UDR = buf;
    
    //TODO: ...
    UCSRB |= _BV(UDRIE); //enable: USART Data Register Empty Interrupt
}

/**
 * UDRIE: USART Data Register Empty Interrupt handler
 */
ISR(USART_UDRE_vect) {
    //TODO: ...
    UCSRB &= ~(_BV(UDRIE)); //disable interrupt
}
