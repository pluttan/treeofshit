#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials are in config.h (gitignored)
// Copy src/config.h.example → src/config.h and fill in your values
#include "config.h"

// Конфигурация пинов
#define LED_PIN         3
#define SENSOR_PIN      4
#define NUM_LEDS        6
#define POV_LEDS        5       // LED 0-4 для POV, LED 5 - звезда
#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB
#define BRIGHTNESS      200

#define SEGMENTS        24

CRGB leds[NUM_LEDS];
WebServer server(80);

volatile unsigned long lastPulseTime = 0;
volatile unsigned long rotationPeriod = 0;
volatile bool newRotation = false;

uint8_t currentEffect = 0;
uint8_t currentStarEffect = 0;
#define NUM_EFFECTS 10
#define NUM_STAR_EFFECTS 5

uint8_t animOffset = 0;
bool isAPMode = false;

void IRAM_ATTR sensorISR() {
    unsigned long now = micros();
    rotationPeriod = now - lastPulseTime;
    lastPulseTime = now;
    newRotation = true;
}

uint16_t getCurrentSegment() {
    if (rotationPeriod == 0) return 0;
    unsigned long elapsed = micros() - lastPulseTime;
    return (elapsed * SEGMENTS / rotationPeriod) % SEGMENTS;
}

// ========== ЭФФЕКТЫ ЁЛКИ ==========

// 1. Радужная гирлянда
void effect_rainbowGarland() {
    uint16_t seg = getCurrentSegment();
    if (seg % 2 == 0) {
        uint8_t hue = (seg * 256 / SEGMENTS);
        for (int i = 0; i < POV_LEDS; i++) {
            leds[i] = CHSV(hue + i * 20, 255, 255);
        }
    } else {
        fill_solid(leds, POV_LEDS, CRGB::Black);
    }
}

// 2. Мерцающие огоньки
void effect_twinkle() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t val = sin8(seg * 30 + i * 60 + animOffset);
        if (val > 180) {
            leds[i] = CHSV((seg * 10 + i * 40) % 256, 255, val);
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

// 3. Красно-зелёная гирлянда
void effect_redGreen() {
    uint16_t seg = getCurrentSegment();
    if (seg % 2 == 0) {
        for (int i = 0; i < POV_LEDS; i++) {
            leds[i] = (i % 2 == 0) ? CRGB::Red : CRGB::Green;
        }
    } else {
        for (int i = 0; i < POV_LEDS; i++) {
            leds[i] = (i % 2 == 0) ? CRGB::Green : CRGB::Red;
        }
    }
}

// 4. Спираль радуги
void effect_spiral() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t hue = (seg * 256 / SEGMENTS + i * 50 + animOffset) % 256;
        leds[i] = CHSV(hue, 255, 255);
    }
}

// 5. Золотой дождь (сверху вниз)
void effect_goldenRain() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t brightness = sin8(seg * 15 + i * 50 + animOffset);
        leds[i] = CHSV(32, 255, brightness); // Золотой
    }
}

// 6. Северное сияние
void effect_aurora() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t hue = 100 + sin8(seg * 8 + i * 30 + animOffset) / 4; // Зелёно-голубой
        uint8_t brightness = sin8(seg * 12 - i * 25 + animOffset);
        leds[i] = CHSV(hue, 200, brightness);
    }
}

// 7. Огонь
void effect_fire() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t heat = sin8(seg * 20 + i * 40 + animOffset * 3);
        uint8_t hue = heat / 4; // 0-64 (красный-жёлтый)
        leds[i] = CHSV(hue, 255, heat);
    }
}

// 8. Снежинки (белые вспышки на синем)
void effect_snowflakes() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t val = sin8(seg * 25 + i * 70 + animOffset);
        if (val > 220) {
            leds[i] = CRGB::White;
        } else {
            leds[i] = CHSV(160, 200, 50); // Тёмно-синий фон
        }
    }
}

