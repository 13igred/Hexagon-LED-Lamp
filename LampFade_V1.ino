#include <Adafruit_NeoPixel.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(33, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel. Avoid connecting
// on a live circuit... if you must, connect GND first.

const int INTERRUPT_PIN = 2;
volatile int progStep = 0;
volatile unsigned long last_micros;
long debouncing_time = 15; //Debouncing Time in Milliseconds
int delayTime = 20;

struct rgb{
  int r;
  int g;
  int b;
};

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // initialize all pixels to "off"
  strip.setBrightness(100);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), debounceInterrupt, LOW);      // trigger when button pressed, but not when released.
  progStep = LOW;
}

void loop() {

  if (progStep > 5)
  {
    progStep = 1;    
  }

  // fade in fade out pattern
  if (progStep == 1)
  {
    struct rgb rgb_start;
    struct rgb rgb_current;
    
    int a = random(30,100);
    int b = random(0,30);
    int c = random(0,10);
    int choice = random(0,7);
    
    if (choice == 1) {
      rgb_start.r = a;
      rgb_start.g = b; 
      rgb_start.b = c;
    }
    if (choice == 2) {
      rgb_start.r = a;
      rgb_start.g = c; 
      rgb_start.b = b;
    }
    if (choice == 3) {
      rgb_start.r = b;
      rgb_start.g = c; 
      rgb_start.b = a;
    }
    if (choice == 4) {
      rgb_start.r = b;
      rgb_start.g = a; 
      rgb_start.b = c;
    }
    if (choice == 5) {
      rgb_start.r = c;
      rgb_start.g = b; 
      rgb_start.b = a;
    }
    if (choice == 6) {
      rgb_start.r = c;
      rgb_start.g = a; 
      rgb_start.b = b;
    }
    
    rgb_current = brighten(rgb_start.r, rgb_start.g, rgb_start.b);
  
    darken(rgb_start.r, rgb_start.g, rgb_start.b, rgb_current.r, rgb_current.g, rgb_current.b);
  }

  //Spin pattern
  if (progStep == 2)
  {
    int r = random(0,255);
    int g = random(0,255);
    int b = random(0,255);

    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, r, g, b);
      strip.show();
      delay(20);
    }
  }

  // sprinkle
  if (progStep == 3)
  {
    int r = random(0,255);
    int g = random(0,255);
    int b = random(0,255);

    for (int i = 0; i < 5; i++) {
      int a = random(0, strip.numPixels());
      strip.setPixelColor(a, r, g, b);
      strip.show();
      delay(100);
    }
  }

  // tail chase
  if (progStep == 4)
  {
    int r = random(0,255);
    int g = random(0,255);
    int b = random(0,255);
    int tailLen = random(2,12);
    int index = 0;
    
    for(int j = 0; j < strip.numPixels(); j++)
    {
      for (int i = index; i < index + tailLen; i++)
      {
        strip.setPixelColor(i, r, g, b);      
      }
      strip.show();
      delay(35);
      for (int k = 0; k < strip.numPixels(); k++)
      {
        strip.setPixelColor(k, 0, 0, 0);
      }
      index++;
    }
  }

  if (progStep == 5)
  {
    strip.clear();
    strip.show();
  }
}

struct rgb brighten(int r, int g, int b) {
  //Serial.println("brighten");
  
  struct rgb rgb_instance;  
  int temp = max(r,g);
  int maxC = max(b,temp);
  int r0 = r;
  int g0 = g;
  int b0 = b;
   
   for (int j = maxC; j < 175; j++) {
    
    //if statment enables richer colours
    if(r0 == maxC)
    {
      r = r + 2;      
    }
    else if (r0 > 5)
    {
      r++;
    }
    if(g0 == maxC)
    {
      g = g + 2;      
    }
    else if (g0 > 5)
    {
      g++;
    }
    if(b0 == maxC)
    {
      b = b + 2;      
    }
    else if (b0 > 5)
    {
      b++;
    }
    
    // Limit values at 254
    if(r > 255)
    {
      r = 254;
    }
    if(g > 255)
    {
      g = 254;
    }
    if(b > 255)
    {
      b = 254;
    }
    
    //Print values to strip
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, r, g, b);
    }
        
    strip.show();
    delay(delayTime);
  }
  
  rgb_instance.r = r;
  rgb_instance.g = g;
  rgb_instance.b = b;
  return rgb_instance;
}

void darken(int startR, int startG, int startB, int curR, int curG, int curB) {
  //Serial.println("darken");
  
  int temp = max(startR, startG);
  int minStart = max(temp, startB);
    
  temp = max(curR,curG);
  int minCur = max(curB,temp);

  int r0 = curR;
  int g0 = curG;
  int b0 = curB;
  
  for (int j = minCur; j > minStart; j--) {
    
    if(r0 == minCur)
    {
      curR = curR - 2;      
    }
    else if (r0 > 5)
    {
      curR--;
    }
    if(g0 == minCur)
    {
      curG = curG - 2;      
    }
    else if (g0 > 5)
    {
      curG--;
    }
    if(b0 == minCur)
    {
      curB = curB - 2;      
    }
    else if (b0 > 5)
    {
      curB--;
    }

    if(curR < 0)
    {
      curR = 0;
    }
    if(curG < 0)
    {
      curG = 0;
    }
    if(curB < 0)
    {
      curB = 0;
    }
    
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, curR, curG, curB);
    }  
    strip.show();
    delay(delayTime);
  }
}

void debounceInterrupt() {
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    button_ISR();
    last_micros = micros();
  }
}

void button_ISR()
{
  progStep++;
}
