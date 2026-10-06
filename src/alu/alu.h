#pragma once

#include <stdint.h>

// ============================================================
// 4-BIT ALU OPERATIONS
// ============================================================
//
// Control signals:
//     S3 S2 S1 S0 + Cin
//
// Result:
//     F[3:0]
//
// ============================================================

enum AluOp : uint8_t
{
    ALU_PASS_A,          // A
    ALU_INC_A,           // A + 1

    ALU_ADD,             // A + B
    ALU_ADD_CIN,         // A + B + 1

    ALU_SUB_B_MINUS_1,   // A - B - 1
    ALU_SUB_B,           // A - B
    ALU_DEC_A,           // A - 1

    ALU_AND,             // A AND B
    ALU_OR,              // A OR B
    ALU_XOR,             // A XOR B
    ALU_NOT_A,           // NOT A

    ALU_SHR,             // Shift right A
    ALU_SHL              // Shift left A
};

constexpr uint8_t ALU_OP_COUNT = 13;
constexpr uint8_t ALU_MASK     = 0x0F;


// ============================================================
// ALU CONTROL
// ============================================================
//
// This is what the ESP32 sends to the physical ALU.
//
// operation:
//     S3 S2 S1 S0
//
// cin:
//     Cin
//
// ============================================================

struct AluControl
{
    uint8_t operation;   // S3:S0
    bool    cin;
};


// Convert an operation into the physical ALU control signals.
AluControl aluGetControl(AluOp op);


// Check whether an operation is valid.
bool aluIsValidOp(uint8_t op);


// Human-readable operation name.
const char* aluOpName(uint8_t op);


// ============================================================
// SOFTWARE REFERENCE MODEL
// ============================================================
//
// Calculates what the physical ALU SHOULD produce.
//
// Only the lower 4 bits are returned because this is a 4-bit ALU.
//

uint8_t aluCompute(
    AluOp op,
    uint8_t a,
    uint8_t b
);


// ============================================================
// HARDWARE TEST RESULT
// ============================================================

#ifdef ARDUINO

struct AluResult
{
    bool    ok;

    uint8_t a;
    uint8_t b;

    AluOp   op;

    uint8_t expected;

    bool    hardwareValid;
    uint8_t hardware;

    bool    match;
};


// ============================================================
// RUN PHYSICAL ALU
// ============================================================
//
// 1. Send A to ALU
// 2. Send B to ALU
// 3. Send S3:S0
// 4. Send Cin
// 5. Wait for ALU_SETTLE_MS
// 6. Read F[3:0]
// 7. Compare hardware result with expected result
//

AluResult aluRun(
    AluOp op,
    uint8_t a,
    uint8_t b
);

#endif