#include "../inc/std_types.h"
#include "uart/uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "gpio/gpio.h"
#include "rotaryEncoder/rotaryEncoder.h"

int main()
{
   uint8 cnt = 0;
    sint8 val = 0;
   sint8 valOld = 0;
   uint8 uartBuf[5];

   uart_init(UART_0, RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED, 115200UL, 1);
   uart_init(UART_1, RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED, 115200UL, 1);
//   encode_init();

   sei(); /* Enable the interrupts */

   while(1)
   {
//       val = encode_read2();
//       valOld = val;
////       if(val != valOld)
//       {
//           sprintf(uartBuf, "%d\r\n", cnt++);
//           uart_puts(UART_0, uartBuf);
//       }

       _delay_ms(100);
   }
   return 0;
}
