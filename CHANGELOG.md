# Changelog

All notable changes to this project are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and [Semantic Versioning](https://semver.org/).

## [Unreleased]
### Added
- `examples/` with minimal Struct API and Direct API GPIO examples.
- `CHANGELOG.md`.
### Changed
- README: fixed project-structure tree, added examples and changelog links.
- Projects and examples stop with a clear `#error` when the GPIO API (or LED driver) they use is disabled in `device_config.h`.
- PORTA valid-pin mask follows `OSC_MODE`: RA6/RA7 are excluded from port-wide operations unless `OSC_MODE_INTIO67` (they are OSC1/OSC2 and read back as 0).
### Fixed
- Struct API failed to build on XC8 v3 (error 712) when a `const` (flash) `pin_config_t` and a RAM one were both passed to it (e.g. Struct-only + LED driver); every Struct API function now works on a RAM copy of the config.
- `NOP()` was redefined over the one from `<xc.h>` (warning on every file).
- Single-pin APIs (Direct and Struct, and so the LED driver) accepted RA6/RA7 in XT/HS/HSPLL mode, where they are OSC2/OSC1; `IS_VALID_PORT_PIN` now rejects them with `E_NOT_OK` unless `OSC_MODE_INTIO67`, matching the PORTA port mask.
- No unused LAT/PORT tables or helpers when both GPIO APIs are disabled; removed an implicit-conversion warning in the LED driver.

## [2.1.0] - 2026-10-11
### Added
- Named port/pin mask constants (`GPIO_PORT_ALL_*`, `GPIO_PINx_MASK`).
- `Config/config_bits.c`: all configuration bits in one file, oscillator selection via `OSC_MODE` with compile-time frequency checks.
- `Projects/`: LED Chaser, 4-Way Traffic Light, Binary Counter, Morse Code.
### Changed
- Struct API and Direct API are fully independent (each can be enabled alone).
- Port toggle flips every existing pin.
### Fixed
- Traffic light starts in all-red at power-up; Morse inter-message gap is exactly 7 units.

## [1.0.0] - 2026-10-11
### Added
- Layered architecture (Config, Common, MCAL, ECU, Application).
- GPIO driver (pin/port, analog/digital, PORTB pull-ups) and LED driver.
