
#include "state.h"
#include "beat_sequencer.h"
#include <Arduino.h>
#include <avr/interrupt.h>

#define STATE_BIT_0 PB2
#define STATE_PORT PORTB

uint8_t state = DEFAULT_STATE;

void set_state(uint8_t state);


void set_state(uint8_t new_state) {
  state = new_state & 0b11;
  new_state |= 0b100;
  STATE_PORT = new_state << STATE_BIT_0;
}

// Iterrupt fires at high edge of state-change input
ISR(INT0_vect) {
  static unsigned long prevMillis = 0;
  unsigned long nowMillis = millis();
  // debounce
  if(nowMillis - prevMillis > 200) {
    set_state(state+1);
    
    prevMillis = nowMillis;
  }
}
