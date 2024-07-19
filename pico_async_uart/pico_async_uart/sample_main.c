#include "async_uart/AsyncUart.h"

// [define]
#define BUF_SIZE 256 // buffer size

// [File Scope Variables]
static UCHAR f_buf[BUF_SIZE]; // buffer

// [Function Prototype Declaration]
static void MainLoop();

int main() 
{
	stdio_init_all();

	// Initialize asynchronous UART
	ASUART_Init();
	
	// Main loop
	MainLoop();

	return 0;
}

// Main loop
static void MainLoop()
{
	int recvedSize;
	int sendSize;
	ULONG errBits;

	while (1) {
		// Asynchronous UART main processing
		// Call this function every cycle in the main loop
		ASUART_Main();

		// Get the received data from the receive queue
		recvedSize = ASUART_RecvBuf((PVOID)f_buf, sizeof(f_buf)); // The second argument is the size of the receive buffer.
		if (recvedSize > 0) { // If the received size is greater than 0
			// [Sending received data]
			sendSize = recvedSize;
			if (sendSize != ASUART_SendBuf((PVOID)f_buf, sendSize)) { // The second argument is the size you want to send.
				// The transmission queue is full, so it was not possible to store all the transmission data in the transmission queue.
			}
		}

		// Get the last UART error
		errBits = ASUART_GetLastErrorBits();
		if (errBits != 0) {
			// errBits:
			// *Multiple bits can be 1
			// 0x00000008 Overrun error
			// 0x00000004 Break error
			// 0x00000002 Parity error
			// 0x00000001 Framing error  		
			printf("errBits = %lu\n", errBits);	
			// Clear errBits	
			ASUART_ClearLastErrorBits();
		}
	}
}

