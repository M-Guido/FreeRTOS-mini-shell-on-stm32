#ifndef INC_SHELL_H_
#define INC_SHELL_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define SHELL_CMD_LEN    64
#define SHELL_QUEUE_LEN  8

extern QueueHandle_t     xCmdQueue;
extern SemaphoreHandle_t xUartMutex;

void blink_task(void *params);
void shell_task(void *params);
void shell_print(const char *str);

#endif /* INC_SHELL_H_ */  // ← tego brakuje!
