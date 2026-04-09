# TCP1650 Hardware Notes

## 1. Target modules

This project targets TM1650-based 4-digit LED modules with button input.

## 2. Known central constraint

The current working assumption is:

- decimal points require 8-segment mode
- button scanning requires 7-segment/key mode

This creates a design conflict that must be handled explicitly.

## 3. Initial policy direction

The current policy direction is:

- operate normally in decimal-point-capable display mode
- temporarily switch mode for raw button reads
- restore the display state immediately afterward

## 4. Validation items

Hardware validation must determine:

- whether the mode-switch assumption is correct on the target modules
- whether a delay is required before or after key reads
- whether digit registers must be rewritten after each key read
- whether visible flicker is acceptable
- whether target boards differ materially in observed behavior
