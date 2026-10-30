
#ifndef STATE_H
#define STATE_H

#include <inttypes.h>
#include <avr/interrupt.h>
#include <Arduino_FreeRTOS.h>

#define ENABLE_STATE_CHANGE_ISR EIMSK |= (1<<INT0);
#define DISABLE_STATE_CHANGE_ISR EIMSK &= ~(1<<INT0);
#define CLEAR_STATE_CHANGE_INTERRUPT_PENDING EIFR &= (1<<INTF0);

#define STATE_RECORDING 0
#define STATE_STORE 1
#define STATE_LOAD 2
#define STATE_TEMPO 3

#define DEFAULT_STATE STATE_RECORDING

#define STATE_TRANSITION_MS 300

extern uint8_t state;
extern TaskHandle_t change_state_handle;

void change_state_task(void *pvparameters);

#endif