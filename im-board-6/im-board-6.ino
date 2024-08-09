// Charles Matthews 2019

// For use with Leonardo, Touch Board etc. using our sensor shield
int sensors[] = {A0, A1, A2, A3, A4, A5, A6, A7};

// Set up an array so to avoid duplicate data later on
int lastValues[] = {-1, -1, -1, -1, -1, -1, -1, -1};

// Add neopixels
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS      6
#define PIN            12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#include <MIDIUSB.h>
MIDIEvent e;

void setup() {
  
  pixels.begin();
  Serial.begin(9600);
  
  for (int i = 0; i < 8; i++) {
    pinMode(sensors[i], INPUT);  
  }
}

void loop() {
  int values[8];

  bool changeFlag = false;
 
  for (int i = 0; i < 8; i++) {
  
    // Get the sensor value, squash down to 0-127, and invert it.
    
    values[i] = constrain((127 - (analogRead(sensors[i]) / 8) - 50), 0, 127); 
    
    // The current IM boards only send a limited range, so boost this back up to 0-127
    values[i] = constrain(map(values[i], 0, 76, 0, 127), 0, 127);
    
    Serial.print(values[i]);
    Serial.print(" ");
    
    if (values[i] != lastValues[i]) {
      // Construct a MIDI message -- generic control change starting from 0
      e.m1 = 176; //label as cc, channel 1
      e.m2 = i; //cc lane from sensor index number   
      e.m3 = values[i]; //set the value from the sensor
      e.type = 8; // should this be 11?
                      
      MIDIUSB.write(e);
    
      lastValues[i] = values[i];
    
      pixels.setPixelColor(i, pixels.Color(values[i] / 2, constrain(values[i] / 3 - 50, 0, 127), constrain(values[i] / 3 - 50, 0, 127))); 
      changeFlag = true;
    
    }
  }
  
  Serial.println("inputs: ");
  
  if (changeFlag) pixels.show();
  
}
