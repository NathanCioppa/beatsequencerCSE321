
// Any time ATMega datasheet is referenced in a comment throughout the project, it refers to:
// https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf

#include "beat_sequencer.h"
#include "Arduino_FreeRTOS.h"
#include <Arduino.h>
#include "poll_inputs_task.h"
#include "state.h"

void setup() {
  // allow pins to act as interrupts, ATMega datasheet 7.3.1
  SREG |= (1<<SREG_I); 

  // setup tempo clock (on Timer1)
  DDRB |= (1<<PB1); // PortB Pin1 is the output for Timer1, must be in output mode. ATMega datasheet 14.3.1
  TCCR1A = (1<<COM1A0); // set to Toggle on Compare Match (toggle output pin on each counter reset). ATMega datasheet 16.11.1
  TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10); // set to CTC Mode, with prescalar 64. ATMega datasheet 16.11.2
  OCR1A = BPM_TO_OCR1A(DEFAULT_BPM); // initialize clock to DEFAULT_BPM. 
  
/*
  // setup A0-A3 as interupt triggers (to trigger interrupt when user preses inputs EXCLIUDING state change input)
  // PortC pins can generate interrupts when configured, ATMega Datasheet 14.3.2 Table 14-6
  DDRC &= ~(0xF); // set pins A0-A3 as input
  PCICR = (1<<1); // enable pin change interrupt 1, ATMega datasheet 13.2.4
  PCMSK1 = 0xF; // enable pin change interrupts on PCINT[11:8], ATMega datasheet 13.2.7
*/

  // setup D2 as risng-edge interrupt trigger (to trigger state change)
  DDRD &= ~(1<<PD2); // PortD Pin2 is the input for INT0, ATMega datasheet 14.3.3 table 14-9
  EIMSK = (1<<INT0); // set interrupt mask to allow INT0 to trigger interrupt, ATMega datsheet 13.2.1
  EICRA = (1<<ISC01)|(1<<ISC00); // set INT0 to be rising edge triggerd, ATMega datasheet 13.2.2

  // setup outputs
  DDRD |= (1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7); // set PortD pins 4-7 as output, for sequence output.
  DDRB |= (1<<PB2)|(1<<PB3)|(1<<PB4); // set PortB pins 2-4 as output, for state tracking.

  xTaskCreate(poll_inputs_task, "poll", 100, NULL, 1, NULL);
  xTaskCreate(tick_write_task, "tikw", 100, NULL, 1, &tick_write_handle);

  SEQUENCER_START // default to sequencer being on, since recording is default state
}

void loop() {
  // Begs the question of "would you still love me if I was an empty loop?"
}
