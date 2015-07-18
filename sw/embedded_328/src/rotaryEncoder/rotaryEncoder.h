#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

#include <avr/io.h>
#include "std_types.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//#define ROTARY_ENCODER_1
#define ROTARY_ENCODER_2
//#define ROTARY_ENCODER_4
#define ROTARY_ENCODER_BUTTON

#define ROTARY_ENCODER_PHASE_A_PIN (PIND)
#define ROTARY_ENCODER_PHASE_A     (PD2)
#define ROTARY_ENCODER_PHASE_B_PIN (PIND)
#define ROTARY_ENCODER_PHASE_B     (PD3)


#ifdef ROTARY_ENCODER_BUTTON
#define ROTARY_ENCODER_BUTTON_PIN   PA1
#define ROTARY_ENCODER_BUTTON_PORT  PORTA
#endif


void encode_init(void);

#ifdef ROTARY_ENCODER_1
sint8 encode_read1( void );
#endif

#ifdef ROTARY_ENCODER_2
sint8 encode_read2( void );
#endif

#ifdef ROTARY_ENCODER_4
sint8 encode_read4( void );
#endif

#endif /* ROTARYENCODER_H_ */
