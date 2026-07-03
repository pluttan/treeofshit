<div align="center">

# treeofshit

**POV Christmas tree with ESP32-C3 and WS2812B**


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

<div align="center">

| Component | Technology |
|-----------|------------|
| MCU | ESP32-C3 (RISC-V) |
| LED | WS2812B x6 (5 POV + 1 star) |
| Framework | Arduino + FastLED 3.6 |
| Build | PlatformIO |
| Sensor | Hall-effect (GPIO 4) |
| Interface | Built-in HTTP web server |

</div>

## ■ How It Works

```
1. Hall-effect sensor on GPIO 4 fires interrupts on each full rotation to track angular position.
2. The MCU divides each rotation into 24 angular segments timed by those interrupts.
3. FastLED renders the selected tree effect across the 5 POV LEDs for the current segment, animating offsets across cycles.
4. The 6th LED (star, mounted on top) independently runs its own star-effect set.
5. The built-in HTTP server serves a glassmorphism web UI; effect selection is applied immediately over WiFi.
```

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

## ■ Wiring

<div align="center">

| ESP32-C3 | Component |
|----------|-----------|
| GPIO 3 | WS2812B DIN |
| GPIO 4 | Hall sensor |
| 5V | LED VCC |
| GND | Common GND |

</div>

## ■ License

MIT © [pluttan](https://github.com/pluttan)
