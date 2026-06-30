![Header](header.png)

<div align="center">

# treeofshit

**POV Christmas tree with ESP32-C3 and WS2812B**

[![License](https://img.shields.io/badge/license-MIT-2C2C2C?style=for-the-badge&labelColor=1E1E1E)](LICENSE)
[![ESP32-C3](https://img.shields.io/badge/ESP32--C3-RISC--V-2C2C2C?style=for-the-badge&logo=espressif&labelColor=1E1E1E)]()
[![FastLED](https://img.shields.io/badge/FastLED-3.6-2C2C2C?style=for-the-badge&labelColor=1E1E1E)]()
[![PlatformIO](https://img.shields.io/badge/PlatformIO-build-2C2C2C?style=for-the-badge&logo=platformio&labelColor=1E1E1E)]()

</div>

Persistence-of-vision Christmas tree controller. A rotating arm with 5 WS2812B LEDs and a Hall-effect sensor renders volumetric 24-segment effects around a vertical axis. A 6th LED sits on top as a separate star with its own effect set. Includes a built-in web interface for switching effects over WiFi.

## ■ Features

- ❖ **10 tree effects** — Rainbow Garland, Twinkle, Red-Green, Spiral, Golden Rain, Aurora, Fire, Snowflakes, Confetti, Pulse
- ❖ **5 star effects** — Pulse Gold, Sparkle, Rainbow, Candle, Flash
- ❖ **POV rendering** — 24 angular segments driven by Hall-effect sensor interrupts
- ❖ **Web interface** — responsive glassmorphism UI for effect selection over WiFi
- ❖ **Dual WiFi mode** — connects to home network or falls back to its own AP (`CristmasTree`)
- ❖ **Animation offset** — effects animate smoothly across rotation cycles

## ■ Stack

| Component | Technology |
|-----------|------------|
| MCU | ESP32-C3 (RISC-V) |
| LED | WS2812B x6 (5 POV + 1 star) |
| Framework | Arduino + FastLED 3.6 |
| Build | PlatformIO |
| Sensor | Hall-effect (GPIO 4) |
| Interface | Built-in HTTP web server |

## ■ Wiring

| ESP32-C3 | Component |
|----------|-----------|
| GPIO 3 | WS2812B DIN |
| GPIO 4 | Hall sensor |
| 5V | LED VCC |
| GND | Common GND |

## ■ Usage

```bash
# Build
pio run

# Upload
pio run -t upload

# Monitor
pio device monitor
```

After flashing, connect to the ESP32's WiFi and open the web UI to switch between tree and star effects.

## ■ License

MIT © [pluttan](https://github.com/pluttan)
