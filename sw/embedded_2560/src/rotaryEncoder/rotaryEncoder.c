/*
 * rotaryEncoder.c
 *
 *  Created on: Jul 18, 2015
 *      Author: armin
 */


#include "rotaryEncoder.h"
#include "../gpio/gpio.h"

                // target: ATmega16
//------------------------------------------------------------------------

#define XTAL        16000000UL         // 16MHz

#define PHASE_A     (ROTARY_ENCODER_PHASE_A_PIN & 1<<ROTARY_ENCODER_PHASE_A)
#define PHASE_B     (ROTARY_ENCODER_PHASE_B_PIN & 1<<ROTARY_ENCODER_PHASE_B)

volatile sint8 enc_delta;          // -128 ... 127
static sint8 last;


void encode_init( void )
{
  sint8 new;

  new = 0;
  if( PHASE_A )
    new = 3;
  if( PHASE_B )
    new ^= 1;                   // convert gray to binary
  last = new;                   // power on state
  enc_delta = 0;
  TCCR0A = (1 << CS00 | 1 << CS01);
  OCR0A  = 0xFA;
  //  TCCR0A = 1<<WGM01^1<<CS01^1<<CS00;     // CTC, XTAL / 64
//  OCR0A = (uint8)(XTAL / 64.0 * 1e-3 - 0.5);   // 1ms
  TIMSK0 |= 1<<OCIE0A;
}


ISR( TIMER0_COMPA_vect )             // 1ms for manual movement
{
  sint8 new, diff;
  gpio_ToggleChannel(GPIO_CHANNEL_PD4);
  new = 0;
  if( PHASE_A )
    new = 3;
  if( PHASE_B )
    new ^= 1;                   // convert gray to binary
  diff = last - new;                // difference last - new
  if( diff & 1 ){               // bit 0 = value (1)
    last = new;                 // store new as next last
    enc_delta += (diff & 2) - 1;        // bit 1 = direction (+/-)
  }
}

#ifdef ROTARY_ENCODER_1
sint8 encode_read1( void )         // read single step encoders
{
  sint8 val;

  cli();
  val = enc_delta;
  enc_delta = 0;
  sei();
  return val;                   // counts since last call
}
#endif

#ifdef ROTARY_ENCODER_2
sint8 encode_read2( void )         // read two step encoders
{
  sint8 val;

  cli();
  val = enc_delta;
  enc_delta = val & 1;
  sei();
  return val >> 1;
}
#endif

#ifdef ROTARY_ENCODER_4
sint8 encode_read4( void )         // read four step encoders
{
  sint8 val;

  cli();
  val = enc_delta;
  enc_delta = val & 3;
  sei();
  return val >> 2;
}
#endif

//
//sint32 main( void )
//{
//  sint32 val = 0;
//
//  LEDS_DDR = 0xFF;
//  encode_init();
//  sei();
//
//  for(;;){
//    val += encode_read1();          // read a single step encoder
//    LEDS = val;
//  }
//}
