#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

#include "network/web_server.h"
#include "alu/alu.h"

static WebServer server(80);

// ============================================================
// Wi-Fi Access Point
// ============================================================

static const char* WIFI_SSID = "ESP32-4BIT-ALU";
static const char* WIFI_PASSWORD = "12345678";


// ============================================================
// Parse URL argument
// ============================================================

static bool parseArg(const char* name, long maxValue, uint8_t& out)
{
    if (!server.hasArg(name))
        return false;

    String value = server.arg(name);

    if (value.length() == 0)
        return false;

    char* end = nullptr;

    long number = strtol(value.c_str(), &end, 10);

    if (*end != '\0' || number < 0 || number > maxValue)
        return false;

    out = static_cast<uint8_t>(number);

    return true;
}


// ============================================================
// Send LittleFS file
// ============================================================

static void sendFile(const char* path, const char* contentType)
{
    File file = LittleFS.open(path, "r");

    if (!file)
    {
        server.send(
            404,
            "text/plain",
            "File not found"
        );

        return;
    }

    server.streamFile(file, contentType);

    file.close();
}


// ============================================================
// ALU API
// GET /api/alu?a=5&b=3&op=2
// ============================================================

static void handleAlu()
{
    uint8_t a;
    uint8_t b;
    uint8_t op;

    if (
        !parseArg("a", 15, a) ||
        !parseArg("b", 15, b) ||
        !parseArg("op", ALU_OP_COUNT - 1, op)
    )
    {
        server.send(
            400,
            "application/json",
            "{\"ok\":false,\"error\":\"a and b must be 0-15, op must be 0-12\"}"
        );

        return;
    }

    AluResult result = aluRun(
        static_cast<AluOp>(op),
        a,
        b
    );

    if (!result.ok)
    {
        server.send(
            400,
            "application/json",
            "{\"ok\":false,\"error\":\"Invalid ALU operation\"}"
        );

        return;
    }

    char json[256];

    if (result.hardwareValid)
    {
        snprintf(
            json,
            sizeof(json),
            "{"
            "\"ok\":true,"
            "\"a\":%u,"
            "\"b\":%u,"
            "\"op\":%u,"
            "\"expected\":%u,"
            "\"hardware\":%u,"
            "\"match\":%s"
            "}",
            result.a,
            result.b,
            static_cast<uint8_t>(result.op),
            result.expected,
            result.hardware,
            result.match ? "true" : "false"
        );
    }
    else
    {
        snprintf(
            json,
            sizeof(json),
            "{"
            "\"ok\":true,"
            "\"a\":%u,"
            "\"b\":%u,"
            "\"op\":%u,"
            "\"expected\":%u,"
            "\"hardware\":null,"
            "\"match\":null"
            "}",
            result.a,
            result.b,
            static_cast<uint8_t>(result.op),
            result.expected
        );
    }

    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// Start Web Server + Wi-Fi
// ============================================================

void webServerInit()
{
    // --------------------------------------------------------
    // LittleFS
    // --------------------------------------------------------

    Serial.println();
    Serial.println("Initializing LittleFS...");

    if (!LittleFS.begin(true))
    {
        Serial.println("ERROR: LittleFS mount failed.");
    }
    else
    {
        Serial.println("LittleFS mounted.");

        // Check required web files
        Serial.println();
        Serial.println("Checking web files...");

        if (LittleFS.exists("/index.html"))
        {
            Serial.println("[OK] index.html found");
        }
        else
        {
            Serial.println("[ERROR] index.html NOT found");
        }

        if (LittleFS.exists("/style.css"))
        {
            Serial.println("[OK] style.css found");
        }
        else
        {
            Serial.println("[ERROR] style.css NOT found");
        }

        if (LittleFS.exists("/app.js"))
        {
            Serial.println("[OK] app.js found");
        }
        else
        {
            Serial.println("[ERROR] app.js NOT found");
        }
    }

    // --------------------------------------------------------
    // Create ESP32 Wi-Fi Access Point
    // --------------------------------------------------------

    WiFi.mode(WIFI_AP);

    bool wifiStarted = WiFi.softAP(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    if (wifiStarted)
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("      ESP32 4-BIT ALU");
        Serial.println("==============================");

        Serial.println("Wi-Fi AP started");

        Serial.print("SSID: ");
        Serial.println(WIFI_SSID);

        Serial.print("Password: ");
        Serial.println(WIFI_PASSWORD);

        Serial.print("IP address: ");
        Serial.println(WiFi.softAPIP());
    }
    else
    {
        Serial.println("ERROR: Wi-Fi AP failed to start.");
    }

    // --------------------------------------------------------
    // Web pages
    // --------------------------------------------------------

    server.on(
        "/",
        HTTP_GET,
        []()
        {
            sendFile(
                "/index.html",
                "text/html"
            );
        }
    );

    server.on(
        "/style.css",
        HTTP_GET,
        []()
        {
            sendFile(
                "/style.css",
                "text/css"
            );
        }
    );

    server.on(
        "/app.js",
        HTTP_GET,
        []()
        {
            sendFile(
                "/app.js",
                "application/javascript"
            );
        }
    );

    // --------------------------------------------------------
    // Status API
    // --------------------------------------------------------

    server.on(
        "/api/status",
        HTTP_GET,
        []()
        {
            server.send(
                200,
                "application/json",
                "{\"ok\":true,\"service\":\"4-bit ALU\"}"
            );
        }
    );

    // --------------------------------------------------------
    // ALU API
    // --------------------------------------------------------

    server.on(
        "/api/alu",
        HTTP_GET,
        handleAlu
    );

    // --------------------------------------------------------
    // 404
    // --------------------------------------------------------

    server.onNotFound(
        []()
        {
            server.send(
                404,
                "text/plain",
                "Not found"
            );
        }
    );

    // --------------------------------------------------------
    // Start HTTP server
    // --------------------------------------------------------

    server.begin();

    Serial.println("Web server started.");
}


// ============================================================
// Handle HTTP requests
// ============================================================

void webServerHandle()
{
    server.handleClient();
}