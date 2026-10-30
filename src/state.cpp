
#include "state.h"
#include "beat_sequencer.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#define STATE_BIT_0 PB2
#define STATE_PORT PORTB
#define STATE_TRANSITION_BIT 2
#define STATE_MASK 0b111 // state is 2 bits + 1 bit transition marker

uint8_t state = DEFAULT_STATE;
TaskHandle_t change_state_handle;

void set_state(uint8_t state);

void set_state(uint8_t new_state) {
  state = new_state & 0b11;
  new_state |= 0b100;
  STATE_PORT = new_state << STATE_BIT_0;
}

void change_state_task(void *pvparameters) {
  for(;;) {
    state = ((state+1) | (1<<STATE_TRANSITION_BIT)) & STATE_MASK;
    STATE_PORT = state << STATE_BIT_0;
  
    vTaskDelay(pdMS_TO_TICKS(STATE_TRANSITION_MS));
    state &= ~(1<<STATE_TRANSITION_BIT);
    STATE_PORT = state << STATE_BIT_0;
    CLEAR_STATE_CHANGE_INTERRUPT_PENDING // acts as a debounce
    vTaskSuspend(change_state_handle); // suspend self
  }
}

// Iterrupt fires at high edge of state-change input
ISR(INT0_vect) {
  xTaskResumeFromISR(change_state_handle);
}
