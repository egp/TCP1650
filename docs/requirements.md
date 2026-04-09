# TCP1650 Requirements Specification

## 1. Scope

TCP1650 shall provide an Arduino library for TM1650-based 4-digit LED display modules with raw button-read capability.

## 2. Constraints

### R-001 Dependencies
The library shall have no dependencies other than the standard Arduino core and the `Wire` library.

### R-002 Targets
The initial supported board set shall be Arduino Uno R3, Arduino Uno R4 Minima, and Arduino Uno R4 WiFi.

### R-003 Language and environment
The library shall be implemented as a conventional Arduino library using `.h` and `.cpp` files.

## 3. API Requirements

### R-010 Construction
The user shall be able to create an instance by supplying SDA and SCL pin assignments at instance creation.

### R-011 Initialization
The library shall provide an initialization operation that prepares the TM1650 device for use.

### R-012 Mid-level display API
The library shall provide a mid-level display API rather than requiring users to manually write per-digit device registers for ordinary usage.

### R-013 Set all digits
The library shall provide an operation to set all four digits in one command.

### R-014 Decimal-point control
The library shall provide an operation to set the decimal-point position independently of digit content.

### R-015 Raw buttons
The library shall provide an operation to read raw button state.

## 4. Functional Device Requirements

### R-020 Clear display
The library shall provide an operation to clear the display.

### R-021 Brightness
The library shall provide an operation to set display brightness.

### R-022 Refresh
The library shall maintain or restore the display state as needed after internal control operations.

### R-023 Font coverage
The library shall support at least a defined subset of displayable characters needed for digits and a minimal ASCII-like set appropriate for 7-segment rendering.

## 5. Mixed Display/Input Requirements

### R-030 Decimal-point support
The library shall support visible decimal points during normal display operation.

### R-031 Button-read policy
Because decimal points require 8-segment mode and key scanning appears to require 7-segment mode, the library shall define an explicit policy for switching modes when reading buttons.

### R-032 Restore after read
If button reads require temporary mode switching, the library shall restore the intended display mode and display contents after the read.

### R-033 Document limitations
Any visible flicker, timing limits, or hardware caveats associated with mode switching shall be documented.

## 6. Non-Functional Requirements

### R-040 Dynamic allocation
The core display and input path should avoid dynamic allocation.

### R-041 String dependency
The library shall not require Arduino `String` for its primary API.

### R-042 Testability
Core non-hardware logic shall be structured so it can be tested on a host system in GitHub Actions.

### R-043 CI
The repository shall provide a host-side CI path that runs without third-party test frameworks.

## 7. Documentation Requirements

### R-050 Planning documents
The repository shall include project planning, requirements, design, and test strategy documents under `docs/`.

### R-051 Examples
The repository shall include example sketches for the main library use cases.
