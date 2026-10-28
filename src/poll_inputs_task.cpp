
#include "poll_inputs_task.h"

#include <inttypes.h>
#include <Arduino_FreeRTOS.h>
#include "beat_sequencer.h"
#include <Arduino.h>
#include "sequencer.h"
#include "state.h"

void handle_inputs_recording(uint8_t inputs, uint8_t *effected_tick, uint8_t effected_position);
void handle_inputs_load(uint8_t inputs);
void handle_inputs_store(uint8_t inputs);
void handle_inputs_tempo(uint8_t inputs);

void poll_inputs_task(void *pvParams) {
  uint8_t prev = 0;
  uint8_t read = 0;
  uint8_t highedge = 0;
  for(;;) {
    read = READ_INPUTS;
    taskENTER_CRITICAL();
    uint8_t *effected_tick = &sequence[seq_index];
    uint8_t effected_position = seq_position;
    taskEXIT_CRITICAL();
    highedge = (read ^ prev) & read;
    prev = read;

    switch(state) {
      case STATE_RECORDING: 
        handle_inputs_recording(highedge, effected_tick, effected_position);
        break;
      case STATE_LOAD:
        handle_inputs_load(highedge);
        break;
      case STATE_STORE:
        handle_inputs_store(highedge);
        break;
      case STATE_TEMPO:
        handle_inputs_tempo(highedge);
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
  }
}

void handle_inputs_recording(uint8_t inputs, uint8_t *effected_tick, uint8_t effected_position) {
  if(inputs)
    sequence_insert(inputs, effected_tick, effected_position);
}

void handle_inputs_load(uint8_t inputs) {

}

void handle_inputs_store(uint8_t inputs) {

}

void handle_inputs_tempo(uint8_t inputs) {

}