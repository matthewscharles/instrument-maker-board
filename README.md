# Instrument Maker Board

Arduino code for use with the Instrument Maker library in Pd or more generic MIDI applications.  

This is intended as an entry point for rapid instrument prototyping, so that workshop attendees can experience building a physical controller before thinking about code and breadboarding, with their respective access barriers.

The result is a MIDI controller for use with up to 8 analog sensors, including optional NeoPixel visual feedback on the 6 sensor board.

## Hardware
The Instrument Maker Board is a shield for ATmega32u4 boards such as the Arduino Leonardo, Pro Micro, or Bare Conductive Touch Board. 

The board offers 6-8 voltage dividers hardwired to analog inputs, which are then intended to be sent over USB as MIDI.
If installed, a NeoPixel (WS2812) per channel displays the incoming sensor value mapped to brightness.

## Outputs
Channel 1: MIDI CC 1-6 (sensor range is compressed slightly) outputs a range of 0-127.

## Inputs
Channel 1: MIDI CC 1-6 emulates the sensor input, so that it can be recorded externally and played back for demo purposes.
Channel 16: MIDI CC 1-6 sets the hue for the corresponding NeoPixel (red by default).
