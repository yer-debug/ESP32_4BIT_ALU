#pragma once

#include <stdint.h>

// ============================================================
// HARDWARE GPIO INTERFACE
// ============================================================
//
// This file defines the interface between the ALU software
// and the physical ESP32 GPIO pins.
//
// A[3:0]  → ALU input A
// B[3:0]  → ALU input B
// S[3:0]  → ALU operation select
// Cin      → ALU carry input
// F[3:0]  → ALU result readback
//
// ============================================================


// -------------------------
// INITIALIZATION
// -------------------------

// Configure all ALU GPIO pins.
// A, B, S and Cin start at 0.
void gpioInit();


// -------------------------
// ALU INPUTS
// -------------------------

// Write 4-bit value to A[3:0].
// Only the lower 4 bits are used.
void write_a(uint8_t value);


// Write 4-bit value to B[3:0].
// Only the lower 4 bits are used.
void write_b(uint8_t value);


// -------------------------
// ALU CONTROL
// -------------------------

// Write S3:S0 operation select.
// Only the lower 4 bits are used.
void write_operation(uint8_t operation);


// Write carry input.
void write_cin(bool cin);


// -------------------------
// ALU RESULT
// -------------------------

// Read physical ALU result F[3:0].
// Returns a value from 0x0 to 0xF.
uint8_t read_result();