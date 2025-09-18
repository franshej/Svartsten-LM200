# Svartsten LM200 Amplifier UI

A modern audio spectrum visualizer and control interface for the Svartsten LM200 amplifier, built with JUCE framework.

## Features

- **Real-time Audio Spectrum Analysis** - Visual frequency display of audio input
- **Amplifier Control Interface** - Toggle buttons for power, RCA input, and Moode audio
- **Cross-platform GUI** - Built with JUCE for desktop compatibility

## Requirements

- CMake 3.12+
- C++20 compiler
- JUCE framework (included as submodule)
- Raspberry Pi with GPIO (for hardware control)

## Quick Start

```bash
# Build the application
mkdir build && cd build
cmake ..
make
