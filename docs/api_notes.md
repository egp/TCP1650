# TCP1650 API Notes

## Planned mid-level operations

### Constructor

```cpp
TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire = Wire);
```

### Initialization

```cpp
bool begin();
```

### Display control

```cpp
bool clear();
bool setBrightness(uint8_t level);
bool setDigits(const char digits[4]);
bool setDigits(const uint8_t segments[4]);
bool setDecimalPosition(int8_t position);
bool refresh();
```

### Input

```cpp
uint8_t readButtonsRaw();
```

## Open API questions for Phase 1

1. Whether `setDigits(const char digits[4])` should interpret spaces and dot characters directly, or whether decimal points should remain entirely independent.
2. Whether `setDecimalPosition(-1)` is the best way to express “no decimal point”.
3. Whether the constructor should take `int` or `uint8_t` for pin identifiers for maximal Arduino compatibility.
4. Whether `readButtonsRaw()` should return `0` for no keys and use distinct bit positions for each key.
