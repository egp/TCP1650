# TCP1650 project plan

## Goal

Create a small Arduino library for the TM1650 that supports:

- 4-digit decimal output
- 4-digit hexadecimal output
- one decimal point at a time
- brightness control
- display on/off
- raw button reads

## Phase 0 — repository and planning docs

Status: complete

Delivered:
- Arduino library skeleton
- docs
- examples
- host-side test scaffold
- GitHub Actions scaffold

## Phase 1 — host-testable low-level core

Status: complete

Delivered:
- thin Arduino/Wire layer
- host-testable device core
- fake-transport host tests for exact register sequencing
- power-state and failure-path host coverage

## Phase 2 — hardware shakeout

Status: substantially complete for smoke validation

Delivered:
- hardware smoke sketch
- verification of brightness ordering
- verification of decimal display
- verification of hex display
- verification of one-dot behavior
- verification of raw button stability
- verification of off/on cache behavior
- acceptable flicker in normal smoke-test use

## Phase 3 — remaining acceptance and release polish

Tasks:
- run the hardware acceptance checklist on each target board
- tighten any remaining board-specific notes
- keep docs aligned with final code
- push release commit
- tag initial release

## Target boards

Initial focus:
- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi