# TCP1650 project plan

## Goal

Create a small Arduino library for the TCP1650/TM1650 that supports:

- 4-digit numeric output
- one decimal point at a time
- brightness control
- display on/off
- raw button reads

## Phase 0 — repository and planning docs

Status: complete

Deliverables:
- Arduino library skeleton
- docs
- examples
- host-side test scaffold
- GitHub Actions scaffold

## Phase 1 — host-testable low-level core

Status: in progress

Immediate work:
- keep the Arduino/Wire layer thin
- move protocol behavior into a host-testable core
- add fake-transport host tests for exact register sequencing

Acceptance criteria:
- host tests verify exact write/read sequences for `begin()`, `setNumber()`, `setDot()`, and `getButtons()`
- the public wrapper delegates to the host-tested core
- only the thin Wire adapter remains outside host coverage

## Phase 2 — hardware shakeout

Tasks:
- run the hardware smoke sketch on target boards
- verify digit mapping
- verify brightness behavior
- verify one-dot behavior
- verify temporary 7-segment button reads and subsequent 8-segment display restore
- confirm the actual key-read address and any required settle timing on real hardware

## Target boards

Initial focus:
- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi