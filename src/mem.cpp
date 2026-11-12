
#include "mem.h"
#include <Arduino.h>
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include "sequencer.h"
#include "beat_sequencer.h"

#define WRITE_CONTROL 0b10100000
#define READ_CONTROL 0b10100001

#define I2C_MEM_ADDR 0x50

#define ADDR_DATA_LEN 2
#define TRANSMISSION_LEN SEQUENCE_LEN+ADDR_DATA_LEN

#define MEM_SEQ_TO_ADDR(x) (x << 6)

static uint8_t seq_buf[SEQUENCE_LEN];
static uint8_t transmission[32];
uint16_t target_mem_seq = 0;

void increment_target_mem_seq(uint16_t inc_amount) {
    if(inc_amount == 0)
        return;

    INCREMENT_TARGET_MEM_SEQ(inc_amount);
    lcd.clear();
    lcd.print(target_mem_seq);
    
}

void store_sequence(uint16_t mem_seq) {
    if(mem_seq > MAX_MEM_SEQ)
        return;

    uint16_t loc = MEM_SEQ_TO_ADDR(mem_seq);
    
    // copy the current sequence into local buffer
    taskENTER_CRITICAL();
    for(int i=0; i<SEQUENCE_LEN; i++)
        seq_buf[i] = sequence[i];
    taskEXIT_CRITICAL();
    
    // Wire only buffers 32 bytes at a time.
    // to store 64 bytes, multiple transmissions are needed.
    // each transmission also needs a 2 byte header before the data to set the address.
    // 
    // 3 transmissions are needed
    // first 30 bytes, second 30 bytes, final 4 bytes, each with a 2 byte header.

    // transmission 1, with first 30 bytes of sequence
    transmission[0] = loc >> 8;
    transmission[1] = loc;
    for(int i=0; i<30; i++) {
        transmission[i+2] = seq_buf[i];
    }
    for(;;) { // loop until the transmission is successful
        Wire.beginTransmission(I2C_MEM_ADDR);
        Wire.write(transmission, 32);
        if(Wire.endTransmission() == 0)
            break;
    }

    // transmission 2, next 30 bytes
    loc+=30;
    transmission[0] = loc >> 8;
    transmission[1] = loc;
    for(int i=0; i<30; i++) {
        transmission[i+2] = seq_buf[i+30];
    }
    for(;;) {
        Wire.beginTransmission(I2C_MEM_ADDR);
        Wire.write(transmission, 32);
        if(Wire.endTransmission() == 0)
            break;
    }

    // transmission 3, final 4 bytes
    loc+=30;
    transmission[0] = loc >> 8;
    transmission[1] = loc;
    for(int i=0; i<4; i++) {
        transmission[i+2] = seq_buf[i+60];
    }
    for(;;) {
        Wire.beginTransmission(I2C_MEM_ADDR);
        Wire.write(transmission, 6);
        if(Wire.endTransmission() == 0)
            break;
    }

    // should probably make this a loop with some error checking but that probably wont happen.
    // sorry flash memory :(
}

void load_sequence(uint16_t mem_seq) {
    if(mem_seq > MAX_MEM_SEQ)
        return;

    uint16_t loc = MEM_SEQ_TO_ADDR(mem_seq);

    // Wire library only does communication up to 32 bytes at a time.
    // Reading from memory will need to be done in 2 requests.

    // Before reading, the target address needs to be written to the memory chip.
    for(;;) {
        Wire.beginTransmission(I2C_MEM_ADDR);
        Wire.write(loc >> 8);
        Wire.write(loc);
        Wire.endTransmission(false); // keep connection open to do the read.
        if(Wire.requestFrom(I2C_MEM_ADDR, SEQUENCE_LEN/2) == 32) // try again if it doesnt get enough data
            break;
    }
    
    for(int i=0; i<32; i++) {
        seq_buf[i] = Wire.read(); // the requested bytes are gotten from read.
    }
    loc+=32;
    for(;;) {
        Wire.beginTransmission(I2C_MEM_ADDR);
        Wire.write(loc >> 8);
        Wire.write(loc);
        Wire.endTransmission(false);
        if(Wire.requestFrom(I2C_MEM_ADDR, SEQUENCE_LEN/2) == 32)
            break;
    }

    for(int i=32; i<64; i++) {
        seq_buf[i] = Wire.read();
    }

    taskENTER_CRITICAL();
    for(int i=0; i<SEQUENCE_LEN; i++) {
        sequence[i] = seq_buf[i];
    }
    taskEXIT_CRITICAL();

}