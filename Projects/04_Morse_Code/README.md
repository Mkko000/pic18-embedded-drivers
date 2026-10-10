# 📡 04 · Morse Code Transmitter

This project sends any text as **International Morse code** on an LED, and optionally a buzzer. The demo sends **"SOS"** and then **"MOHAMMED"**, over and over, with an exact 7-unit word gap between messages.

## ⏱️ Timing (one constant: `UNIT_MS`)
| Element | Units | @ 150 ms |
|---|:-:|:-:|
| Dot | 1 | 150 ms |
| Dash | 3 | 450 ms |
| Gap inside a letter | 1 | 150 ms |
| Gap between letters | 3 | 450 ms |
| Gap between words | 7 | 1050 ms |

To change the speed, set `UNIT_MS = 1200 / WPM` (PARIS standard).

## ✨ Highlights
- **Lookup tables:** `A–Z` and `0–9` are stored as `'.'` / `'-'` strings.
- **Any text:** lowercase is converted to uppercase, a space gives a word gap, and unsupported characters are skipped.
- Uses the **LED driver** (`led_on` / `led_off`), which sits on top of the GPIO driver.

## 🧰 Components
- PIC18F4620 + 4 MHz crystal, 10 kΩ MCLR pull-up
- 1 × LED + 330 Ω (optional: an active buzzer driven through an NPN transistor on the same pin)

## 🔌 Wiring
| Pin | Connection |
|---|---|
| RD0 | LED → 330 Ω → GND (optional buzzer via transistor) |

## 💻 Usage
```c
morse_send_string("HELLO WORLD");
```

## 🛠️ Build (MPLAB X)

> **One project per MPLAB X build.** Every project (and the root `application.c`) defines its own `main()`, so add **only one** of them to a build.

1. New **Standalone Project**, device **PIC18F4620**, compiler **XC8**.
2. Add these files:
   - `Projects/04_Morse_Code/main.c`
   - `Config/config_bits.c` (configuration bits, add exactly once)
   - `MCAL_Layer/GPIO/hal_gpio.c`
   - `ECU_Layer/LED/ecu_led.c`
3. Add the include directory: the repository root (so the `../../` paths resolve), or keep the folder structure.
4. Check `Config/device_config.h`: `GPIO_MODULE_ENABLE`, `GPIO_DIRECT_API_ENABLE`, `LED_MODULE_ENABLE` must be `CONFIG_ENABLE`.
5. Build and flash the `.hex`, or load it into **Proteus** (PIC18F4620, 4 MHz crystal, set *Program File* to the `.hex`).

## 🇪🇬 بالعربي
<div dir="rtl">

مشروع بيبعت أي نص بشفرة مورس على لمبة على RD0، وممكن كمان على Buzzer. الحروف من A لـ Z والأرقام من 0 لـ 9 متخزنة في جدول، وكل التوقيتات محسوبة من ثابت واحد اسمه `UNIT_MS`: النقطة وحدة، والشَرطة ٣، والمسافة بين الحروف ٣، والمسافة بين الكلمات ٧. المثال اللي في المشروع بيبعت "SOS" وبعدها "MOHAMMED".

</div>
