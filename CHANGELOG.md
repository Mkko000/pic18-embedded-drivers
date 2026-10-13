# Changelog

All notable changes to this project are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and [Semantic Versioning](https://semver.org/).

## [Unreleased]
### Added
- `examples/` with minimal Struct API and Direct API GPIO examples.
- `CHANGELOG.md`.
### Changed
- README: fixed project-structure tree, added examples and changelog links.

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
