# TCP1650 API notes

## Public class

```cpp
class TCP1650 {
public:
  explicit TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire = Wire);

  bool begin();
  bool displayOn();
  bool displayOff();
  bool setBrightness(uint8_t level);
  bool setNumber(uint16_t value, bool leadingZeros = false);
  bool setDot(uint8_t position, bool on);
  uint8_t getButtons();
};