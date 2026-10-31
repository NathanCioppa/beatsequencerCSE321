#ifndef BEAT_SEQUENCER_H
#define BEAT_SEQUENCER_H

#include "sequencer.h"

#define CPU_CLK_HZ 16000000

// At BPM of MAX_BPM=300, tempo clock operates at f=20Hz or T=50ms. (keep in mind each beat has 4 ticks)
// Poll inputs at least more frequently than the max tempo clock frequency to ensure they are captured at high tempos
#define POLL_PERIOD_MS 10 

#define READ_INPUTS (PINC & 0xF)

#endif