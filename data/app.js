// ============================================================
// 4-BIT ALU WEB CONTROLLER
// ============================================================
//
// Operation codes MUST match alu.cpp on the ESP32.
//
// 0  A
// 1  A + 1
// 2  A + B
// 3  A + B + 1
// 4  A - B - 1
// 5  A - B
// 6  A - 1
// 7  A AND B
// 8  A OR B
// 9  A XOR B
// 10 NOT A
// 11 SHIFT RIGHT A
// 12 SHIFT LEFT A
// ============================================================


// ============================================================
// Constants
// ============================================================

const MASK = 0x0F;


// ============================================================
// ALU Operations
// ============================================================

const OPS = [
    {
        code: 0,
        name: "A",
        fn: (a, b) => a
    },

    {
        code: 1,
        name: "A + 1",
        fn: (a, b) => (a + 1) & MASK
    },

    {
        code: 2,
        name: "A + B",
        fn: (a, b) => (a + b) & MASK
    },

    {
        code: 3,
        name: "A + B + 1",
        fn: (a, b) => (a + b + 1) & MASK
    },

    {
        code: 4,
        name: "A - B - 1",
        fn: (a, b) => (a - b - 1) & MASK
    },

    {
        code: 5,
        name: "A - B",
        fn: (a, b) => (a - b) & MASK
    },

    {
        code: 6,
        name: "A - 1",
        fn: (a, b) => (a - 1) & MASK
    },

    {
        code: 7,
        name: "A AND B",
        fn: (a, b) => (a & b) & MASK
    },

    {
        code: 8,
        name: "A OR B",
        fn: (a, b) => (a | b) & MASK
    },

    {
        code: 9,
        name: "A XOR B",
        fn: (a, b) => (a ^ b) & MASK
    },

    {
        code: 10,
        name: "NOT A",
        fn: (a, b) => (~a) & MASK
    },

    {
        code: 11,
        name: "Shift Right A",
        fn: (a, b) => (a >> 1) & MASK
    },

    {
        code: 12,
        name: "Shift Left A",
        fn: (a, b) => (a << 1) & MASK
    }
];


// ============================================================
// DOM Elements
// ============================================================

const $ = (id) => document.getElementById(id);

const aIn = $("a");
const bIn = $("b");
const opSel = $("op");

const runBtn = $("run");

const errorEl = $("error");
const hwEl = $("hw");
const linkEl = $("link");


// ============================================================
// Utility Functions
// ============================================================

function bin4(value)
{
    return value
        .toString(2)
        .padStart(4, "0");
}


function hex1(value)
{
    return value
        .toString(16)
        .toUpperCase();
}


// ============================================================
// Populate Operation Selector
// ============================================================

OPS.forEach((operation) =>
{
    const option = document.createElement("option");

    option.value = operation.code;
    option.textContent =
        `${operation.code}: ${operation.name}`;

    opSel.appendChild(option);
});


// Default operation = A + B
opSel.value = 2;


// ============================================================
// Read 4-Bit Input
// ============================================================

function readNibble(input)
{
    const raw = input.value.trim();

    if (!/^\d+$/.test(raw))
        return null;

    const value = Number(raw);

    if (value < 0 || value > 15)
        return null;

    return value;
}


// ============================================================
// Update Binary Input Display
// ============================================================

function refreshBinary()
{
    const a = readNibble(aIn);
    const b = readNibble(bIn);

    $("a-bin").textContent =
        a === null ? "----" : bin4(a);

    $("b-bin").textContent =
        b === null ? "----" : bin4(b);

    $("op-bin").textContent =
        bin4(Number(opSel.value));
}


// ============================================================
// Error Display
// ============================================================

function showError(message)
{
    errorEl.textContent = message;
    errorEl.hidden = !message;
}


// ============================================================
// Display Expected ALU Result
// ============================================================

function showExpected(value)
{
    value &= MASK;

    $("r-dec").textContent = value;

    $("r-bin").textContent =
        bin4(value);

    $("r-hex").textContent =
        hex1(value);


    // Physical-style 4-bit LED display
    //
    // LED order:
    // left  = F3
    // right = F0

    [...$("leds").children].forEach(
        (led, index) =>
        {
            const bit =
                (value >> (3 - index)) & 1;

            led.classList.toggle(
                "on",
                Boolean(bit)
            );
        }
    );
}


