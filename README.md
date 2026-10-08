# ESP32_4BIT_ALU
4-Bit ALU — Advanced Design

1. Project Overview

This project is a physical 4-bit Arithmetic Logic Unit (ALU) controlled wirelessly by an ESP32.

The ESP32 does not perform the ALU calculations. It acts as the control and communication system:

                 Wi-Fi
                   │
                   ▼
        ┌─────────────────────┐
        │        ESP32        │
        │  Wi-Fi + Controller │
        └──────────┬──────────┘
                   │
        A[3:0] ────┤
        B[3:0] ────┤
        S[3:0] ────┤
        Cin ───────┤
                   ▼
        ┌─────────────────────┐
        │      4-Bit ALU      │
        │                     │
        │ Arithmetic Unit     │
        │ Logic Unit          │
        │ Shift Unit          │
        │ Control / MUX       │
        └──────────┬──────────┘
                   │
                   ▼
              Result[3:0]
                   │
                   ▼
             LEDs / Display

The main objective is to build the ALU as real digital hardware, while the ESP32 provides a modern wireless interface.

---

2. Main Objectives

The system is designed to demonstrate:

- Digital logic
- Boolean logic
- Binary arithmetic
- 4-bit addition
- 4-bit subtraction
- Two's-complement arithmetic
- Logic operations
- Bit shifting
- Multiplexing
- Control signals
- Microcontroller GPIO control
- Wi-Fi communication
- Hardware/software integration
- Digital computer architecture

The project can later be expanded toward a small processor datapath.

---

3. System Architecture

The complete system contains four major layers.

Layer 1 — User Interface

A phone, laptop, or PC connects to the ESP32 through Wi-Fi.

Example:

Phone / PC
     │
     │ Wi-Fi
     ▼
   ESP32

The user selects:

- A
- B
- Operation
- Cin

---

Layer 2 — ESP32 Controller

The ESP32 receives the command and converts it into digital control signals.

For example:

A = 5
B = 3
Operation = ADD
Cin = 0

The ESP32 produces:

A = 0101
B = 0011
S = 0001
Cin = 0

These signals are sent to the physical ALU.

The ESP32 does not calculate the result.

---

Layer 3 — Physical ALU

The physical ALU receives:

A[3:0]
B[3:0]
S[3:0]
Cin

It performs the selected hardware operation.

---

Layer 4 — Result

The ALU produces:

Result[3:0]

The result can be connected to:

- LEDs
- 7-segment display
- LCD
- Logic analyzer
- Oscilloscope
- Future processor datapath

The result does not need to return to the ESP32.

---

4. ALU Inputs and Outputs

Inputs

Signal| Width| Description
A| 4 bits| First operand
B| 4 bits| Second operand
S| 4 bits| Operation selection
Cin| 1 bit| Carry input

Total control/data inputs:

4 + 4 + 4 + 1 = 13 signals

Output

Signal| Width| Description
Result| 4 bits| ALU result

Optional future outputs:

Cout
Zero
Negative
Overflow
Carry

---

5. ALU Operation Table

The ALU uses four select bits:

S3 S2 S1 S0

and one carry input:

Cin

Complete operation table:

S3| S2| S1| S0| Cin| Operation
0| 0| 0| 0| 0| A
0| 0| 0| 0| 1| A + 1
0| 0| 0| 1| 0| A + B
0| 0| 0| 1| 1| A + B + 1
0| 0| 1| 0| 0| A - B - 1
0| 0| 1| 0| 1| A - B
0| 0| 1| 1| 0| A - 1
0| 0| 1| 1| 1| A
0| 1| 0| 0| X| A AND B
0| 1| 0| 1| X| A OR B
0| 1| 1| 0| X| A XOR B
0| 1| 1| 1| X| NOT A
1| 0| X| X| X| Shift Right A
1| 1| X| X| X| Shift Left A

"X" means the input is a don't-care condition.

---

6. Why S3, S2, S1, S0?

There are four select bits because the ALU contains more than eight possible operations.

Four binary bits provide:

2^4 = 16

possible operation codes.

The design currently uses:

0000 → Arithmetic group
0001 → Arithmetic group
0010 → Arithmetic group
0011 → Arithmetic group

0100 → AND
0101 → OR
0110 → XOR
0111 → NOT

