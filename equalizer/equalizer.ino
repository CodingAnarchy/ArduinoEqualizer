#define DEBUG true

#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
const byte pins[] = {
  6, 7, 8, 9};
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(60, pins[0], NEO_GRB + NEO_KHZ800);

// eq stuff
const int strobe = A1; // strobe pins on digital 4
const int res = A2; // reset pins on digital 5
int band;
int EQ_OUT1 = A0;

int eqLevels[7]; // store band values in these arrays

void led_setup(){
  strip1.begin();
  strip1.show(); // show all pixels as 'off' to start
  led_loop();
}

void initMSGEQ7()
{
  digitalWrite(res, LOW); // reset low
  digitalWrite(strobe, HIGH); //pin 5 is RESET on the shield
}

void readMSGEQ7()
// Function to read 7 band equalizers
{
  digitalWrite(res, HIGH);
  digitalWrite(res, LOW);
  for(band=0; band <7; band++)
  {
    digitalWrite(strobe, LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30); // 
    eqLevels[band] = analogRead(EQ_OUT1); // store band reading
    digitalWrite(strobe, HIGH); 
  }
#ifdef DEBUG
  dumpEQ();
#endif
}

#ifdef DEBUG
void dumpEQ(){

  // display values of right channel on serial monitor
  for (band = 0; band < 7; band++)
  {
    Serial.print(eqLevels[band]);
    Serial.print(" ");
  }
  Serial.println();

}
#endif

// Setup system
void setup(){
#ifdef DEBUG
  Serial.begin(38400);
  Serial.println(F("Setting up."));
#endif
  pinMode(res, OUTPUT); //reset
  pinMode(strobe, OUTPUT); //strobe
  led_setup();
  initMSGEQ7();
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
  unsigned long time = 0;
  unsigned long prev_time = 0;

  while(1) {
    // TODO: Check for current system settings status - various switches and options

    // Operate FHT ADC to obtain current frequency levels - if selected
    if (frequency) {

      initMSGEQ7();
      readMSGEQ7();
      // for (int i = 0; i < 7; i = i + 2){
      int level = map(eqLevels[2], 0, 1024, 0, strip1.numPixels());
#ifdef DEBUG
      Serial.print("Level = ");
      Serial.println(level);
      for(int i = 0; i < level; i++){
        strip1.setPixelColor(i, 0, 255, 255); // set to cyan
      }
      for(int i = level; i < strip1.numPixels(); i++){
        strip1.setPixelColor(i, 0, 0, 0);
      }

      // Delay function to prevent flickering the LEDs too quickly
      /*time = millis();
      if(time - prev_time < 50){
        delay(time - prev_time + 1);
      }
      prev_time = millis();*/
      delay(50);
      strip1.show();
#endif
      //}
    }
    else {
      // Read in volume output and key off of that
    }

#ifndef DEBUG
    led_loop();
#endif
  }
}









