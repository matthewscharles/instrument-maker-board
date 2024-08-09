// Charles Matthews 2019

// For use with Arduino Leonardo, Touch Board etc. using our sensor shield
#include <Adafruit_NeoPixel.h>
#include <MIDIUSB.h>

// Constants for the neopixels
#define NUMPIXELS 6
#define PIN 12

// Sensor pins configuration
#define NUMSENSORS 6
int sensors[] = {A0, A1, A2, A3, A4, A5};

// Array to store the last sensor values to avoid duplicates
int lastValues[] = {-1, -1, -1, -1, -1, -1};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
MIDIEvent e;

void setup() {
  pixels.begin();
  Serial.begin(9600);
  
  // Set sensor pins as input
  for (int i = 0; i < NUMSENSORS; i++) {
    pinMode(sensors[i], INPUT);
  }
}

void loop() {
  int values[8];
  bool changeFlag = false;
 
  for (int i = 0; i < NUMSENSORS; i++) {
    // Read the sensor value, map it to a range of 0-127, and invert it
    values[i] = constrain((127 - (analogRead(sensors[i]) / 8) - 50), 0, 127); 
    //my current im boards only send a limited range, so boost this back up to 0-127
    values[i] = constrain(map(values[i], 0, 76, 0, 127), 0, 127);
    
    Serial.print(values[i]);
    Serial.print(" ");

    // Send MIDI message if sensor value has changed
    if (values[i] != lastValues[i]) {
      e.m1 = 176; // Control Change message on channel 1
      e.m2 = i;   // CC number based on sensor index
      e.m3 = values[i]; // CC value from the sensor
      e.type = 11; // Correct type for Control Change (CC)
                      
      MIDIUSB.write(e);
    
      lastValues[i] = values[i];
      
      // Update neopixel colors based on sensor values
      pixels.setPixelColor(i, pixels.Color(values[i] / 2, constrain(values[i] / 3 - 50, 0, 127), constrain(values[i] / 3 - 50, 0, 127)));
      changeFlag = true;
    }
  }
  
  Serial.println("inputs: ");
  
  if (changeFlag) {
    pixels.show();
  }
}