10XX → Shift Right
11XX → Shift Left

This leaves additional operation codes available for future expansion.

---

7. Internal ALU Architecture

The ALU can be divided into four main blocks.

                 ┌──────────────────┐
A[3:0] ─────────►│ Arithmetic Unit  │
B[3:0] ─────────►│                  │
Cin ─────────────►│ Adder/Subtractor │
                 └────────┬─────────┘
                          │
                          │
                 ┌────────▼─────────┐
A[3:0] ─────────►│    Logic Unit    │
B[3:0] ─────────►│ AND OR XOR NOT   │
                 └────────┬─────────┘
                          │
                 ┌────────▼─────────┐
A[3:0] ─────────►│    Shift Unit    │
                 │   SHR / SHL      │
                 └────────┬─────────┘
                          │
                          ▼
                    ┌──────────┐
S[3:0] ────────────►│   MUX    │
                    └────┬─────┘
                         │
                         ▼
                    Result[3:0]

---

8. Arithmetic Unit

The arithmetic section is based on a 4-bit adder.

A basic structure is:

       A3 A2 A1 A0
        │  │  │  │
        ▼  ▼  ▼  ▼
      ┌───────────────┐
B ───►│ 4-Bit Adder   │
      └───────┬───────┘
              │
              ▼
          Result[3:0]

A 4-bit ripple-carry adder can be constructed from four full adders:

A0 ──┐
B0 ──┤ FA0 ── C1
Cin ─┘

A1 ──┐
B1 ──┤ FA1 ── C2
C1 ──┘

A2 ──┐
B2 ──┤ FA2 ── C3
C2 ──┘

A3 ──┐
B3 ──┤ FA3 ── Cout
C3 ──┘

---

9. Subtraction

Subtraction can be implemented using two's complement.

The fundamental equation is:

A - B = A + (~B) + 1

Therefore, the ALU can reuse the same adder used for addition.

For example:

A = 0101
B = 0011

To calculate:

A - B

invert B:

B      = 0011
~B     = 1100
~B + 1 = 1101

Then:

  0101
+ 1101
------
1 0010

The 4-bit result is:

0010

Therefore:

5 - 3 = 2

---

10. Logic Unit

The logic section performs bit-by-bit operations.

AND

A = 1010
B = 1100

A AND B = 1000

OR

A = 1010
B = 1100

A OR B = 1110

XOR

A = 1010
B = 1100

A XOR B = 0110

NOT

A = 1010

NOT A = 0101

Each operation is performed independently on all four bits.

---

11. Shift Unit

Shift Right

A = 1010

SHR A = 0101

The bits move one position toward the right.

The bit shifted out can optionally become a carry flag.

---

Shift Left

A = 0101

SHL A = 1010

The bits move one position toward the left.

The bit shifted out can optionally become a carry flag.

---

12. Control Logic

The select signals determine which ALU block is active.

S3 S2

00 → Arithmetic
01 → Logic
10 → Shift Right
11 → Shift Left

Then:

S1 S0

select the operation inside the corresponding group.

Conceptually:

                S3 S2
                  │
                  ▼
          ┌───────────────┐
          │ Group Select  │
          └───────┬───────┘
                  │
       ┌──────────┼──────────┐
       ▼          ▼          ▼
 Arithmetic      Logic      Shift
       │          │          │
       └──────────┼──────────┘
                  ▼
                MUX
                  │
                  ▼
             Result[3:0]

---

13. ESP32 Pin Mapping

The ESP32 is connected to the ALU control/data inputs as follows.

ALU Signal| ESP32 GPIO
A0| GPIO16
A1| GPIO17
A2| GPIO18
A3| GPIO19
B0| GPIO21
B1| GPIO22
B2| GPIO23
B3| GPIO25
S0| GPIO26
S1| GPIO27
S2| GPIO32
S3| GPIO33
Cin| GPIO13
GND| GND

Therefore:

A[3:0] → GPIO16,17,18,19

B[3:0] → GPIO21,22,23,25

S[3:0] → GPIO26,27,32,33

Cin → GPIO13

The ESP32 GPIOs are configurable digital I/O, but GPIO selection should account for ESP32-specific restrictions and boot/flash pins.

---

14. Ground Connection

