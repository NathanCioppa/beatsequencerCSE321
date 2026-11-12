
#include "mem.h"
#include <Arduino.h>
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include "sequencer.h"

#define WRITE_CONTROL 0b10100000
#define READ_CONTROL 0b10100001

#define I2C_MEM_ADDR 0x50

#define MAX_MEM_LOC 0xFFFF
#define ADDR_DATA_LEN 2
#define TRANSMISSION_LEN SEQUENCE_LEN+ADDR_DATA_LEN

#define LOC_TO_ADDR(x) (x << 6)

static uint8_t seq_buf[SEQUENCE_LEN];
static uint8_t transmission[32];

void store_sequence(uint16_t loc) {
    if(loc > MAX_MEM_LOC)
        return;
    
    // copy the current sequence into local buffer
    taskENTER_CRITICAL();
    for(int i=0; i<SEQUENCE_LEN; i++)
        seq_buf[i] = sequence[i];
    taskEXIT_CRITICAL();
    
    loc = LOC_TO_ADDR(loc); // multiply by 64 to get base byte address

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

void load_sequence(uint16_t loc) {
    if(loc > MAX_MEM_LOC)
        return;

    loc = LOC_TO_ADDR(loc);

    Wire.beginTransmission(I2C_MEM_ADDR);
    Wire.write(loc >> 8);
    Wire.write(loc);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_MEM_ADDR, SEQUENCE_LEN/2);
    for(int i=0; i<32; i++) {
        seq_buf[i] = Wire.read();
    }
    loc+=32;
    Wire.beginTransmission(I2C_MEM_ADDR);
    Wire.write(loc >> 8);
    Wire.write(loc);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_MEM_ADDR, SEQUENCE_LEN/2);
    for(int i=32; i<64; i++) {
        seq_buf[i] = Wire.read();
    }
    Wire.endTransmission();

    taskENTER_CRITICAL();
    for(int i=0; i<SEQUENCE_LEN; i++) {
        sequence[i] = seq_buf[i];
    }
    taskEXIT_CRITICAL();

}