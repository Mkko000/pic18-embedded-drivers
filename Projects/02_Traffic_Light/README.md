# 🚦 02 · 4-Way Traffic Light

A 4-way junction with **12 LEDs** (N, S, E, W heads, each with Red/Yellow/Green). It runs as a **table-driven 6-state machine** with an **Emergency button**.

## 🔁 State Table
| # | State | N-S | E-W | Duration |
|:-:|---|:-:|:-:|:-:|
| 0 | `ST_NS_GREEN` | 🟢 | 🔴 | 10 s |
| 1 | `ST_NS_YELLOW` | 🟡 | 🔴 | 3 s |
| 2 | `ST_ALL_RED_1` | 🔴 | 🔴 | 2 s |
| 3 | `ST_EW_GREEN` | 🔴 | 🟢 | 10 s |
| 4 | `ST_EW_YELLOW` | 🔴 | 🟡 | 3 s |
| 5 | `ST_ALL_RED_2` | 🔴 | 🔴 | 2 s |

To change the timing or the lamp outputs, edit `tl_table[]` only.

## 🚨 Emergency mode
- **RB0** is a push button to GND, using the internal pull-up enabled by `gpio_portb_pullup_enable()` (active low).
- The loop runs in **50 ms ticks** and checks the button on every tick, so it reacts within about 50 ms. There are no long blocking delays.
- While the button is held, all heads show **RED** and the **yellow lamps flash** at 1 Hz.
- When the button is released, the cycle restarts from *all red* (the safe state).

## 🧰 Components
- PIC18F4620 + 4 MHz crystal, 10 kΩ MCLR pull-up
- 12 × LED (4 red, 4 yellow, 4 green), 12 × 330 Ω
- 1 × push button

## 🔌 Wiring
| Pin | Signal | Pin | Signal |
|---|---|---|---|
| RC0 | North Red | RD0 | East Red |
| RC1 | North Yellow | RD1 | East Yellow |
| RC2 | North Green | RD2 | East Green |
| RC3 | South Red | RD3 | West Red |
| RC4 | South Yellow | RD4 | West Yellow |
| RC5 | South Green | RD5 | West Green |
| RB0 | Emergency button → GND | | |

## 🛠️ Build (MPLAB X)

> **One project per MPLAB X build.** Every project (and the root `application.c`) defines its own `main()`, so add **only one** of them to a build.

1. New **Standalone Project**, device **PIC18F4620**, compiler **XC8**.
2. Add these files:
   - `Projects/02_Traffic_Light/main.c`
   - `MCAL_Layer/GPIO/hal_gpio.c`
3. Add the include directory: the repository root (so the `../../` paths resolve), or keep the folder structure.
4. Check `Config/device_config.h`: `GPIO_MODULE_ENABLE`, `GPIO_DIRECT_API_ENABLE`, `GPIO_PORTB_PULLUP_ENABLE` must be `CONFIG_ENABLE`.
5. Build and flash the `.hex`, or load it into **Proteus** (PIC18F4620, 4 MHz crystal, set *Program File* to the `.hex`).

## 🇪🇬 بالعربي
<div dir="rtl">

إشارة مرور لتقاطع فيه ٤ اتجاهات، بـ ١٢ لمبة: الشمال والجنوب على PORTC، والشرق والغرب على PORTD. الإشارة شغالة بـ State Machine فيها ٦ حالات، وكل الحالات ومددها متجمعة في جدول واحد. وفيه زرار طوارئ على RB0 شغال بالـ Pull-up الداخلي: طول ما هو مضغوط، كل الاتجاهات بتبقى أحمر والأصفر بيفلاش، ولما تسيبه الدورة بتبدأ من الأول من حالة "الكل أحمر" عشان الأمان.

</div>
