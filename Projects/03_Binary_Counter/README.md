# 🔢 03 · Binary Counter (0–255)

An 8-bit counter shown in binary on **PORTC**, with **Up / Down / Reset** buttons on **PORTB**.

## ✨ Highlights
- **Internal pull-ups:** buttons connect straight to GND, so no external resistors are needed.
- **Software debounce:** a new level is accepted only after it stays stable for 4 × 5 ms = 20 ms.
- **Edge detection:** each press counts exactly once, even if the button is held.
- **Wrap-around:** Up goes 255 → 0, and Down goes 0 → 255.
- **Classic bug avoided:** `for (uint8 i = 0; i < 256; i++)` never ends, because a `uint8` is always `< 256`. The counter relies on the defined modulo-256 wrap of `uint8` instead.

## 🧰 Components
- PIC18F4620 + 4 MHz crystal, 10 kΩ MCLR pull-up
- 8 × LED + 8 × 330 Ω, 3 × push buttons

## 🔌 Wiring
| Pin | Connection |
|---|---|
| RC0 … RC7 | LED bit0 (LSB) … bit7 (MSB) → 330 Ω → GND |
| RB0 | **UP** button → GND |
| RB1 | **DOWN** button → GND |
| RB2 | **RESET** button → GND |

## ⚙️ How it works
Every 5 ms the program samples each button with `gpio_pin_read()` and passes it through `button_pressed()`, which handles debouncing and detects the HIGH → LOW edge. The counter value is then written to PORTC with `gpio_port_write()`.

## 🛠️ Build (MPLAB X)

> **One project per MPLAB X build.** Every project (and the root `application.c`) defines its own `main()`, so add **only one** of them to a build.

1. New **Standalone Project**, device **PIC18F4620**, compiler **XC8**.
2. Add these files:
   - `Projects/03_Binary_Counter/main.c`
   - `Config/config_bits.c` (configuration bits, add exactly once)
   - `MCAL_Layer/GPIO/hal_gpio.c`
3. Add the include directory: the repository root (so the `../../` paths resolve), or keep the folder structure.
4. Check `Config/device_config.h`: `GPIO_MODULE_ENABLE`, `GPIO_DIRECT_API_ENABLE`, `GPIO_PORTB_PULLUP_ENABLE` must be `CONFIG_ENABLE`.
5. Build and flash the `.hex`, or load it into **Proteus** (PIC18F4620, 4 MHz crystal, set *Program File* to the `.hex`).

## 🇪🇬 بالعربي
<div dir="rtl">

عداد ٨ بت من ٠ لـ ٢٥٥ بيظهر بالـ Binary على PORTC، وفيه ٣ زراير على PORTB: زيادة ونقصان وتصفير. الزراير شغالة بالـ Pull-up الداخلي، وعليها Debounce في السوفتوير، وكل ضغطة بتتحسب مرة واحدة بس حتى لو فضلت ضاغط. وكمان المشروع متفادي الغلطة المشهورة إن `uint8 < 256` تعمل لوب مالوش نهاية.

</div>
