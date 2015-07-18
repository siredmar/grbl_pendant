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
   sint8 val = 0;
   sint8 valOld = 0;
   uint8 uartBuf[5];

   uart_init(RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED);
   encode_init();

   sei(); /* Enable the interrupts */

   while(1)
   {
       val = encode_read2();
//       valOld = val;
//       if(val != valOld)
       {
           sprintf(uartBuf, "%d\r\n", val);
           uart_puts(uartBuf);
       }

       _delay_ms(100);
   }
   return 0;
}
