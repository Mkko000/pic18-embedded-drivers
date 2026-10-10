# 💡 01 · LED Chaser

Eight LEDs on **PORTC** play a playlist of lighting effects. The effects are stored in a **constant pattern table**, so you can add a new one by adding an array and one line to the table, without touching the main loop.

## ✨ Effects
| # | Effect | Frame time | Description |
|:-:|---|:-:|---|
| 1 | Knight Rider (bounce) | 60 ms | A single LED sweeps left ↔ right |
| 2 | Fill / Empty | 80 ms | Fills from RC0 to RC7, then empties |
| 3 | Center-out / in | 120 ms | Expands from the middle, then closes |
| 4 | Alternate | 300 ms | `0x55` ↔ `0xAA` |
| 5 | Running pair | 70 ms | Two adjacent LEDs run across |

## 🧰 Components
- PIC18F4620 + 4 MHz crystal (2 × 22 pF), 10 kΩ MCLR pull-up
- 8 × LED, 8 × 330 Ω resistors

## 🔌 Wiring
| Pin | Connection |
|---|---|
| RC0 … RC7 | LED0 … LED7 → 330 Ω → GND |

## ⚙️ How it works
- `gpio_init()` makes all pins digital, then `gpio_port_setup(PORTC_INDEX, 0x00, 0x00)` sets PORTC as all outputs, starting low.
- Each `effect_t` holds `{frames, count, step (×10 ms), repeat}`.
- `play_effect()` writes each frame with `gpio_port_write()`.

## 🛠️ Build (MPLAB X)

> **One project per MPLAB X build.** Every project (and the root `application.c`) defines its own `main()`, so add **only one** of them to a build.

1. New **Standalone Project**, device **PIC18F4620**, compiler **XC8**.
2. Add these files:
   - `Projects/01_LED_Chaser/main.c`
   - `MCAL_Layer/GPIO/hal_gpio.c`
3. Add the include directory: the repository root (so the `../../` paths resolve), or keep the folder structure.
4. Check `Config/device_config.h`: `GPIO_MODULE_ENABLE`, `GPIO_DIRECT_API_ENABLE` must be `CONFIG_ENABLE`.
5. Build and flash the `.hex`, or load it into **Proteus** (PIC18F4620, 4 MHz crystal, set *Program File* to the `.hex`).

## 🇪🇬 بالعربي
<div dir="rtl">

مشروع LED Chaser: ٨ لمبات على PORTC بتشغّل مجموعة تأثيرات ورا بعض، زي Knight Rider والملء والتفريغ ومن النص لبرة والتبديل. كل التأثيرات محطوطة في جدول ثابت، فلو عايز تضيف تأثير جديد تضيف Array وسطر واحد في الجدول، من غير ما تغيّر الـ main.

</div>
