// Charles Matthews 2024
// Instrument Board setup with MIDI input for colour change.

// For use with Arduino Leonardo or ATmega32u4-based boards.
#include <Adafruit_NeoPixel.h>
#include <MIDIUSB.h>

// Constants for the NeoPixels
#define NUMPIXELS 6
#define PIN 12

// Sensor pins configuration
#define NUMSENSORS 6
const int sensors[NUMSENSORS] = {A0, A1, A2, A3, A4, A5};


int lastValues[NUMSENSORS] = {-1, -1, -1, -1, -1, -1};
uint8_t hueValues[NUMSENSORS] = {0, 0, 0, 0, 0, 0};

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

// It's surprisingly difficult to map HSB to RGB for NeoPixels
// Here I have hardcoded the values for red, green, blue, and interpolate the rest.
/**
 * @brief Converts Hue and Brightness to a 24-bit RGB value (fixed saturation).
 * 
 * @param hue 0-255
 * @param brightness 0-255 
 */
uint32_t HBtoRGB(uint8_t hue, uint8_t val) {
    uint8_t r, g, b;

    if (hue <= 85) { // From Red to Green
        r = 255 - (hue * 3); // Decrease red
        g = hue * 3;         // Increase green
        b = 0;               // Blue stays 0
    } else if (hue <= 170) { // From Green to Blue
        r = 0;                // Red stays 0
        g = 255 - ((hue - 85) * 3); // Decrease green
        b = (hue - 85) * 3;        // Increase blue
    } else { // From Blue back to Red
        r = (hue - 170) * 3;       // Increase red
        g = 0;                     // Green stays 0
        b = 255 - ((hue - 170) * 3); // Decrease blue
    }

    // Apply brightness (value)
    r = (r * val) / 255;
    g = (g * val) / 255;
    b = (b * val) / 255;

    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

/**
 * @brief Extracts R, G, B components from a 24-bit RGB value and prints them.
 * 
 * @param rgb The 24-bit RGB value (0xRRGGBB).
 */
void printRGBComponents(uint32_t rgb) {
  uint8_t r = (rgb >> 16) & 0xFF; // Extract the Red component
  uint8_t g = (rgb >> 8) & 0xFF;  // Extract the Green component
  uint8_t b = rgb & 0xFF;         // Extract the Blue component
  
  Serial.print("RGB Array: [R: ");
  Serial.print(r);
  Serial.print(", G: ");
  Serial.print(g);
  Serial.print(", B: ");
  Serial.println(b);
}

void setup() {
  pixels.begin();
  Serial.begin(9600);
  
  for (int i = 0; i < NUMSENSORS; i++) {
    pinMode(sensors[i], INPUT);
  }
}

void loop() {
    int values[NUMSENSORS];
    bool changeFlag = false; // Flag to track if pixels need to be updated

    

    for (int i = 0; i < NUMSENSORS; i++) {
        values[i] = constrain((127 - (analogRead(sensors[i]) / 8) - 50), 0, 127);
        values[i] = constrain(map(values[i], 0, 76, 0, 127), 0, 127);

        if (values[i] != lastValues[i] || changeFlag) { // Update if there's a sensor change or a hue change via MIDI
            sendControlChange(1, i, values[i]); // Control Change on channel 1
            lastValues[i] = values[i];
            lastDebounceTime[i] = millis();
            
            uint32_t rgb = HBtoRGB(hueValues[i], values[i]);  // Use sensor value for brightness
            pixels.setPixelColor(i, rgb);
            changeFlag = true; // Set flag to update the display
        }
    }

    midiEventPacket_t rx = MidiUSB.read();
    while (rx.header != 0) {
        uint8_t status = rx.byte1 & 0xF0;  // Control Change status byte is 0xB0
        uint8_t channel = (rx.byte1 & 0x0F) + 1; // MIDI channels 1-16
        uint8_t control = rx.byte2;
        uint8_t value = rx.byte3;

        // Check for Control Change messages on channel 16, CC 1-6
        if (status == 0xB0 && channel == 16 && control >= 1 && control <= 6) {
            uint8_t sensorIndex = control - 1; // 0-based index
            hueValues[sensorIndex] = map(value, 0, 127, 0, 255); // Map CC value to hue (0-255)

            // Use the current sensor value for brightness to update the NeoPixel color
            uint32_t rgb = HBtoRGB(hueValues[sensorIndex], values[sensorIndex]);
            pixels.setPixelColor(sensorIndex, rgb);
            changeFlag = true;

            // // Debugging: Print received MIDI message and color values
            // Serial.print("Received MIDI CC ");
            // Serial.print(control);
            // Serial.print(" on channel ");
            // Serial.print(channel);
            // Serial.print(" with value ");
            // Serial.println(value);
            // printRGBComponents(rgb);
        } else if (status == 0xB0 && channel == 1 && control >= 1 && control <= 6) {
          uint8_t sensorIndex = control - 1; // 0-based index
          values[sensorIndex] = value;
          uint32_t rgb = HBtoRGB(hueValues[sensorIndex], value);  // Replace sensor value for brightness
          pixels.setPixelColor(sensorIndex, rgb);
          changeFlag = true;
        }
        
        rx = MidiUSB.read();  // Read next MIDI message
    }
    
    // Update the NeoPixels display if any change is flagged
    if (changeFlag) {
        pixels.show();
    }
}
