// Charles Matthews 2019

#include <MIDIUSB.h>

int sensors[] = {A0, A1, A2, A3, A8, A9, A6, A7};
const int NUMSENSORS = 8;

// Array to store the last sensor values to avoid duplicates
int lastValues[NUMSENSORS] = {-1, -1, -1, -1, -1, -1, -1, -1};

const unsigned long debounceDelay = 10; // milliseconds
unsigned long lastDebounceTime[NUMSENSORS] = {0};

// Helper function to send Control Change MIDI messages
void sendControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  midiEventPacket_t controlChange = {0x0B, 0xB0 | (channel - 1), control, value};
  MidiUSB.sendMIDI(controlChange);
  MidiUSB.flush(); // Ensure the MIDI message is sent immediately
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Set sensor pins as input
  for (int i = 0; i < NUMSENSORS; i++) {
    pinMode(sensors[i], INPUT);  
  }
}

void loop() {
  int values[NUMSENSORS];
  
  for (int i = 0; i < NUMSENSORS; i++) {
    // Read the sensor value, map it to a range of 0-127, and invert it
    values[i] = constrain((127 - (analogRead(sensors[i]) / 8) - 50), 0, 127); 
    // Current IM boards only send a limited range, so boost this back up to 0-127
    values[i] = constrain(map(values[i], 0, 76, 0, 127), 0, 127);
    
    // Debugging: Print sensor values to Serial Monitor
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(values[i]);
    Serial.print(" | ");
    
    // Debounce check: Only send MIDI if enough time has passed since last change
    if (values[i] != lastValues[i] && (millis() - lastDebounceTime[i] > debounceDelay)) {
      sendControlChange(1, i, values[i]); // Control Change on channel 1
      
      // Update lastValues and lastDebounceTime
      lastValues[i] = values[i];
      lastDebounceTime[i] = millis();
    }
  }
  
  Serial.println("inputs: ");
  
}
