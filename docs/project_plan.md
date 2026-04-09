# TCP1650 Project Plan

## Purpose

Build an Arduino library for TM1650 modules that:

- has no external dependencies beyond Arduino core and `Wire`
- exposes a mid-level user API
- supports decimal points
- supports raw button reads
- accepts SDA/SCL pin assignments at instance creation
- supports host-side CI tests without third-party test frameworks

## Target boards

- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi

## Phase 0 — Repository scaffold

Deliverables:

- repository layout
- Arduino library metadata
- MIT license
- initial source skeleton
- planning/specification documents
- host-side CI test scaffold
- example sketch placeholders

Exit criteria:

- repo is structurally ready for commits
- docs capture current goals and open risks
- CI runs host-side tests successfully

## Phase 1 — Requirements and design freeze

Deliverables:

- reviewed requirements specification
- reviewed design specification
- reviewed API plan
- hardware validation checklist

Key decisions:

- exact public API
- exact behavior when switching between 8-segment display mode and 7-segment key mode
- error handling policy for unsupported SDA/SCL assignments on a target board

Exit criteria:

- requirements and design approved
- traceable test list drafted from requirements
- no open architectural blockers remain

## Phase 2 — Core implementation

Deliverables:

- TM1650 register and transport layer
- display buffer management
- font/segment encoding support
- decimal position handling
- raw button read path
- mode-switch/restore policy

Exit criteria:

- library initializes and writes digits on target hardware
- raw button reads work on hardware
- display state survives button polling path

## Phase 3 — Tests and examples

Deliverables:

- requirement-traceable host-side tests
- example sketches for display, decimal point, and buttons
- manual hardware acceptance checklist results

Exit criteria:

- CI host tests pass
- basic examples work on supported boards
- key open risks are either retired or documented

## Open risk items

1. Decimal points require 8-segment mode, while key scanning appears to require 7-segment mode.
2. Mode switching may cause visible flicker or require a display refresh after key reads.
3. Runtime SDA/SCL assignment may be board-core specific even within the target Arduino families.
