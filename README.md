This is an asynchronous UART library for Raspberry Pi pico.  
(using C and pico-sdk)  

This library performs asynchronous UART transmission and reception using queues and UART transmit/receive interrupts.  

When used in a multi-core environment, exclusive processing is performed within the library.  
    
**1.How to use**   
Please set the following user-dependent settings in AsyncUart.h  
![image](https://github.com/user-attachments/assets/14ce3c06-a9e2-4c53-b987-751fa1164aad)  

**2.In your app, do the following**  
(See sample_main.c)    

(1)#include "async_uart/AsyncUart.h"    
  
(2)Calls ASUART_Init() when power is turned on.  
  
(3)The main loop calls ASUART_Main() every cycle.    
  
(4)If you want to send, use ASUART_SendBuf().  
  
(5)If you want to get received data from the receive queue, use ASUART_RecvBuf().  
  
(6)The last UART error can be got by ASUART_GetLastErrorBits().    
   (It can be cleared by ASUART_ClearLastErrorBits().)  
  
**3. About the sample program**    
   
Sends the received data.   
Please refer to AsyncUart.h for the pins used and UART communication settings.    
