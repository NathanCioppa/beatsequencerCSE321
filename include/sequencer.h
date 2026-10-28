#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <inttypes.h>
#include <Arduino.h>

#define SEQ_OUT_PORT PORTD
#define SEQ_OUT_PORT_LSB PD4

#define ENABLE_TIMER1_ISR TIMSK1 |= (1<<OCIE1A);
#define DISABLE_TIMER1_ISR TIMSK1 &= ~(1<<OCIE1A);

extern uint8_t sequence[64];
extern uint8_t seq_position;
extern uint8_t seq_index;

void sequence_insert(uint8_t tick, uint8_t *effected_tick, uint8_t effected_position);
void sequence_restart();
void tick_write_task(void *pvparameters);

#endif