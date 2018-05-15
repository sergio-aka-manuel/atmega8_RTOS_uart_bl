#pragma once

#define maxHandlers	32 //FIX:64
#define maxMessages	64 //FIX:128
#define maxTimers	16 //FIX:32

// Сообщения
enum {
	MSG_DEFAULT_TASK = 1,
	
	MSG_BUTTON,
	MSG_BUTTON_REPEAT,
	
	MSG_LED1_CONTROL,
        MSG_LED2_CONTROL,
        MSG_LED3_CONTROL,
};

typedef uint8_t msg_num;		// тип события - мне пока хватает одного байта
typedef uint16_t msg_par;		// тип параметра события
typedef uint8_t (*handler)(msg_par);	// описание функции-обработчика

// структура записи из списка обработчиков
typedef struct {
	msg_num msg;                    // обрабатываемое событие
	handler hnd;                    // собственно сам обработчик
} iHandler;

// структура события из буфера событий
typedef struct {
	msg_num msg;                    // номер события
	msg_par par;                    // параметр
} iMessage;

// структура таймера
typedef struct {
	msg_num msg;			// номер генерируемого сообщения
	msg_par par;			// его параметр
	uint16_t time;			// таймер в условных тиках (сейчас 1 мс)
	uint16_t timeOrigin;		// таймер в условных тиках (сейчас 1 мс)
} iTimer;

extern volatile iTimer lTimer[maxTimers];		// список таймеров
extern volatile iHandler lHandler[maxHandlers];		// список обработчиков
extern volatile uint8_t numHandlers, numTimers;

extern volatile iMessage lMessage[maxMessages];		// буфер сообщений
extern volatile uint16_t lMesPointer, hMesPointer;	// указатели на начало и конец буфера

// установка обработчика события
// вызывается: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd);

// снятие обработчика события
// вызывается: killHandler(MSG_KEY_PRESS, &checkKey);
void killHandler(msg_num msg, handler hnd);

// занести событие в очередь
// пример вызова: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par);

// установить таймер
// пример вызова: setTimer(MSG_LCD_REFRESH, 0, 50);
void setTimer(msg_num msg, msg_par par, uint16_t time);

// убить таймер
// особенность - убивает все установленные таймеры на данное событие,
// не зависимо от параметра события
void killTimer(msg_num msg);

// начальная инициализация
void initMessages(void);
