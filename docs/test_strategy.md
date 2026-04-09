# TCP1650 Test Strategy

## 1. Goals

Provide a test strategy that:

- traces back to requirements
- runs in GitHub Actions on a host system
- does not depend on third-party test frameworks
- separates host-verifiable logic from hardware-only behavior

## 2. Test categories

### 2.1 Host-side logic tests
These tests are expected to run in CI and cover:

- segment encoding for supported characters
- decimal-position mapping
- display-buffer updates
- mode-switch sequencing policy using repository-local fakes
- API-level argument validation where possible

### 2.2 Hardware acceptance tests
These tests are expected to run manually on supported boards and cover:

- visible digit output
- visible decimal points
- brightness changes
- raw button reads
- behavior during mode switching between display and input

## 3. Requirements traceability

Initial mapping targets:

- R-013 -> tests for setting all four digits
- R-014 -> tests for decimal-position control
- R-015 -> tests for raw button-read entry point
- R-031/R-032 -> tests for mode-switch and restore sequencing
- R-042/R-043 -> CI-hosted tests for logic paths

A fuller traceability matrix will be added once the API and internal helpers are frozen.

## 4. CI approach

The repository shall include a host-side test harness based on:

- standard C++ compiler available in GitHub Actions
- local assertion helpers in the repo
- no third-party unit test framework

## 5. Phase 0 scope

Phase 0 provides only the test scaffold, smoke test, and CI wiring.
