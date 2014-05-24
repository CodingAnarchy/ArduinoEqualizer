#define OCTAVE 1  // use the octave output function
#define FHT_N 128 // set to 128 point FHT

#include <FHT.h>
#include <Adafruit_NeoPixel.h>
#include "led.h"
#include "fht.h"

// Setup system
void setup(){
  led_setup();
  fht_setup();

}

// Main operating loop
void loop(){
  while(1) {
    // TODO: Check for current system settings status - various switches and options
    boolean frequency = true;
    
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
      
      fht_window(); // window the data for better frequency response
      fht_reorder(); // reorder the data before doing the fht
      fht_run(); // process the data in the fht
      fht_mag_octave(); // take the output of the fht
      
      sei();
      TIMSK0 = 1; // turn timer0 back on for timing measurements - may be needed
    }
    
    else {
      // Read in volume output and key off of that
    }
    
    led_loop();
  }
}