// 9. Конфетти (разноцветные вспышки)
void effect_confetti() {
    uint16_t seg = getCurrentSegment();
    for (int i = 0; i < POV_LEDS; i++) {
        uint8_t val = sin8(seg * 35 + i * 80 + animOffset);
        if (val > 200) {
            leds[i] = CHSV(random8(), 255, 255);
        } else {
            leds[i].fadeToBlackBy(30);
        }
    }
}

// 10. Пульсация по кругу
void effect_pulse() {
    uint16_t seg = getCurrentSegment();
    uint8_t brightness = sin8(seg * 10 + animOffset);
    uint8_t hue = animOffset;
    for (int i = 0; i < POV_LEDS; i++) {
        leds[i] = CHSV(hue + i * 10, 255, brightness);
    }
}

// ========== ЭФФЕКТЫ ЗВЕЗДЫ ==========

// 1. Пульсирующее золото
CRGB star_pulse() {
    uint8_t brightness = sin8(animOffset * 3);
    return CHSV(32, 255, brightness);
}

// 2. Мерцание (случайное)
CRGB star_sparkle() {
    uint8_t brightness = random8(150, 255);
    return CHSV(32, 255, brightness);
}

// 3. Радужная звезда
CRGB star_rainbow() {
    return CHSV(animOffset * 2, 255, 255);
}

// 4. Тёплое мерцание (как свеча)
CRGB star_candle() {
    uint8_t flicker = random8(200, 255);
    uint8_t hue = 20 + random8(20); // Оранжево-жёлтый
    return CHSV(hue, 255, flicker);
}

// 5. Белая вспышка
CRGB star_flash() {
    uint8_t val = sin8(animOffset * 5);
    if (val > 200) {
        return CRGB::White;
    }
    return CHSV(32, 255, 150); // Золотой фон
}

const char* effectNames[NUM_EFFECTS] = {
    "Rainbow Garland",
    "Twinkle",
    "Red-Green",
    "Spiral",
    "Golden Rain",
    "Aurora",
    "Fire",
    "Snowflakes",
    "Confetti",
    "Pulse"
};

const char* starNames[NUM_STAR_EFFECTS] = {
    "Pulse Gold",
    "Sparkle",
    "Rainbow",
    "Candle",
    "Flash"
};

