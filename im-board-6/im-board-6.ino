// For use with ATmega32u4-based boards such as the Arduino Leonardo
#include <Adafruit_NeoPixel.h>
#include <MIDIUSB.h>

// Constants for the NeoPixels
#define NUMPIXELS 6
#define PIN 12

// Sensor pins configuration
#define NUMSENSORS 6
const int sensors[NUMSENSORS] = {A0, A1, A2, A3, A4, A5};

// Array to store the last sensor values to avoid duplicates
int lastValues[NUMSENSORS] = {-1, -1, -1, -1, -1, -1};

// Initialize NeoPixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Optional: Debounce variables to prevent rapid MIDI messages
const unsigned long debounceDelay = 10; // milliseconds
unsigned long lastDebounceTime[NUMSENSORS] = {0};

// Helper function to send Control Change MIDI messages
void sendControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  midiEventPacket_t controlChange = {0x0B, 0xB0 | (channel - 1), control, value};
  MidiUSB.sendMIDI(controlChange);
  MidiUSB.flush(); // Ensure the MIDI message is sent immediately
}

void setup() {
  pixels.begin();
  Serial.begin(9600);
  
  // Set sensor pins as input
  for (int i = 0; i < NUMSENSORS; i++) {
    pinMode(sensors[i], INPUT);
  }
}

void loop() {
  int values[NUMSENSORS];
  bool changeFlag = false;
 
  for (int i = 0; i < NUMSENSORS; i++) {
    // Read the sensor value, map it to a range of 0-127, and invert it
    values[i] = constrain((127 - (analogRead(sensors[i]) / 8) - 50), 0, 127); 
    // Current IM boards only send a limited range, so boost this back up to 0-127
    values[i] = constrain(map(values[i], 0, 76, 0, 127), 0, 127);
    
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(values[i]);
    Serial.print(" | ");

    // Debounce check: Only send MIDI if enough time has passed since the last change
    if (values[i] != lastValues[i] && (millis() - lastDebounceTime[i] > debounceDelay)) {
      sendControlChange(1, i, values[i]); // Control Change on channel 1

      lastValues[i] = values[i];
      lastDebounceTime[i] = millis();
      
      // Update the NeoPixel colors based on sensor values (red only)
      pixels.setPixelColor(
        i, 
        pixels.Color(
          values[i] / 2, 
          constrain(values[i] / 3 - 50, 0, 127), 
          constrain(values[i] / 3 - 50, 0, 127)
        )
      );
      changeFlag = true;
    }
  }
  
  Serial.println("inputs: ");
  
  if (changeFlag) {
    pixels.show();
  }
}
