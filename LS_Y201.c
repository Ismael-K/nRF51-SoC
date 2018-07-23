
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


uint16_t JPEG_address = 0x0000;						// Starting address of JPEG chunks
volatile uint16_t JPEG_file_size;					// JPEG file size in bytes uint16_t
uint8_t *p;
int *temp;

//const uint16_t JPEG_Bytes = 5000;					// Big number for many, many bytes!
//volatile uint8_t JPEG[JPEG_Bytes];					// Array that stores incoming JPEG


uint8_t RX_Init[15] =
	{0x36, 0x32, 0x35, 0x0D, 0x0A, 0x49, 0x6E, 0x69,
	0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A
	};		// Camera's response after initialization
uint8_t RX_Reset[5] =
	{0x76, 0x00, 0x26, 0x00, 0x00};					// Reset ACK
uint8_t RX_TakePic[5] =
	{0x76, 0x00, 0x36, 0x00, 0x00};					// Take Pic ACK
uint8_t RX_FileSize[7] =	// Full ACK has 9 bytes, last two checked in function
	{0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00};		// Read JPEG file size ACK
uint8_t RX_ReadFile[5] =
	{0x76, 0x00, 0x32, 0x00, 0x00};					// Read JPEG ACK
uint8_t RX_StopPic[5] =
	{0x76, 0x00, 0x36, 0x00, 0x00};					// Stop Pic ACK
uint8_t RX_CompRat[5] =		// Full ACK has 6 bytes, last one checked in function
	{0x76, 0x00, 0x31, 0x00, 0x00};					// Compression Ratio ACK
uint8_t RX_Resolution[5] =
	{0x76, 0x00, 0x31, 0x00, 0x00};					// Image Size ACK
uint8_t RX_PowerSave[5] =
	{0x76, 0x00, 0x3E, 0x00, 0x00};					// Power Save ACK
uint8_t RX_BaudRate[5] =
	{0x76, 0x00, 0x24, 0x00, 0x00};					// Baud Rate ACK

	
void LSY201_Init(){							// Checks for the correct initialization routine from camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED 0,
	nrf_gpio_pin_set(LED_1);					// and LED 1 while camera is initializing
	
	uint8_t RX_ACK[15];						// Array to store incoming ACK
	uint8_t IB=0;
	while(IB < 15){
		RX_ACK[IB] = simple_uart_get();
		if(RX_ACK[IB] == RX_Init[IB]){		// If RX byte is correct,
			IB++;								// increment, and get next byte
		}
	}
	
	delay_ms(3000);							// Wait after correct initialization complete
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
	nrf_gpio_pin_clear(LED_1);				// Turn off LED1
}

