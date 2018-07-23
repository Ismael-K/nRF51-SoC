#ifndef _LS_Y201_H
#define _LS_Y201_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "nrf.h"
#include "nrf51.h"
#include "nrf_delay.h"
#include "simple_uart.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "LS_Y201.h"

extern uint16_t JPEG_address;
extern volatile uint16_t JPEG_file_size; //uint16_t
extern uint8_t *p;
//extern volatile uint8_t JPEG[];


void LSY201_Init(void);
void LSY201_Reset(void);						// TX 'Reset' command to camera
void LSY201_TakePic(void);						// TX 'Take Picture' command to camera
void LSY201_FileSize(void);						// TX 'Read JPEG File Size' command to camera
void LSY201_ReadFile(uint16_t LSY201_ChunkBytes);	// TX 'Read JPEG File Content' command to camera
void Get_Full_JPEG(uint16_t BytesPerChunk);		// Run LSY201_ReadFile() as many times as needed to obtain full JPEG
void LSY201_StopPic(void);						// TX 'Stop Taking Pictures' command to camera
void LSY201_CompRat(void);						// TX 'Compression Ratio' command to camera
void LSY201_Resolution(uint8_t LSY201_Res);		// TX 'Image Size' command (reset included) to camera
void LSY201_PowerSave(uint8_t LSY201_PS);		// TX 'Power Saving' command to camera
void LSY201_BaudRate(uint8_t LSY201_BR);		// TX 'Change Baud Rate' command to camera
void delay_ms(float volatile delay_in_ms);		// Function for delays in ms
void LED_FLASHY_TIME(void);						// You dun goofed now!
void Find_FFD8(void);
void Find_FFD9(void);
void Skip_ack(void);
bool Get_Chunk_JPEG(uint16_t BytesPerChunk, uint8_t arr[]);




#endif
