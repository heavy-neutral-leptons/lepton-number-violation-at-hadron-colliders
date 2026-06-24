#!/bin/bash
# Copyright 2023-2026 Bruno M. S. Oliveira

set -e

python madanalyse-trimuon-fcchh.py
python madanalyse-dimuonss-fcchh.py

python madplot-signal-background.py
