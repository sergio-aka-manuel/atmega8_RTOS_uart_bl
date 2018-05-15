#pragma once

#include <util/atomic.h>
#include "messages.h"

// обработка очереди сообщений
inline void __attribute__((always_inline)) dispatchMessage(void) {
	int8_t i;
	uint8_t res;
	msg_num msg;
	msg_par par;

	if (hMesPointer == lMesPointer) { // если пустая очередь - возврат
		return;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		lMesPointer = (lMesPointer + 1) & (maxMessages - 1); // сдвинем указатель

		msg = lMessage[lMesPointer].msg;
		par = lMessage[lMesPointer].par;
	}

	if (msg != 0 && numHandlers > 0) {
		for (i = numHandlers - 1; i >= 0; i--) { // просматриваем обработчики с конца
			if (lHandler[i].msg == msg) { // последний занесенный имеет приоритет
				res = lHandler[i].hnd(par); // вызываем обработчик
				if (res) { // если обработчик вернул 1, перываем обработку события
					break;
				}
			}
		}
	}
}

