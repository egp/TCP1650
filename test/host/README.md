# Host-side tests

This harness intentionally avoids third-party test frameworks.

It exercises the host-testable part of the library:
- digit-to-segment encoding
- cached display state
- control-byte generation
- exact low-level write/read sequencing for begin, setNumber, setDot, and getButtons

The thin Arduino/Wire adapter is intentionally left outside this harness.

## Run

```bash
make test