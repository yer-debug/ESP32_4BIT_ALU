#pragma once

#include <stdint.h>

// ============================================================
// ESP32 ↔ 4-BIT ALU PIN CONFIGURATION
// ============================================================
//
// Array index:
//   [0] = bit 0 = LSB
//   [3] = bit 3 = MSB
//
// Example:
//   PINS_A[0] → A0
//   PINS_A[3] → A3
//
// ============================================================


// -------------------------
// ALU INPUT A
// -------------------------

constexpr uint8_t PINS_A[4] = {
    13,     // A0 - LSB
    14,     // A1
    25,     // A2
    26      // A3 - MSB
};


// -------------------------
// ALU INPUT B
// -------------------------

constexpr uint8_t PINS_B[4] = {
    27,     // B0 - LSB
    32,     // B1
    33,     // B2
    23      // B3 - MSB
};


// -------------------------
// OPERATION SELECT
// -------------------------
//
// PINS_OP[0] = S0
// PINS_OP[1] = S1
// PINS_OP[2] = S2
// PINS_OP[3] = S3
//

constexpr uint8_t PINS_OP[4] = {
    16,     // S0
    17,     // S1
    18,     // S2
    19      // S3
};


// -------------------------
// CARRY INPUT
// -------------------------

constexpr uint8_t PIN_CIN = 21;


// -------------------------
// ALU RESULT READBACK
// -------------------------
//
// ESP32 reads the physical ALU result.
//
// PINS_F[0] = F0
// PINS_F[1] = F1
// PINS_F[2] = F2
// PINS_F[3] = F3
//
// D34 and D35 are input-only.
// D4 and D5 are used for the remaining result bits.
//

#define ENABLE_F_READBACK 0

constexpr uint8_t PINS_F[4] = {
    34,     // F0 - LSB
    35,     // F1
    4,      // F2
    5       // F3 - MSB
};


// -------------------------
// ALU SETTLE TIME
// -------------------------

// Time allowed for the physical ALU logic
// to stabilize before reading F.

constexpr uint16_t ALU_SETTLE_MS = 20;