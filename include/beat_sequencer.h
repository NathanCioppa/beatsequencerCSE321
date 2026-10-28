#ifndef BEAT_SEQUENCER_H
#define BEAT_SEQUENCER_H

#include "sequencer.h"

#define STATE_RECORDING 0b00
#define STATE_STORE 0b01
#define STATE_LOAD 0b10
#define STATE_TEMPO 0b11

#define DEFAULT_STATE STATE_RECORDING

#define CPU_CLK_HZ 16000000

#define DEFAULT_BPM 30
#define MAX_BPM 300
#define MIN_BPM 30

// Convert x in BPM to the OCR1A value required by Timer1 for tempo clock. 
// Assumes prescalar of 64, suitable for range 30-300 BPM
#define BPM_TO_OCR1A(x) (uint16_t)( ( (float)CPU_CLK_HZ / ( 2.0 * 64.0 * ((float)x / 15.0) ) ) - 1.0  )
// ATMega datasheet ATMega 16.9.2 describes OCR1A usage and gives formula.
// Note above that a conversion from BPM to HZ is used which is not present in the cited formula. 

// At BPM of MAX_BPM=300, tempo clock operates at f=20Hz or T=50ms. (keep in mind each beat has 4 ticks)
// Poll inputs at least more frequently than the max tempo clock frequency to ensure they are captured at high tempos
#define POLL_PERIOD_MS 10 

#define READ_INPUTS (PINC & 0xF)

#endif