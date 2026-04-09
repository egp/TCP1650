#!/usr/bin/env bash
set -euo pipefail
make -C "$(dirname "$0")/.." test
