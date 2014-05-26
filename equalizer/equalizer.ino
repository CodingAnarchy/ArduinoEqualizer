#define OCTAVE 1  // use the octave output function
#define FHT_N 256 // set to 256 point FHT
#define PIN1 6


#include <FHT.h>
#include <Adafruit_NeoPixel.h>
#include "led.h"
#include "fht.h"

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(60, PIN1, NEO_GRB + NEO_KHZ800);
// Setup system
void setup(){
  led_setup();
  fht_setup();
  strip1.begin();
  strip1.show();  // Show all pixels as 'off'

}

// Main operating loop
void loop(){
  // Initialize variables
  boolean frequency = true;
  int bass, low_mid, high_mid, treble;
  byte outputs = 4;
  boolean test = true;
  unsigned long time = 0;
  unsigned long prev_time = 0;
  
  while(1) {
    // TODO: Check for current system settings status - various switches and options
    
    // Operate FHT ADC to obtain current frequency levels - if selected
    if (frequency) {
      cli();  // UDRE interrupt slows this way down on arduino1.0
      TIMSK0 = 0; // turn off timer0 for lower jitter
      
      for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
        while(!(ADCSRA & 0x10)); // wait for adc to be ready
        ADCSRA = 0xf5; // restart adc
        byte m = ADCL; // fetch adc data
        byte j = ADCH;
        int k = (j << 8) | m; // form into an int
        k -= 0x0200; // form into a signed int
        k <<= 6; // form into a 16b signed int
        fht_input[i] = k; // put real data into bins
      }
      sei();
      TIMSK0 = 1; // turn timer0 back on for timing measurements - may be needed
      
      fht_window(); // window the data for better frequency response
      fht_reorder(); // reorder the data before doing the fht
      fht_run(); // process the data in the fht
      fht_mag_octave(); // take the output of the fht
      
      if (outputs == 4){
        bass = (fht_oct_out[0] + fht_oct_out[1]) / 2;
        low_mid = (fht_oct_out[2] + fht_oct_out[3]) / 2;
        high_mid = (fht_oct_out[4] + fht_oct_out[5]) / 2;
        treble = (fht_oct_out[6] + fht_oct_out[7]) / 2;
        
        // Normalize groupings to obtain outputs
        bass = (int)(((float)bass / 256.0) * 60.0);
        low_mid = (int)(((float)low_mid / 256.0) * 60.0);
        high_mid = (int)(((float)high_mid / 256.0) * 60.0);
        treble = (int)(((float)treble / 256.0) * 60.0);
      }
      
    }
    
    else {
      // Read in volume output and key off of that
    }
    
    if (test){
      led_loop();
    }
    else{
      if (outputs == 4){
        for(int i = 0; i < bass; i++){
          strip1.setPixelColor(i, 255, 0, 0); // set to red
        }
      }
      
      // Delay function to prevent flickering the LEDs too quickly
      time = millis();
      if(time - prev_time < 50){
        delay(time - prev_time + 1);
      }
      prev_time = millis();
      strip.show();
    }
    
    
  }
}