// ============================================================
// Hardware Status Display
// ============================================================

function setHardware(text, className = "")
{
    hwEl.textContent = text;

    hwEl.className =
        "hw" +
        (className ? ` ${className}` : "");
}


// ============================================================
// ESP32 Connection Status
// ============================================================

function setLink(connected)
{
    if (connected)
    {
        linkEl.textContent =
            "ESP32 connected";

        linkEl.className =
            "link up";
    }
    else
    {
        linkEl.textContent =
            "ESP32 not reachable";

        linkEl.className =
            "link down";
    }
}


// ============================================================
// Execute ALU Operation
// ============================================================

async function execute()
{
    showError("");

    const a = readNibble(aIn);
    const b = readNibble(bIn);
    const op = Number(opSel.value);


    // --------------------------------------------------------
    // Validate inputs
    // --------------------------------------------------------

    if (a === null || b === null)
    {
        showError(
            "A and B must be whole numbers from 0 to 15."
        );

        return;
    }


    // --------------------------------------------------------
    // Find selected operation
    // --------------------------------------------------------

    const operation = OPS.find(
        (item) => item.code === op
    );

    if (!operation)
    {
        showError(
            "Invalid ALU operation."
        );

        return;
    }


    // --------------------------------------------------------
    // Software reference calculation
    // --------------------------------------------------------

    const expected =
        operation.fn(a, b) & MASK;


    // Display expected result immediately
    showExpected(expected);


    // --------------------------------------------------------
    // Send request to ESP32
    // --------------------------------------------------------

    setHardware(
        "Sending to ESP32..."
    );

    runBtn.disabled = true;


    try
    {
        const url =
            `/api/alu?a=${a}&b=${b}&op=${op}`;


        const response =
            await fetch(
                url,
                {
                    method: "GET",
                    cache: "no-store"
                }
            );


        const data =
            await response.json();


        // ----------------------------------------------------
        // ESP32 rejected request
        // ----------------------------------------------------

        if (!response.ok || !data.ok)
        {
            throw new Error(
                data.error ||
                "ESP32 rejected the request."
            );
        }


        // ESP32 is reachable
        setLink(true);


        // ----------------------------------------------------
        // Hardware readback unavailable
        // ----------------------------------------------------

        if (data.hardware === null)
        {
            setHardware(
                "Operation sent to the physical ALU. Check the PCB LEDs and 7-segment display."
            );

            return;
        }


        // ----------------------------------------------------
        // Hardware result matches
        // ----------------------------------------------------

        if (data.hardware === expected)
        {
            showExpected(data.hardware);

            setHardware(
                `Hardware result: ${bin4(data.hardware)} — matches expected result.`,
                "ok"
            );

            return;
        }


        // ----------------------------------------------------
        // Hardware result does NOT match
        // ----------------------------------------------------

        showExpected(data.hardware);

        setHardware(
            `Mismatch: hardware = ${bin4(data.hardware)}, expected = ${bin4(expected)}.`,
            "bad"
        );
    }


    // --------------------------------------------------------
    // Network / ESP32 error
    // --------------------------------------------------------

    catch (error)
    {
        console.error(error);

        setLink(false);

        setHardware(
            "Could not reach the ESP32. The result shown above is software-only.",
            "bad"
        );
    }


    // --------------------------------------------------------
    // Re-enable button
    // --------------------------------------------------------

    finally
    {
        runBtn.disabled = false;
    }
}


// ============================================================
// Event Listeners
// ============================================================

[aIn, bIn].forEach(
    (element) =>
    {
        element.addEventListener(
            "input",
            refreshBinary
        );
    }
);


opSel.addEventListener(
    "change",
    refreshBinary
);


runBtn.addEventListener(
    "click",
    execute
);


// ============================================================
// Check ESP32 Connection
// ============================================================

async function checkConnection()
{
    try
    {
        const response =
            await fetch(
                "/api/status",
                {
                    method: "GET",
                    cache: "no-store"
                }
            );

        setLink(response.ok);
    }
    catch (error)
    {
        setLink(false);
    }
}


// ============================================================
// Initial Page Setup
// ============================================================

refreshBinary();

checkConnection();