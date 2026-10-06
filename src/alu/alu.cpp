#include "alu/alu.h"

#ifdef ARDUINO
#include <Arduino.h>
#include "config/pins.h"
#include "hardware/gpio.h"
#endif


// ============================================================
// OPERATION VALIDATION
// ============================================================

bool aluIsValidOp(uint8_t op)
{
    return op < ALU_OP_COUNT;
}


// ============================================================
// OPERATION NAMES
// ============================================================

const char* aluOpName(uint8_t op)
{
    switch (static_cast<AluOp>(op))
    {
        case ALU_PASS_A:        return "A";
        case ALU_INC_A:         return "A + 1";

        case ALU_ADD:           return "A + B";
        case ALU_ADD_CIN:       return "A + B + 1";

        case ALU_SUB_B_MINUS_1: return "A - B - 1";
        case ALU_SUB_B:         return "A - B";
        case ALU_DEC_A:         return "A - 1";

        case ALU_AND:           return "A AND B";
        case ALU_OR:            return "A OR B";
        case ALU_XOR:           return "A XOR B";
        case ALU_NOT_A:         return "NOT A";

        case ALU_SHR:           return "Shift Right";
        case ALU_SHL:           return "Shift Left";

        default:                return "Invalid";
    }
}


// ============================================================
// OPERATION → PHYSICAL ALU CONTROL
// ============================================================
//
// Returns:
//     operation = S3 S2 S1 S0
//     cin       = Cin
//
// Based on the actual ALU control table.
//
// ============================================================

AluControl aluGetControl(AluOp op)
{
    switch (op)
    {
        // -------------------------
        // Arithmetic
        // -------------------------

        case ALU_PASS_A:
            // 0000, Cin = 0 → A
            return {0b0000, false};

        case ALU_INC_A:
            // 0000, Cin = 1 → A + 1
            return {0b0000, true};

        case ALU_ADD:
            // 0001, Cin = 0 → A + B
            return {0b0001, false};

        case ALU_ADD_CIN:
            // 0001, Cin = 1 → A + B + 1
            return {0b0001, true};

        case ALU_SUB_B_MINUS_1:
            // 0010, Cin = 0 → A - B - 1
            return {0b0010, false};

        case ALU_SUB_B:
            // 0010, Cin = 1 → A - B
            return {0b0010, true};

        case ALU_DEC_A:
            // 0011, Cin = 0 → A - 1
            return {0b0011, false};


        // -------------------------
        // Logic
        // -------------------------

        case ALU_AND:
            // 0100, Cin = don't care
            return {0b0100, false};

        case ALU_OR:
            // 0101, Cin = don't care
            return {0b0101, false};

        case ALU_XOR:
            // 0110, Cin = don't care
            return {0b0110, false};

        case ALU_NOT_A:
            // 0111, Cin = don't care
            return {0b0111, false};


        // -------------------------
        // Shifts
        // -------------------------

        case ALU_SHR:
            // 10XX → Shift Right
            // Choose 1000 because S1:S0 are don't care.
            return {0b1000, false};

        case ALU_SHL:
            // 11XX → Shift Left
            // Choose 1100 because S1:S0 are don't care.
            return {0b1100, false};


        default:
            // Safe default
            return {0b0000, false};
    }
}


// ============================================================
// SOFTWARE REFERENCE MODEL
// ============================================================
//
// This calculates what the physical ALU SHOULD produce.
//
// Only 4 bits are returned.
// ============================================================

uint8_t aluCompute(AluOp op, uint8_t a, uint8_t b)
{
    a &= ALU_MASK;
    b &= ALU_MASK;

    switch (op)
    {
        case ALU_PASS_A:
            return a;

        case ALU_INC_A:
            return (a + 1) & ALU_MASK;


        case ALU_ADD:
            return (a + b) & ALU_MASK;

        case ALU_ADD_CIN:
            return (a + b + 1) & ALU_MASK;


        case ALU_SUB_B_MINUS_1:
            return (a - b - 1) & ALU_MASK;

        case ALU_SUB_B:
            return (a - b) & ALU_MASK;

        case ALU_DEC_A:
            return (a - 1) & ALU_MASK;


        case ALU_AND:
            return a & b;

        case ALU_OR:
            return a | b;

        case ALU_XOR:
            return a ^ b;

        case ALU_NOT_A:
            return (~a) & ALU_MASK;


        case ALU_SHR:
            return (a >> 1) & ALU_MASK;

        case ALU_SHL:
            return (a << 1) & ALU_MASK;


        default:
            return 0;
    }
}


#ifdef ARDUINO

// ============================================================
// RUN PHYSICAL ALU
// ============================================================
//
// 1. Validate inputs
// 2. Convert operation → S3:S0 + Cin
// 3. Send A
// 4. Send B
// 5. Send operation
// 6. Send Cin
// 7. Wait for ALU
// 8. Read F
// 9. Compare hardware with expected
// ============================================================

AluResult aluRun(AluOp op, uint8_t a, uint8_t b)
{
    AluResult r = {};

    r.a = a;
    r.b = b;
    r.op = op;

    // -------------------------
    // Validate
    // -------------------------

    if (!aluIsValidOp(static_cast<uint8_t>(op)) ||
        a > ALU_MASK ||
        b > ALU_MASK)
    {
        r.ok = false;
        return r;
    }


    // -------------------------
    // Get physical control
    // -------------------------

    AluControl control = aluGetControl(op);


    // -------------------------
    // Send inputs to ALU
    // -------------------------

    write_a(a);
    write_b(b);

    write_operation(control.operation);
    write_cin(control.cin);


    // -------------------------
    // Allow ALU to settle
    // -------------------------

    delay(ALU_SETTLE_MS);


    // -------------------------
    // Software expected result
    // -------------------------

    r.ok = true;

    r.expected = aluCompute(op, a, b);


    // -------------------------
    // Read physical ALU result
    // -------------------------

#if ENABLE_F_READBACK

    r.hardwareValid = true;

    r.hardware = read_result();

    r.match = (r.hardware == r.expected);

#else

    r.hardwareValid = false;
    r.hardware = 0;
    r.match = false;

#endif

    return r;
}

#endif