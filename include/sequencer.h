#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <inttypes.h>
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#define SEQ_OUT_PORT PORTD
#define SEQ_OUT_PORT_LSB PD4

// set Timer1 to trigger an interrupt on Compare Match. ATMega datasheet 16.11.8
#define ENABLE_TIMER1_ISR TIMSK1 |= (1<<OCIE1A); 
#define DISABLE_TIMER1_ISR TIMSK1 &= ~(1<<OCIE1A);

#define SEQUENCER_STOP DISABLE_TIMER1_ISR; vTaskSuspend(tick_write_handle);
#define SEQUENCER_START vTaskResume(tick_write_handle); ENABLE_TIMER1_ISR;

#define TICK_WRITE_PERIOD_MS 10

extern uint8_t sequence[64];
extern uint8_t seq_position;
extern uint8_t seq_index;
extern bool sequencer_on;
extern TaskHandle_t tick_write_handle;

void sequence_insert(uint8_t tick, uint8_t *effected_tick, uint8_t effected_position);
void tick_write_task(void *pvparameters);

#endif