// ========== ВЕБ-ИНТЕРФЕЙС ==========

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>POV Tree</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 20px;
            max-width: 400px;
            margin: 0 auto;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }
        h1, h2 {
            color: #fff;
            text-align: center;
            margin-bottom: 15px;
        }
        h1 { font-size: 24px; }
        h2 { font-size: 18px; color: #ffd700; margin-top: 20px; }
        .btn {
            display: block;
            width: 100%;
            padding: 12px 15px;
            margin: 8px 0;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
        }
        .btn.active {
            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
        }
        .star-btn {
            background: linear-gradient(135deg, #f5af19 0%, #f12711 100%);
        }
        .star-btn.active {
            background: linear-gradient(135deg, #fff 0%, #ffd700 100%);
            color: #333;
        }
        .status {
            text-align: center;
            color: #aaa;
            margin-top: 15px;
            font-size: 12px;
        }
        .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>POV Christmas Tree</h1>
        
        <h2>Tree Effects</h2>
        <div class="grid">
            <button class="btn" onclick="setEffect(0)">Rainbow</button>
            <button class="btn" onclick="setEffect(1)">Twinkle</button>
            <button class="btn" onclick="setEffect(2)">Red-Green</button>
            <button class="btn" onclick="setEffect(3)">Spiral</button>
            <button class="btn" onclick="setEffect(4)">Golden Rain</button>
            <button class="btn" onclick="setEffect(5)">Aurora</button>
            <button class="btn" onclick="setEffect(6)">Fire</button>
            <button class="btn" onclick="setEffect(7)">Snowflakes</button>
            <button class="btn" onclick="setEffect(8)">Confetti</button>
            <button class="btn" onclick="setEffect(9)">Pulse</button>
        </div>
        
        <h2>Star Effects</h2>
        <div class="grid">
            <button class="btn star-btn" onclick="setStar(0)">Pulse Gold</button>
            <button class="btn star-btn" onclick="setStar(1)">Sparkle</button>
            <button class="btn star-btn" onclick="setStar(2)">Rainbow</button>
            <button class="btn star-btn" onclick="setStar(3)">Candle</button>
            <button class="btn star-btn" onclick="setStar(4)">Flash</button>
        </div>
        
        <div class="status" id="status">Loading...</div>
    </div>
    <script>
        function setEffect(n) {
            fetch('/effect?id=' + n).then(r => r.text()).then(t => updateUI());
        }
        function setStar(n) {
            fetch('/star?id=' + n).then(r => r.text()).then(t => updateUI());
        }
        function updateUI() {
            fetch('/current').then(r => r.json()).then(d => {
                document.getElementById('status').textContent = 
                    'Tree: ' + d.effect + ' | Star: ' + d.star;
                document.querySelectorAll('.btn:not(.star-btn)').forEach((b,i) => {
                    b.classList.toggle('active', i === d.effectId);
                });
                document.querySelectorAll('.star-btn').forEach((b,i) => {
                    b.classList.toggle('active', i === d.starId);
                });
            });
        }
        updateUI();
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleEffect() {
    if (server.hasArg("id")) {
        int id = server.arg("id").toInt();
        if (id >= 0 && id < NUM_EFFECTS) {
            currentEffect = id;
            server.send(200, "text/plain", effectNames[currentEffect]);
            return;
        }
    }
    server.send(400, "text/plain", "Invalid");
}

void handleStar() {
    if (server.hasArg("id")) {
        int id = server.arg("id").toInt();
        if (id >= 0 && id < NUM_STAR_EFFECTS) {
            currentStarEffect = id;
            server.send(200, "text/plain", starNames[currentStarEffect]);
            return;
        }
    }
    server.send(400, "text/plain", "Invalid");
}

void handleCurrent() {
    String json = "{\"effectId\":" + String(currentEffect) + 
                  ",\"effect\":\"" + effectNames[currentEffect] + "\"" +
                  ",\"starId\":" + String(currentStarEffect) + 
                  ",\"star\":\"" + starNames[currentStarEffect] + "\"}";
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nPOV Christmas Tree Starting...");
    
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    
    pinMode(SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorISR, FALLING);
    
    Serial.print("Connecting to ");
    Serial.println(home_ssid);
    
    IPAddress staticIP(192, 168, 1, 251);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(192, 168, 1, 1);
    
    WiFi.mode(WIFI_STA);
    WiFi.config(staticIP, gateway, subnet, dns);
    WiFi.begin(home_ssid, home_password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        isAPMode = false;
        Serial.println("\nConnected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        isAPMode = true;
        Serial.println("\nAP Mode");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid, ap_password);
        Serial.print("AP: ");
        Serial.println(ap_ssid);
        Serial.print("IP: ");
        Serial.println(WiFi.softAPIP());
    }
    
    server.on("/", handleRoot);
    server.on("/effect", handleEffect);
    server.on("/star", handleStar);
    server.on("/current", handleCurrent);
    server.begin();
    
    Serial.println("Web server started!");
}

void loop() {
    server.handleClient();
    
    if (newRotation) {
        animOffset += 5;
        newRotation = false;
    }
    
    // Эффект ёлки
    switch (currentEffect) {
        case 0: effect_rainbowGarland(); break;
        case 1: effect_twinkle(); break;
        case 2: effect_redGreen(); break;
        case 3: effect_spiral(); break;
        case 4: effect_goldenRain(); break;
        case 5: effect_aurora(); break;
        case 6: effect_fire(); break;
        case 7: effect_snowflakes(); break;
        case 8: effect_confetti(); break;
        case 9: effect_pulse(); break;
    }
    
    // Эффект звезды
    switch (currentStarEffect) {
        case 0: leds[NUM_LEDS - 1] = star_pulse(); break;
        case 1: leds[NUM_LEDS - 1] = star_sparkle(); break;
        case 2: leds[NUM_LEDS - 1] = star_rainbow(); break;
        case 3: leds[NUM_LEDS - 1] = star_candle(); break;
        case 4: leds[NUM_LEDS - 1] = star_flash(); break;
    }
    
    FastLED.show();
    delayMicroseconds(50);
}
