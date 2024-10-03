#include "arduino_secrets.h"

#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <TaskScheduler.h>

// Nnumber of LEDs in the strip
#define NUM_LEDS 1

// Neopixel data line pin
#define PIN 4

//DC Offset
#define DC_OFFS_RE 2
#define DC_OFFS_IM 2
// Create a NeoPixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//analog pinouts
int realPin = A5;
int complexPin = A4;

// Intensities
float red = 0;
float green = 0;
float blue = 0;

int realVal = 0;
int complexVal = 0;
float real = 0.0;
float complex = 0.0;
float amp = 0.0;
float phase = 0;
float deg = 180 / PI;

// Scheduler
Scheduler runner;

// Task declarations
void Task1();
void Task2();
void Task3();
Task task1(0.1, TASK_FOREVER, &Task1);
Task task2(0.1, TASK_FOREVER, &Task2); 
Task task3(1000, TASK_FOREVER, &Task3);

void setup() {
  Serial.begin(115200);
  // Initialize the NeoPixel
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
  
  // Initialize the scheduler
  runner.init();

  // Add tasks to the scheduler
  runner.addTask(task1);
  runner.addTask(task2);
  runner.addTask(task3);

  // Enable the tasks
  task1.enable();
  task2.enable();
  task3.enable();
}

void loop() {
  // Execute the scheduler
  runner.execute();
}

void Task1() {
  realVal = analogRead(realPin); //10 bit digital value
  complexVal = analogRead(complexPin); //10 bit digital value

  //convert to analog voltage
  real = (realVal/1023.0)*5.0;
  complex = (complexVal/1023.0)*5.0;
}

void Task2() {
    //amplitude calculation
  //account for dc offset
    complex =- DC_OFFS_IM;
    real =- DC_OFFS_RE;
    amp = sqrt((complex * complex) + (real * real));
 //no signal: no colour led is off so r,g,b are all 0
    if (amp < 2.12) {
      red = 0;
      green = 0;
      blue = 0;
    }
      //weak signal is blue so only blue is at full intensity of 255
  else if (amp < 4.5){
    red = 0;
    blue = 255;
    green = 0;
  }
    //strong signal is green so only green is at full intensity
    //of 255
  else {
    red = 0;
    green = 255;
    blue = 0;
  }
  //this is just to reduce the brightness of the led
  //rvb valuws can be between 0,255, higher the value brighter the led glows
  red /= 5;
  green/=5;
  blue/=5;
  

    // Convert to 8-bit color (0-255)
    uint8_t r = (uint8_t)(red);
    uint8_t g = (uint8_t)(green);
    uint8_t b = (uint8_t)(blue);

    // Set the color of the LED
    strip.setPixelColor(0, strip.Color(r, g, b));

    // Show the updated color
    strip.show();
}

//serial monitor
void Task3(){
  Serial.println("Real: ");
  Serial.println(real);
  Serial.println("Complex: ");
  Serial.println(complex);
  Serial.println("Amplitude: ");
  Serial.println(amp);
}