The ESP32 and ALU must share a common reference.

ESP32 GND
    │
    ├──────── ALU GND
    │
    └──────── Logic circuit GND

Without a common ground, the digital HIGH and LOW levels may not be interpreted reliably.

---

15. Important Voltage Consideration

The ESP32 operates with approximately 3.3 V logic.

Do not connect a 5 V signal from the ALU into an ESP32 GPIO.

The ESP32 GPIO voltage tolerance is specified up to 3.6 V, so a 5 V signal should not be connected directly to an ESP32 input.

In this project the ESP32 is primarily sending signals to the ALU, so the ALU input circuitry must correctly recognize the ESP32's 3.3 V HIGH level.

If the ALU is built from a logic family that requires higher input voltage, use an appropriate level-shifting solution.

---

16. Wi-Fi Control

The ESP32 acts as a Wi-Fi server.

Conceptually:

Phone / PC
     │
     │ Wi-Fi
     ▼
ESP32 Web Server
     │
     ▼
Command Parser
     │
     ▼
GPIO Controller
     │
     ├── A[3:0]
     ├── B[3:0]
     ├── S[3:0]
     └── Cin
     │
     ▼
Physical ALU

A user could send a command such as:

A=5
B=3
OP=ADD
Cin=0

The ESP32 converts this into:

A = 0101
B = 0011
S = 0001
Cin = 0

and writes those values to the GPIO pins.

---

17. Example Operation

Suppose:

A = 5
B = 3
Operation = ADD
Cin = 0

Binary representation:

A = 0101
B = 0011

For ADD:

S3 S2 S1 S0 = 0001

Therefore:

S = 0001
Cin = 0

The physical ALU performs:

  0101
+ 0011
------
  1000

Result:

1000

which is:

8

---

18. Example GPIO State

For:

A = 0101
B = 0011
S = 0001
Cin = 0

the ESP32 outputs:

GPIO16 = 1   A0
GPIO17 = 0   A1
GPIO18 = 1   A2
GPIO19 = 0   A3

GPIO21 = 1   B0
GPIO22 = 1   B1
GPIO23 = 0   B2
GPIO25 = 0   B3

GPIO26 = 1   S0
GPIO27 = 0   S1
GPIO32 = 0   S2
GPIO33 = 0   S3

GPIO13 = 0    Cin

The physical ALU then produces:

Result = 1000

---

19. Result Display

The simplest result display is four LEDs.

Result[0] → LED0
Result[1] → LED1
Result[2] → LED2
Result[3] → LED3

Example:

Result = 1000

Only the LED representing bit 3 is ON.

For a more advanced version:

ALU
 │
 ├── Result[3:0] → LEDs
 │
 ├── Result[3:0] → 7-Segment
 │
 └── Result[3:0] → Logic Analyzer

---

20. Recommended Hardware Structure

A clean physical implementation should separate the ALU into modules.

┌─────────────────────────────┐
│        ESP32 BOARD           │
│                             │
│ Wi-Fi + GPIO Controller     │
└──────────────┬──────────────┘
               │
               │ 13 signals
               ▼
┌─────────────────────────────┐
│       INPUT / CONTROL       │
│                             │
│ A[3:0] B[3:0] S[3:0] Cin    │
└──────────────┬──────────────┘
               │
       ┌───────┼────────┐
       ▼       ▼        ▼
┌──────────┐ ┌────────┐ ┌─────────┐
│Arithmetic│ │ Logic  │ │  Shift  │
│  Unit    │ │  Unit  │ │  Unit   │
└────┬─────┘ └───┬────┘ └────┬────┘
     │           │           │
     └───────────┼───────────┘
                 ▼
             ┌───────┐
             │  MUX  │
             └───┬───┘
                 │
                 ▼
           Result[3:0]
                 │
                 ▼
             LED Display

---

21. Engineering Design Philosophy

The project should be developed progressively.

Stage 1 — Individual Logic

Build and test:

AND
OR
XOR
NOT

---

Stage 2 — Full Adder

Build:

Half Adder
      ↓
Full Adder
      ↓
4-Bit Ripple Carry Adder

---

Stage 3 — Subtractor

Implement:

A - B

using:

A + ~B + 1

---

Stage 4 — Arithmetic Unit

