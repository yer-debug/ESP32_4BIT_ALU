#include <Arduino.h>

#include "hardware/gpio.h"
#include "config/pins.h"


// ============================================================
// INTERNAL: WRITE 4-BIT VALUE
// ============================================================
//
// pins[0] = bit 0 (LSB)
// pins[3] = bit 3 (MSB)
//

static void writeNibble(const uint8_t pins[4], uint8_t value)
{
    value &= 0x0F;

    for (uint8_t i = 0; i < 4; i++)
    {
        digitalWrite(
            pins[i],
            (value >> i) & 0x01 ? HIGH : LOW
        );
    }
}


// ============================================================
// GPIO INITIALIZATION
// ============================================================

void gpioInit()
{
    // -------------------------
    // A[3:0]
    // -------------------------

    for (uint8_t i = 0; i < 4; i++)
    {
        pinMode(PINS_A[i], OUTPUT);
    }


    // -------------------------
    // B[3:0]
    // -------------------------

    for (uint8_t i = 0; i < 4; i++)
    {
        pinMode(PINS_B[i], OUTPUT);
    }


    // -------------------------
    // S[3:0]
    // -------------------------

    for (uint8_t i = 0; i < 4; i++)
    {
        pinMode(PINS_OP[i], OUTPUT);
    }


    // -------------------------
    // Cin
    // -------------------------

    pinMode(PIN_CIN, OUTPUT);


    // -------------------------
    // F[3:0] READBACK
    // -------------------------

#if ENABLE_F_READBACK

    for (uint8_t i = 0; i < 4; i++)
    {
        // F pins are driven by the physical ALU.
        pinMode(PINS_F[i], INPUT);
    }

#endif


    // -------------------------
    // SAFE INITIAL STATE
    // -------------------------

    write_a(0);
    write_b(0);
    write_operation(0);
    write_cin(false);
}


// ============================================================
// WRITE A
// ============================================================

void write_a(uint8_t value)
{
    writeNibble(PINS_A, value);
}


// ============================================================
// WRITE B
// ============================================================

void write_b(uint8_t value)
{
    writeNibble(PINS_B, value);
}


// ============================================================
// WRITE OPERATION SELECT
// ============================================================
//
// operation = S3:S0
//

void write_operation(uint8_t operation)
{
    writeNibble(PINS_OP, operation);
}


// ============================================================
// WRITE CIN
// ============================================================

void write_cin(bool cin)
{
    digitalWrite(
        PIN_CIN,
        cin ? HIGH : LOW
    );
}


// ============================================================
// READ ALU RESULT
// ============================================================
//
// Returns:
//
//     F3 F2 F1 F0
//
// as a normal 4-bit number.
//

uint8_t read_result()
{
    uint8_t value = 0;

#if ENABLE_F_READBACK

    for (uint8_t i = 0; i < 4; i++)
    {
        if (digitalRead(PINS_F[i]) == HIGH)
        {
            value |= (1 << i);
        }
    }

#endif

    return value & 0x0F;
}