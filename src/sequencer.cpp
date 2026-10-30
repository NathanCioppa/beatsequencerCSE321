
#include "sequencer.h"
#include <Arduino.h>
#include <avr/interrupt.h>
#include <Arduino_FreeRTOS.h>
#include "state.h"

#define SEQ_ON_HIGH_TICK seq_position & 1
#define IS_HIGH_TICK(x) x & 1
#define WRITE_TICK_OUT(tick) SEQ_OUT_PORT |= ((tick&0x0f)<<SEQ_OUT_PORT_LSB);
#define CLEAR_TICK_OUT SEQ_OUT_PORT &= ~(0x0f<<SEQ_OUT_PORT_LSB);
#define SEQUENCER_SET_POSITION_START seq_position = 0; seq_index = 0;

// sequence fits in 64 bytes (128 ticks, 4 bits each)
uint8_t sequence[64];

// position ranges 0-127, to address 128 ticks.
uint8_t seq_position;
// updated to be seq_position/2 when 
uint8_t seq_index;

TaskHandle_t tick_write_handle;

// Inserts a tick into the sequence's current position.
// Tick to insert is assumed to be contained in lower 4 bits of argument.
//
// NOTE insertion adds high inputs specified by the tick argument 
//  into the sequence's existing current tick.
//  EX: if current seq tick is 1010, and tick argument is 0011, 
//      then current seq tick becomes 1011
void sequence_insert(uint8_t tick, uint8_t *effected_tick, uint8_t effected_position) {
    if(IS_HIGH_TICK(effected_position))
        tick = tick << 4;
    else
        tick &= 0x0f;

    *effected_tick |= tick;
}

// periodically updates outputs with the sequence's current tick
// when not in recording state, reset sequencer to defaults
void tick_write_task(void *pvparameters) {
    for(;;) {
            taskENTER_CRITICAL();
            uint8_t tick = sequence[seq_index];

            if(SEQ_ON_HIGH_TICK)
                tick = tick >> 4;
            WRITE_TICK_OUT(tick);
            taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(TICK_WRITE_PERIOD_MS));
    }
}

// Updates sequence position and index counters.
// Clears the current sequence output pins.
ISR(TIMER1_COMPA_vect) {
    if(PINB & (1<<PB1)) { // trigger on High edge of clock
        // Black Magic high-speed value updates
        // Remember to pay tribute to the Dark Lords of bit-wise operations and powers of 2
        seq_position = (seq_position + 1) & 127;
        seq_index = seq_position >> 1;
        // equivilent to:
        //      seq_position = (seq_position+1)%128
        //      seq_index = seq_position/2
        CLEAR_TICK_OUT
    }
}

void exit_recording_state() {
    SEQUENCER_STOP
    CLEAR_TICK_OUT
}

void enter_recording_state() {
    seq_position = 0;
    seq_index = 0;
    SEQUENCER_START
}