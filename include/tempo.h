
#ifndef TEMPO_H
#define TEMPO_H

#include <inttypes.h>

#define DEFAULT_BPM 30

#define MAX_BPM 300
#define MIN_BPM 30
// Convert x in BPM to the OCR1A value required by Timer1 for tempo clock. 
// Assumes prescalar of 64, suitable for range 30-300 BPM
#define BPM_TO_OCR1A(x) (uint16_t)( ( (float)CPU_CLK_HZ / ( 2.0 * 64.0 * ((float)x / 15.0) ) ) - 1.0  )
// ATMega datasheet ATMega 16.9.2 describes OCR1A usage and gives formula.
// Note above that a conversion from BPM to HZ is used which is not present in the cited formula. 

#define SET_TEMPO_CLK_BPM(x) OCR1A = BPM_TO_OCR1A(x);

void enter_tempo_state();
void exit_tempo_state();
void increment_tempo(uint16_t amount);

extern uint16_t tempo; // in BPM

#endif