void LSY201_Reset(){						// TX 'Reset' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX

//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x26);
	simple_uart_put(0x00);	
	
//	uint8_t RX_ACK[5];						// Array to store incoming ACK
//	for(uint8_t i=0; i<5; i++)				// Comparing RX byte with predefined ACK bytes:
//	{
//		RX_ACK[i] = simple_uart_get();
//		if(RX_ACK[i] != RX_Reset[i])		// If RX byte is not correct,
//			LED_FLASHY_TIME();					// Error: flash LEDs in infinite loop.
//	}
	
	uint8_t RX_ACK[5];						// Array to store incoming ACK
	uint8_t IB=0;
	while(IB < 5){
		RX_ACK[IB] = simple_uart_get();
		if(RX_ACK[IB] == RX_Reset[IB]){		// If RX byte is correct,
			IB++;								// increment, and get next byte
//			RX_ACK[IB] = simple_uart_get();
//			if(RX_ACK[IB] != RX_Init[IB])
//				IB=0;
		}
	}
	
	LSY201_Init();							// Check for correct initialization  (3000ms delay included)
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void LSY201_TakePic(){						// TX 'Take Picture' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
//	NRF_UART0->RXD = 0;
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x36);
	simple_uart_put(0x01);
	simple_uart_put(0x00);
	
	uint8_t RX_ACK[5];						// Array to store incoming ACK
	for(uint8_t i=0; i<5; i++)				// Comparing RX byte with predefined ACK bytes:
	{
		RX_ACK[i] = simple_uart_get();
		if(RX_ACK[i] != RX_TakePic[i])		// If RX byte is not correct,
			while(1);					// Error: flash LEDs in infinite loop. LED_FLASHY_TIME()
	}
	
	
//	uint8_t RX_ACK[5];						// Array to store incoming ACK
//	uint8_t IB=0;
//	while(IB < 5){
//		RX_ACK[IB] = simple_uart_get();
//		if(RX_ACK[IB] == RX_TakePic[IB]){		// If RX byte is correct,
//			IB++;								// increment, and get next byte
////			RX_ACK[IB] = simple_uart_get();
////			if(RX_ACK[IB] != RX_Init[IB])
////				IB=0;
//		}
//	}
	
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void LSY201_FileSize(){						// TX 'Read JPEG File Size' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x34);
	simple_uart_put(0x01);
	simple_uart_put(0x00);
	
	uint8_t RX_ACK[9];						// Array to store incoming ACK
	for(uint8_t i=0; i<7; i++)				// Comparing RX byte with predefined ACK bytes:
	{
		RX_ACK[i] = simple_uart_get();
		if(RX_ACK[i] != RX_FileSize[i])		// If RX byte is not correct,
			while(1);					// Error: flash LEDs in infinite loop. LED_FLASHY_TIME()
	}
	
//	uint8_t RX_ACK[9];						// Array to store incoming ACK
//	uint8_t IB=0;
//	while(IB < 7){
//		RX_ACK[IB] = simple_uart_get();
//		if(RX_ACK[IB] == RX_Reset[IB]){		// If RX byte is correct,
//			IB++;								// increment, and get next byte
////			RX_ACK[IB] = simple_uart_get();
////			if(RX_ACK[IB] != RX_Init[IB])
////				IB=0;
//		}
//	}
	
	RX_ACK[7] = simple_uart_get();			// 8th and 9th bytes of ACK include the JPEG
	RX_ACK[8] = simple_uart_get();				// file size, 8th byte has MSB.
	JPEG_file_size = 0x100*RX_ACK[7] + RX_ACK[8];	// The actual JPEG file size in bytes 0x100 = 256
	
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0	
}

/**< The following function will help to look for the beginning and end of the jpeg file */
void Find_FFD8(){
	uint8_t match_counter = 0;
	while(match_counter < 2){
		uint8_t get_data = simple_uart_get();
		if(get_data == 0xFF)
		{
			match_counter ++;
		}
		else if(match_counter == 1 && get_data == 0xD8)
		{
			match_counter ++;
		}
		else
		{
			match_counter = 0;
		}
	}
}

void Find_FFD9(){
	uint8_t match_counter = 0;
	while(match_counter < 2){
		uint8_t get_data = simple_uart_get();
		if(get_data == 0xFF)
		{
			match_counter ++;
		}
		else if(match_counter == 1 && get_data == 0xD9)
		{
			match_counter ++;
		}
		else
		{
			match_counter = 0;
		}
	}
}

void Skip_ack()
{
	uint8_t match_counter = 0;
	uint8_t i = 0;
	while(match_counter < 5){
		uint8_t get_data = simple_uart_get();
		if(get_data == RX_ReadFile[i])
		{
			match_counter ++;
		}
		else
		{
			i = 0;
			match_counter = 0;
		}
		i++;
	}
}


/**< end of Jpeg finder */

void LSY201_ReadFile(uint16_t LSY201_ChunkBytes){	// TX 'Read JPEG File Content' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
	uint8_t MH = JPEG_address/0x100;		// JPEG chunk start address upper 8 bits
	uint8_t ML = JPEG_address%0x100;		// JPEG chunk start address lower 8 bits
	uint8_t KH = LSY201_ChunkBytes/0x100;	// JPEG chunk length upper 8 bits
	uint8_t KL = LSY201_ChunkBytes%0x100;	// JPEG chunk length lower 8 bits	
	
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x32);
	simple_uart_put(0x0C);
	simple_uart_put(0x00);
	simple_uart_put(0x0A);
	simple_uart_put(0x00);
	simple_uart_put(0x00);
	simple_uart_put(MH);					// MH	
	simple_uart_put(ML);					// ML
	simple_uart_put(0x00);
	simple_uart_put(0x00);
	simple_uart_put(KH);					// KH			
	simple_uart_put(KL);					// KL
	simple_uart_put(0x00);
	simple_uart_put(0x0A);
	
	JPEG_address += LSY201_ChunkBytes;		// Starting address of each chunk incremented by num of bytes in chunk
	
/**<	uint8_t RX_ACK[5];						// Array to store incoming ACK
	if(ERROR_CHECK_ON == true)
{
	for(uint8_t i=0; i<5; i++)				// Comparing RX byte with predefined ACK bytes:
	{
		RX_ACK[i] = simple_uart_get();
		if(RX_ACK[i] != RX_ReadFile[i])		// If RX byte is not correct,
			while(1);					// Error: flash LEDs in infinite loop. LED_FLASHY_TIME()
	}	
}
		nrf_gpio_pin_clear(LED_0);				// Turn off LED0
*/

}

