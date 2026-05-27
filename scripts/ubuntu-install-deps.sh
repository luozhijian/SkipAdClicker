#!/usr/bin/env bash
set -euo pipefail

sudo apt update
sudo apt install -y \
  build-essential \
  cmake \
  ninja-build \
  pkg-config \
  qt6-base-dev \
  libopencv-dev \
  libtesseract-dev \
  libleptonica-dev \
  tesseract-ocr \
  tesseract-ocr-eng \
  libx11-dev \
  libxtst-dev

echo "Ubuntu dependencies installed."
