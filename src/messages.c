#include "globals.h"

#include "messages.h"

volatile iTimer lTimer[maxTimers]; 		// список таймеров
volatile iHandler lHandler[maxHandlers];	// список обработчиков
volatile uint8_t numHandlers, numTimers;

volatile iMessage lMessage[maxMessages];	// буфер сообщений
volatile uint16_t lMesPointer, hMesPointer;	// указатели на начало и конец буфера

// установка обработчика события
// вызывается: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (numHandlers < maxHandlers) {
			lHandler[numHandlers].hnd = hnd; // и регистрируем обработчик
			lHandler[numHandlers].msg = msg;
			numHandlers++;
		}
	}
}

// снятие обработчика события
// вызывается: killHandler(MSG_KEY_PRESS, &checkKey);
// удаляет один последний обработчик.
void killHandler(msg_num msg, handler hnd) {
	if (numHandlers==0)
		return;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		int8_t i, j;
		j = 0;
		for (i = numHandlers-1; i >= 0; i--) {

			if ((lHandler[i].msg == msg) && (lHandler[i].hnd == hnd)) {

				// сдвигаем все записи к началу списка, чтобы дырок не было
				for (j = i; j < numHandlers-1 ; j++) {
					lHandler[j].msg = lHandler[j + 1].msg;
					lHandler[j].hnd = lHandler[j + 1].hnd;
				}
				numHandlers--;
				break;
			}
		}

	}
}

// занести событие в очередь
// пример вызова: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hMesPointer = (hMesPointer + 1) & (maxMessages - 1); // сдвигаем указатель головы

		lMessage[hMesPointer].msg = msg; // заносим событие и параметр
		lMessage[hMesPointer].par = par;
		if (hMesPointer == lMesPointer) { // догнали начало очереди, убиваем необработанное сообытие
			lMesPointer = (lMesPointer + 1) & (maxMessages - 1);
		}
	}
}

// установить таймер
// пример вызова: setMsgTimer(MSG_LCD_REFRESH, 0, 50);
void setTimer(msg_num msg, msg_par par, uint16_t time) {
	uint8_t i, firstFree;
	if (time == 0) {
		sendMessage(msg, par);
	} else {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			firstFree = numTimers + 1;

			for (i = 0; i <= numTimers; i++) { // ищем установленный таймер
				if (lTimer[i].msg == 0) {
					firstFree = i;
				} else { // если нашли - обновляем время
					//if ((lTimer[i].msg == msg) && (lTimer[i].par == par)) {
					if (lTimer[i].msg == msg) {
						lTimer[i].time = time;
						lTimer[i].timeOrigin = time;
						lTimer[i].par = par;
						firstFree = maxTimers + 1;
						break;
					}
				}
			}
			if (firstFree <= maxTimers) { // иначе - просто добавляем новый
				lTimer[firstFree].msg = msg;
				lTimer[firstFree].par = par;
				lTimer[firstFree].time = time;
				lTimer[firstFree].timeOrigin = time;

				if (firstFree > numTimers)
					numTimers = firstFree;
			}
		}
	}
}

// убить таймер
// особенность - убивает все установленные таймеры на данное событие,
// не зависимо от параметра события
void killTimer(msg_num msg) {
	uint8_t i;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		for (i = 0; i < maxTimers; i++) {
			if (lTimer[i].msg == msg) {
				lTimer[i].msg = 0;
			}
		}
		while ((lTimer[numTimers].msg == 0) && (numTimers > 0))
			numTimers--;
	}
}

// диспетчер таймеров
void dispatchTimer(void) {
	uint8_t i;

	for (i = 0; i <= numTimers; i++) {
		if (lTimer[i].msg == 0)
			continue;

		if (lTimer[i].time > 0) { // если не пришло время
			lTimer[i].time--; // просто уменьшаем время
		} else {
			sendMessage(lTimer[i].msg, lTimer[i].par); // создаем событие
			lTimer[i].time = lTimer[i].timeOrigin - 1; // FIXED: лишний тик таймера (-1)
		}
	}
}

void initMessages(void) {
	numHandlers     = 0;
	numTimers       = 0;
	lMesPointer     = 0;
	hMesPointer     = 0;

	/* главный таймер */	
        
	/** 
         * ATMEGA8 16bit timer
         * --------------------
         * mode: режим подсчета импульсов (сброс при совпадении)
         * Mode WGM13   WGM12   WGM11   WGM10
         * 4    0       1       0       0           CTC     OCR1A   Immediate   MAX
         * 
         * CS12     CS11    CS10        Description
         * 0        1       1           clk I/O /64 (From prescaler)
         * 
         * 2486AA–AVR–02/2013 ATmega8/L datasheet p. 97-99
         */
	TCCR1A = 0; // WGM11 = 0,  WGM10 = 0
        TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10); // WGM13 = 0, CS12 = 0

 
	/** регистр сравнения A
	 * --------------------
	 */
	OCR1A  = F_CPU / 62500UL; 

	/** enable interrupt: при совпадении с A
	 * -------------------------------------
	 */	
	TIMSK |=_BV(OCIE1A);
}

/**
 * RTOS Timer Handler
 */
ISR(TIMER1_COMPA_vect)
{
	dispatchTimer(); // а вот и диспетчер
}
