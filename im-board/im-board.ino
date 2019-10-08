//Charles Matthews 2019

//this is designed for a Pro Micro, and A4/A5 were not available for some reason
int sensors[] = {A0, A1, A2, A3, A8, A9, A6, A7};
//set up an array so to avoid duplicate data later on
int lastValues[] = {-1, -1, -1, -1, -1, -1, -1, -1};



//I'm lazily using the Bare Conductive Touch Board arcore setup here, need to confirm which MIDI USB library this uses
#include <MIDIUSB.h>
MIDIEvent e;


void setup() {
  //set all our sensor pins to input
  for (int i = 0; i < 8; i++) {
    pinMode(sensors[i], INPUT);  
  }
}

void loop() {
  //we'll put our sensor values here just in case we need to process at a later date
  int values[8];
 
 for (int i = 0; i < 8; i++) {
    //get the sensor value, squash down to 0-127, and invert it.
    //not much use treating this as an array for now, but anyway..
    values[i] = 127 - analogRead(sensors[i]) / 8; 
    //check against previous value; only send if changed.
    if (values[i] != lastValues[i]) {
      //construct a MIDI message -- generic control change
      //(this is for Pd; I'm not fussed about respecting GM conventions here, so start from 0)
        e.m1 = 176; //label as cc, channel 1
        e.m2 = i; //cc lane from sensor index number   
        e.m3 = values[i]; //set the value from the sensor
        e.type = 8; //what is this again? I'm rusty
                      
    MIDIUSB.write(e);
    //write to the array to check next time
    lastValues[i] = values[i];
    }
   
  }
  
  
  
}
