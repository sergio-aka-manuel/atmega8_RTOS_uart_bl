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



//-----------------------------------------------------------------------------------
/**
 * MSG_DEFAULT_TASK Handler
 */
uint8_t defaultTask(msg_par par)
{
 
    return 1;
}

/**
 * MSG_LED1_CONTROL Handler
 */
uint8_t led1_ControlTask(msg_par par)
{
    PORTC ^= 0x20;
    
    return 1;
}

/**
 * MSG_LED2_CONTROL Handler
 */
uint8_t led2_ControlTask(msg_par par)
{
    PORTC ^= 0x10;
    
    return 1;
}

/**
 * MSG_LED3_CONTROL Handler
 */
uint8_t led3_ControlTask(msg_par par)
{
    PORTC ^= 0x08;
    
    return 1;
}

int main(void) {
    /**[1] 11.9.2 WDTCSR – Watchdog Timer Control Register
     * WDCE: Watchdog Change Enable
     *          This bit is used in timed sequences for changing WDE and prescaler bits.
     *          To clear the WDE bit, and/or changethe prescaler bits, WDCE must be set.
     *          Once written to one, hardware will clear WDCE after four clock cycles.
     * WDE: Watchdog System Reset Enable
     *          WDE is overridden by WDRF in MCUSR. This means that WDE is always set when WDRF is set.
     *          To clear WDE, WDRF must be cleared first. This feature ensures multiple resets during
     *          conditions causing failure, and a safe start-up after the failure.
     * WDP[3:0]: Watchdog Timer Prescaler 3, 2, 1 and 0
     *          The WDP[3:0] bits determine the Watchdog Timer prescaling when the Watchdog
     *          Timer is running. The different prescaling values and their corresponding
     *          time-out periods are shown in Table:
     *
     *          WDP3    WDP2    WDP1    WDP0    WDT                     Time-out
     *          0       0       0       0       2K (2048) cycles        16ms
     *          0       0       0       1       4K (4096)               32ms
     *          0       0       1       0       8K (8192)               64ms
     *          0       0       1       1       16K (16384)             0.125s
     *          0       1       0       0       32K (32768)             0.250s
     *          0       1       0       1       64K (65536)             0.500s
     *          0       1       1       0       128K (131072)           1.0 s
     *          0       1       1       1       256K (262144)           2.0 s
     *          1       0       0       0       512K (524288)           4.0 s
     *          1       0       0       1       1024K (1048576)         8.0 s
     */

    //WDTCSR = _BV(WDE) | _BV(WDCE);
    //WDTCSR = _BV(WDE) | (0x00) /* ~15ms */;

    /* GPIO: init */
    //DDRB |= 0xFF;

    PORTC &= ~(0x38);    
    DDRC |= 0x38;

//
//    while (1) {
//        PORTC |= 0x20; 
//        PORTC &= ~0x10;
//        PORTB |= 0x80;
//        _delay_ms(1);
//
//        PORTC &= ~0x20;
//        PORTC |= 0x10;
//        PORTB &= ~0x80;
//        _delay_ms(1);
//    }
    
    
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

    
    for (;;)
    {
        dispatchMessage();
        //wdt_reset();
    }   
}
