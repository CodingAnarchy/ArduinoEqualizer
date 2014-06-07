#define OCTAVE 1  // use the octave output function
#define FHT_N 256 // set to 256 point FHT

#define DEBUG true

#include <FHT.h>
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
byte pins[] = {
  6, 7, 8, 9};
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(60, pins[0], NEO_GRB + NEO_KHZ800);

void fht_setup(){
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void led_setup(){
  strip1.begin();
  strip1.show(); // show all pixels as 'off' to start
  led_loop();
}

// Setup system
void setup(){
#ifdef DEBUG
  Serial.begin(38400);
  Serial.println(F("Setting up."));
#endif
  fht_setup();
  led_setup();
}

void led_loop() {
  for(byte i = 0; i < strip1.numPixels(); i++) {
    strip1.setPixelColor(i, 0, 255, 255);
    strip1.show();
    delay(50);
  }


  for(byte i = 0; i < strip1.numPixels(); i++) {
    strip1.setPixelColor(i, 255, 0, 255);
    strip1.show();
    delay(50);
  }  

}

// Main operating loop
void loop(){
  // Initialize variables
  boolean frequency = true;
  int data;
  const byte outputs = 4;
  unsigned long time = 0;
  unsigned long prev_time = 0;

  while(1) {
    // TODO: Check for current system settings status - various switches and options

    // Operate FHT ADC to obtain current frequency levels - if selected
    if (frequency) {
      // noInterrupts();  // UDRE interrupt slows this way down on arduino1.0
      // #ifdef DEBUG
      // Serial.println("Interrupts disabled.");
      // delay(50);
      // #endif
      TIMSK0 = 0; // turn off timer0 for lower jitter

      for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
        while(!(ADCSRA & 0x10)); // wait for adc to be ready
        ADCSRA = 0xf5; // restart adc
        // data = analogRead(0);
        byte m = ADCL; // fetch adc data
        byte j = ADCH;
        data = (j << 8) | m; // form into an int
        data -= 0x0200; // form into a signed int
        data <<= 6; // form into a 16b signed int
        fht_input[i] = data; // put real data into bins
      }

      // interrupts();
      TIMSK0 = 1; // turn timer0 back on for timing measurements - may be needed

      fht_window(); // window the data for better frequency response
      fht_reorder(); // reorder the data before doing the fht
      fht_run(); // process the data in the fht
      fht_mag_octave(); // take the output of the fht
#ifdef DEBUG
      Serial.write(255);
      Serial.write(fht_oct_out, 8);
      Serial.print("\n");
#endif

      if (outputs == 4){
        int bass = (fht_oct_out[0] + fht_oct_out[1]) / 2;
        int low_mid = (fht_oct_out[2] + fht_oct_out[3]) / 2;
        int high_mid = (fht_oct_out[4] + fht_oct_out[5]) / 2;
        int treble = (fht_oct_out[6] + fht_oct_out[7]) / 2;

        // Normalize groupings to obtain outputs
        bass = (int)(((float)bass / 256.0) * 60.0);
        low_mid = (int)(((float)low_mid / 256.0) * 60.0);
        high_mid = (int)(((float)high_mid / 256.0) * 60.0);
        treble = (int)(((float)treble / 256.0) * 60.0);

#ifdef DEBUG
        char msg[] = "Bass = ";
        Serial.println(msg + bass);
        char msg2[] = "Low Mid = ";
        Serial.println(msg2 + low_mid);
        char msg3[] = "High Mid = ";
        Serial.println(msg3 + high_mid);
        char msg4[] = "Treble = ";
        Serial.println(msg4 + treble);
#endif

#ifdef DEBUG
        for(int i = 0; i < high_mid; i++){
          strip1.setPixelColor(i, 0, 0, 255); // set to blue
        }
        for(int i = high_mid; i < strip1.numPixels(); i++){
          strip1.setPixelColor(i, 0, 0, 0);
        }

        // Delay function to prevent flickering the LEDs too quickly
        time = millis();
        if(time - prev_time < 50){
          delay(time - prev_time + 1);
        }
        prev_time = millis();
        strip1.show();
#endif

      }
      else {
        // Read in volume output and key off of that
      }

#ifndef DEBUG
      led_loop();
#endif
    }
  }
}






