#include "Arduino.h"
#include <FHT.h>

#define OCTAVE 1  // use the octave output function
#define FHT_N 128 // set to 128 point FHT

void fht_setup() {
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}
