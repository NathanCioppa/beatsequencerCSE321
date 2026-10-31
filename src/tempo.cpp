
#include "tempo.h"
#include "beat_sequencer.h"
uint16_t tempo = DEFAULT_BPM;

void increment_tempo(uint16_t amount) {
    tempo = ((tempo-MIN_BPM+amount) % (MAX_BPM-MIN_BPM+1)) + MIN_BPM;
    SET_TEMPO_CLK_BPM(tempo);
}