Combine:

A
A + 1
A + B
A + B + 1
A - B - 1
A - B
A - 1

---

Stage 5 — Logic Unit

Combine:

AND
OR
XOR
NOT

---

Stage 6 — Shift Unit

Implement:

SHR
SHL

---

Stage 7 — MUX / Control

Connect all units to the operation-selection system.

---

Stage 8 — ESP32

Connect:

ESP32 → ALU

and control the select/data signals digitally.

---

Stage 9 — Wi-Fi

Add:

Phone / PC
     ↓
Wi-Fi
     ↓
ESP32
     ↓
ALU

---

22. Testing Strategy

Every ALU operation should be tested independently.

Test 1 — Transfer

A = 0101
Operation = A

Expected:
0101

Test 2 — Addition

A = 0101
B = 0011

Expected:
1000

Test 3 — Subtraction

A = 0101
B = 0011

Expected:
0010

Test 4 — AND

A = 1010
B = 1100

Expected:
1000

Test 5 — OR

A = 1010
B = 1100

Expected:
1110

Test 6 — XOR

A = 1010
B = 1100

Expected:
0110

Test 7 — NOT

A = 1010

Expected:
0101

Test 8 — Shift Right

A = 1010

Expected:
0101

Test 9 — Shift Left

A = 0101

Expected:
1010

---

23. Future Improvements

This ALU can become the foundation of a much larger digital computer architecture.

Possible future additions:

4-bit ALU
   ↓
8-bit ALU
   ↓
16-bit ALU
   ↓
32-bit ALU
   ↓
Registers
   ↓
Register File
   ↓
Program Counter
   ↓
Instruction Decoder
   ↓
Control Unit
   ↓
Memory
   ↓
CPU Datapath

Other possible additions:

- Carry flag
- Zero flag
- Negative flag
- Overflow flag
- Status register
- Input registers
- Output registers
- Clock
- Instruction register
- RAM
- ROM
- Bus architecture
- Custom instruction set
- Custom CPU
- PCB implementation
- FPGA implementation

---

24. Advanced Version

The long-term architecture can become:

                    Wi-Fi
                      │
                      ▼
              ┌──────────────┐
              │     ESP32    │
              │ Communication│
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │ Control Unit │
              └──────┬───────┘
                     │
             ┌───────┴────────┐
             │                │
             ▼                ▼
        ┌─────────┐      ┌─────────┐
        │Registers│      │  Memory │
        └────┬────┘      └────┬────┘
             │                │
             └───────┬────────┘
                     ▼
               ┌───────────┐
               │    ALU    │
               └─────┬─────┘
                     │
                     ▼
                Result Bus

This transforms the project from a simple ALU experiment into a foundation for a custom computer architecture.

---

25. Project Philosophy

The important part of this project is that the ALU is not just a program that calculates numbers.

The calculation is performed by real digital hardware.

The ESP32 provides:

Communication
Control
Wi-Fi
GPIO
User Interface

while the ALU provides:

Arithmetic
Logic
Shifting
Digital computation

Therefore the project demonstrates the fundamental relationship between:

Software
   ↕
Microcontroller
   ↕
Digital Logic
   ↕
Computer Architecture
   ↕
Hardware

---

26. Final System

The complete system is:

              PHONE / PC
                  │
                  │ Wi-Fi
                  ▼
            ┌───────────┐
            │   ESP32   │
            └─────┬─────┘
                  │
       ┌──────────┼──────────┐
       │          │          │
      A[3:0]     B[3:0]    S[3:0]
       │          │          │
       └──────────┼──────────┘
                  │
                 Cin
                  │
                  ▼
        ┌────────────────────┐
        │       4-BIT ALU    │
        │                    │
        │ Arithmetic         │
        │ Logic              │
        │ Shift              │
        │ Control / MUX      │
        └─────────┬──────────┘
                  │
                  ▼
              Result[3:0]
                  │
                  ▼
            LEDs / Display

Final Objective

Build a working physical 4-bit ALU in which:

User
 ↓
Wi-Fi
 ↓
ESP32
 ↓
GPIO control signals
 ↓
Physical ALU
 ↓
Hardware calculation
 ↓
Result

The ESP32 controls the ALU, but the ALU itself performs the computation.
