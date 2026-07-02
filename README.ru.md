![Header](header.png)

<div align="center">

# treeofshit

**POV-ёлка на ESP32-C3 и WS2812B**

[![License](https://img.shields.io/badge/license-MIT-2C2C2C?style=for-the-badge&labelColor=1E1E1E)](LICENSE)
[![ESP32-C3](https://img.shields.io/badge/ESP32--C3-RISC--V-2C2C2C?style=for-the-badge&logo=espressif&labelColor=1E1E1E)]()
[![FastLED](https://img.shields.io/badge/FastLED-3.6-2C2C2C?style=for-the-badge&labelColor=1E1E1E)]()
[![PlatformIO](https://img.shields.io/badge/PlatformIO-build-2C2C2C?style=for-the-badge&logo=platformio&labelColor=1E1E1E)]()

</div>

Контроллер рождественской ёлки на основе persistence-of-vision. Вращающийся рычаг с 5 светодиодами WS2812B и датчиком Холла создаёт объёмные эффекты из 24 угловых сегментов вокруг вертикальной оси. Шестой светодиод на вершине служит отдельной звездой с собственным набором эффектов. Включает встроенный веб-интерфейс для переключения эффектов по WiFi.

## ■ Возможности

- ❖ **10 эффектов ёлки** — Rainbow Garland, Twinkle, Red-Green, Spiral, Golden Rain, Aurora, Fire, Snowflakes, Confetti, Pulse
- ❖ **5 эффектов звезды** — Pulse Gold, Sparkle, Rainbow, Candle, Flash
- ❖ **POV-рендеринг** — 24 угловых сегмента, управляемых прерываниями от датчика Холла
- ❖ **Веб-интерфейс** — адаптивный glassmorphism UI для выбора эффектов по WiFi
- ❖ **Двойной режим WiFi** — подключается к домашней сети или поднимает собственную точку доступа (`CristmasTree`)
- ❖ **Смещение анимации** — эффекты плавно анимируются между оборотами

## ■ Стек

<div align="center">

| Компонент | Technology |
|-----------|------------|
| MCU | ESP32-C3 (RISC-V) |
| LED | WS2812B x6 (5 POV + 1 star) |
| Framework | Arduino + FastLED 3.6 |
| Build | PlatformIO |
| Датчик | Hall-effect (GPIO 4) |
| Интерфейс | Built-in HTTP web server |

</div>

## ■ Подключение

<div align="center">

| ESP32-C3 | Component |
|----------|-----------|
| GPIO 3 | WS2812B DIN |
| GPIO 4 | Hall sensor |
| 5V | LED VCC |
| GND | Common GND |

</div>

## ■ Запуск

```bash
# Сборка
pio run

# Прошивка
pio run -t upload

# Мониторинг
pio device monitor
```

После прошивки подключитесь к WiFi ESP32 и откройте веб-интерфейс для переключения эффектов ёлки и звезды.

## ■ Лицензия

MIT © [pluttan](https://github.com/pluttan)
