#include "main.h"      // ← zastępuje usart.h + stm32f4xx_hal.h
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern UART_HandleTypeDef huart2;

QueueHandle_t xCmdQueue;
SemaphoreHandle_t xUartMutex;

static volatile char rxChar;
static char buildBuf [SHELL_CMD_LEN];
static uint8_t buildIdx = 0;
static TaskHandle_t xBlinkHandle = NULL;

void shell_print(const char *str){
	xSemaphoreTake(xUartMutex, portMAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	xSemaphoreGive(xUartMutex);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    BaseType_t xHigher = pdFALSE;  // ← na samej górze

	if (huart->Instance != USART2) return;
	if(rxChar=='\n' || rxChar=='\r'){
		if(buildIdx >0){
			buildBuf[buildIdx] = '\0';
			xQueueSendFromISR(xCmdQueue, buildBuf, &xHigher);
			buildIdx = 0;
		}
	}
	else if(rxChar== 0x7F || rxChar== 0x08){
		if(buildIdx!=0){
			buildIdx  = buildIdx - 1;
		}
	}
	else {
		buildBuf[buildIdx++] = rxChar;
	}

    HAL_UART_Receive_IT(&huart2, (uint8_t*)&rxChar, 1);  // ← tylko tu, na końcu
	portYIELD_FROM_ISR(xHigher);
}

void blink_task(void *params){
	uint32_t ms = (uint32_t)params;
	char msg[48];
	snprintf(msg, sizeof(msg), "led blink start = %u\r\n", (unsigned int)ms);
	shell_print(msg);
	while(1){
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		vTaskDelay(pdMS_TO_TICKS(ms));
	}
}

static void cmd_help(void) {
    shell_print("Komendy:\r\n");
    shell_print("  help           - ta lista\r\n");
    shell_print("  led zewn on         - zapal zewnetrzny LED\r\n");
    shell_print("  led zewn off        - zgas zewnetrzny LED\r\n");
    shell_print("  led wewn on         - zapal wewnetrzny LED\r\n");
    shell_print("  led wewn off        - zgas wewnetrzny LED\r\n");
    shell_print("  led blink <ms> - migaj co <ms> ms\r\n");
    shell_print("  led stop       - zatrzymaj miganie\r\n");
    shell_print("  status         - lista taskow\r\n");
}

static void cmd_led(const char *args){
	if (strcmp(args, "zewn on") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		shell_print("[led] ON\n");
	}
	else if (strcmp(args, "zewn off") == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
		shell_print("[led] OFF\n");

	}
	else if (strcmp(args, "wewn on") == 0){
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		shell_print("[led] ON\n");
	}
	else if (strcmp(args, "wewn off") == 0){
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
		shell_print("[led] OFF\n");

	}
	else if (strncmp(args, "blink ", 6) == 0){
		uint32_t ms = (uint32_t)atoi(args + 6);
		if (xBlinkHandle != NULL) {  // ← dodaj to
		        vTaskDelete(xBlinkHandle);
		        xBlinkHandle = NULL;
		    }
		xTaskCreate(blink_task, "blink_task", 200, (void*)ms, 1, &xBlinkHandle);
	}
	else if (strcmp(args, "stop") == 0){
		vTaskDelete(xBlinkHandle);
		xBlinkHandle = NULL;
	}
	else{
		shell_print(" Unknown command ");
	}
}

static void cmd_status(void){
	char buf[512];

	shell_print("Naglowek ");

	vTaskList(buf);

	shell_print(buf);

}

static void shell_process(char *cmd){
    if (strcmp(cmd, "help") == 0){
        cmd_help();
    }
    else if (strncmp(cmd, "led ", 4) == 0){  // strncmp bo sprawdzamy początek
        cmd_led(cmd + 4);
    }
    else if (strncmp(cmd, "led ", 4) == 0){  // strncmp bo sprawdzamy początek
            cmd_led(cmd + 4);
        }
    else if (strcmp(cmd, "status") == 0){
        cmd_status();
    }
    else {
        shell_print("[?] nieznana komenda\r\n");
    }
}

void shell_task(void *params){HAL_StatusTypeDef ret = HAL_UART_Receive_IT(&huart2, (uint8_t*)&rxChar, 1);
if (ret != HAL_OK) {
    shell_print("[ERR] UART Receive_IT failed!\r\n");
}

	//HAL_UART_Receive_IT(&huart2, (uint8_t*)&rxChar, 1);
	shell_print("\r\n======================\r\n");
	    shell_print("  FreeRTOS Mini Shell\r\n");
	    shell_print("======================\r\n");
	    shell_print("Wpisz 'help'\r\n> ");
	char cmd[SHELL_CMD_LEN];
	while(1){
		if (xQueueReceive(xCmdQueue, cmd, portMAX_DELAY) == pdPASS){
			shell_print("\r\n[DEBUG] dostałem: ");
			shell_print(cmd);
			shell_print("\r\n");
			shell_process(cmd);
			shell_print("> ");
		}

	}
}
