# Svartsten LM200 Amplifier

Svartsten LM200 is a 2 x 200 W low noise power amplifier based on lateral MOSFETs instead of traditional bipolar transistors. It uses a Raspberry Pi 3B+ for streaming services (Spotify, AirPlay) as well as hosting the UI interface. Additionally, for perfect sound the amplifier includes Dirac Live room correction technology.

## DAC

The Raspberry Pi 3B+ uses a USBStreamer from miniDSP as an audio interface which is further connected to a DAC AKM4440 board which is connected to the amplifier boards. The DAC and Raspberry Pi 3B+ use separate power supplies to minimize the noise from the Raspberry Pi. The USB cable between the Raspberry Pi 3B+ and USBStreamer is galvanically isolated using the ADUM4160 chip to minimize ground noise from the power hungry Raspberry Pi.

## Power supply

### High voltage power supply

The power supply uses a 625 VA toroidal transformer which is further rectified and filtered through a 10 x 1000 uF capacitor bank. For limiting the current at start-up it uses a 100 ohm resistor and two relays for soft start.

## Amplifier circuit

The amplifier board is designed by Rod Elliott (ESP), January 2004. The circuit uses lateral MOSFETs that are designed specifically for audio, and are far more linear than the more common switching devices that many MOSFET amps use.

## UI

A modern audio spectrum visualizer and control interface for the Svartsten LM200 amplifier, built with JUCE framework.

### Features

- **Real-time Audio Spectrum Analysis** - Visual frequency display of audio input
- **Amplifier Control Interface** - Toggle buttons for power, RCA input, and Moode audio
- **GUI** - Built with JUCE for desktop compatibility

### Requirements

- CMake 3.12+
- C++20 compiler
- JUCE framework (included as submodule)
- Raspberry Pi with GPIO (for hardware control)

### Quick Start

```bash
# Build the application
mkdir build && cd build
cmake ..
make