bool Get_Chunk_JPEG(uint16_t BytesPerChunk, uint8_t arr[]){

	uint16_t NumOfFullChunks = JPEG_file_size/BytesPerChunk;
	uint16_t NumOfBytesRemain = JPEG_file_size%BytesPerChunk;
	uint8_t rj1, rj2, rj3;					// For loop counter variables
//	p = (uint8_t *)calloc(BytesPerChunk,sizeof(uint8_t));  //JPEG_file_size,
	LSY201_ReadFile(BytesPerChunk);		// Read JPEG chunk
	Skip_ack();
	for(rj2=0; rj2<BytesPerChunk; rj2++){
		arr[rj2] = simple_uart_get();	//JPEG
//			temp[rj1*BytesPerChunk + rj2] = JPEG[rj1*BytesPerChunk + rj2];	 // temp var
	}
	Skip_ack();
	if(NumOfBytesRemain > 0){
		LSY201_ReadFile(NumOfBytesRemain);			// Read JPEG chunk
		Skip_ack();
		for(rj3=0; rj3<NumOfBytesRemain; rj3++){
			arr[rj3] = simple_uart_get(); //	JPEG
//				temp[(NumOfFullChunks*BytesPerChunk) + rj3] = (int)simple_uart_get();
		}
		Skip_ack();
}
	return true;
}
void Get_Full_JPEG(uint16_t BytesPerChunk){
	// run LSY201_ReadFile as many times as needed and produce a whole JPEG

	nrf_gpio_pin_set(LED_1);
	uint16_t NumOfFullChunks = JPEG_file_size/BytesPerChunk;
	uint16_t NumOfBytesRemain = JPEG_file_size%BytesPerChunk;
	
	//uint8_t JPEG; //[JPEG_file_size];
	p = (uint8_t *)calloc(JPEG_file_size,sizeof(uint8_t));  //JPEG_file_size, 
//		temp[JPEG_file_size]; // temp var to see int elements
	
	uint8_t rj1, rj2, rj3;					// For loop counter variables
	nrf_gpio_pin_set(LED_0);
	for(rj1=0; rj1<NumOfFullChunks; rj1++)
	{	// Ask camera to send JPEG chunks, then store them
		LSY201_ReadFile(BytesPerChunk);		// Read JPEG chunk
		Skip_ack();
		for(rj2=0; rj2<BytesPerChunk; rj2++){
			p[rj1*BytesPerChunk + rj2] = simple_uart_get();	//JPEG
//			temp[rj1*BytesPerChunk + rj2] = JPEG[rj1*BytesPerChunk + rj2];	 // temp var
	}
	Skip_ack();

/**<		uint8_t RX_ACK[5];						// Array to store incoming ACK
		for(uint8_t i=0; i<5; i++)				// Comparing RX byte with predefined ACK bytes:
		{
			RX_ACK[i] = simple_uart_get();
			if(RX_ACK[i] != RX_ReadFile[i])		// If RX byte is not correct,
				while(1);  // Error: flash LEDs in infinite loop. LED_FLASHY_TIME();
		}
*/
	}
	if(NumOfBytesRemain > 0){
		LSY201_ReadFile(NumOfBytesRemain);			// Read JPEG chunk
		Skip_ack();
		for(rj3=0; rj3<NumOfBytesRemain; rj3++){
			p[(NumOfFullChunks*BytesPerChunk) + rj3] = simple_uart_get(); //	JPEG
//				temp[(NumOfFullChunks*BytesPerChunk) + rj3] = (int)simple_uart_get();

		}
		Skip_ack();

/**<		ack checking
		uint8_t RX_ACK[5];						// Array to store incoming ACK
		for(uint8_t i=0; i<5; i++)				// Comparing RX byte with predefined ACK bytes:
		{
			RX_ACK[i] = simple_uart_get();
			if(RX_ACK[i] != RX_ReadFile[i])		// If RX byte is not correct,
				while(1); 				// Error: flash LEDs in infinite loop.LED_FLASHY_TIME();	
		}
*/		
	}
	
	nrf_gpio_pin_clear(LED_0);
	nrf_gpio_pin_clear(LED_1);
}
void LSY201_StopPic(){						// TX 'Stop Taking Pictures' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x36);
	simple_uart_put(0x01);
	simple_uart_put(0x03);
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void LSY201_CompRat(){						// TX 'Compression Ratio' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x31);
	simple_uart_put(0x05);
	simple_uart_put(0x01);
	simple_uart_put(0x01);
	simple_uart_put(0x12);
	simple_uart_put(0x04);
	simple_uart_put(0x36);
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void LSY201_Resolution(uint8_t LSY201_Res){	// TX 'Image Size' command (reset included) to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
	uint8_t XX;
	if(LSY201_Res == 1)						// 640x480 resolution
		XX = 0x00;
	else if(LSY201_Res == 2)				// 320x240 resolution
		XX = 0x11;
	else if(LSY201_Res == 3)				// 160x120 resolution
		XX = 0x22;
	
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x31);
	simple_uart_put(0x05);
	simple_uart_put(0x04);
	simple_uart_put(0x01);
	simple_uart_put(0x00);
	simple_uart_put(0x19);
	simple_uart_put(XX);					// User's choice from above
//	delay_ms(18);
	
	uint8_t RX_ACK[5];						// Array to store incoming ACK
	for(uint8_t i=0; i<5; i++)				// Comparing RX byte with predefined ACK bytes:
	{
		RX_ACK[i] = simple_uart_get();
		if(RX_ACK[i] != RX_Resolution[i])		// If RX byte is not correct,
			while(1);					// Error: flash LEDs in infinite loop.LED_FLASHY_TIME()
	}
	
//	LSY201_Reset();							// Camera needs to be reset after resolution change
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void LSY201_PowerSave(uint8_t LSY201_PS){	// TX 'Compression Ratio' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
	uint8_t XX;
	if(LSY201_PS == 1)
		XX = 0x01;
	else if(LSY201_PS ==0)
		XX = 0x00;
//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x3E);
	simple_uart_put(0x03);
	simple_uart_put(0x00);
	simple_uart_put(0x01);
	simple_uart_put(XX);					// User selected
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void LSY201_BaudRate(uint8_t LSY201_BR){	// TX 'Change Baud Rate' command to camera
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate TX
	
	uint8_t BR1, BR2;
	if(LSY201_BR == 1){
		BR1 = 0x0D;
		BR2 = 0xA6;
	}
	else if(LSY201_BR == 2){
		BR1 = 0x1C;
		BR2 = 0x4C;
	}
	else if(LSY201_BR == 3){
		BR1 = 0x2A;
		BR2 = 0xF2;
	}
	else if(LSY201_BR == 4){
		BR1 = 0x56;
		BR2 = 0xE4;
	}
	else if(LSY201_BR == 5){
		BR1 = 0xAE;
		BR2 = 0xC8;
	}
	//	simple_uart_put(0xFF);					// Keeps line high so that actual command bits are not lost
	simple_uart_put(0x56);
	simple_uart_put(0x00);
	simple_uart_put(0x24);
	simple_uart_put(0x03);
	simple_uart_put(0x01);
	simple_uart_put(BR1);					// User selected
	simple_uart_put(BR2);					// User selected
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
}
void delay_ms(float volatile delay_in_ms){	// Function for delays in ms
	nrf_gpio_pin_set(LED_0);				// Turn on LED0 to indicate delay
	
	uint32_t d_i_us = delay_in_ms*1000;
	nrf_delay_us(d_i_us);
	
	nrf_gpio_pin_clear(LED_0);				// Turn off LED0
	
}
void LED_FLASHY_TIME(){						// You dun goofed now!
	
	nrf_gpio_pin_clear(LED_0);
	nrf_gpio_pin_clear(LED_1);
	
	while(true)
	{		
		nrf_gpio_pin_set(LED_1);
		delay_ms(75);
		nrf_gpio_pin_clear(LED_1);
		delay_ms(75);
		nrf_gpio_pin_set(LED_1);
		delay_ms(75);
		nrf_gpio_pin_clear(LED_1);
		delay_ms(120);
		
		nrf_gpio_pin_set(LED_0);
		delay_ms(75);
		nrf_gpio_pin_clear(LED_0);
		delay_ms(75);
		nrf_gpio_pin_set(LED_0);
		delay_ms(75);
		nrf_gpio_pin_clear(LED_0);
		delay_ms(120);
	}
}

