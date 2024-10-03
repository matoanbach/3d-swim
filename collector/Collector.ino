#include <TaskScheduler.h>
#include "math.h"

#include <Adafruit_NeoPixel.h>

//analog pinouts
# define PI           3.14159265358979323846  /* pi */
#define PIN 4        //Which pin the pixels are connected to
#define LED_COUNT 1  //Number of pixels used

// Create an instance of the Adafruit_NeoPixel class called "leds".
// That'll be what we refer to from here on...
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);


int realPin = A5;
int complexPin = A4;

//rgb pinouts
int redPin = 8;
int greenPin = 9;
int bluePin = 10;

//intensities
int redIntensity = 0;
int greenIntensity = 0;
int blueIntensity = 0;

int realVal = 0;
int complexVal = 0;
int theta = 0;
float realAnalogVoltage = 0.00;
float complexAnalogVoltage = 0.00;



// Scheduler
Scheduler runner;

// Task declarations
void Task1();
void Task2();
void Task3();
double linearMapping(double value, double fromMin, double fromMax, double toMin, double toMax);

float xyToHue(float x, float y);
void hueToRGB(float hue, int &r, int &g, int &b);

Task task1(0.01, TASK_FOREVER, &Task1);
Task task2(0.01, TASK_FOREVER, &Task2);
Task task3(10, TASK_FOREVER, &Task3);

void setup() {
  Serial.begin(115200);

  leds.begin();  // Start up the LED strip
  leds.show();   // LEDs don't actually update until you call this function

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
  realVal = analogRead(realPin);        //10 bit digital value
  complexVal = analogRead(complexPin);  //10 bit digital value
  theta = (atan2((double)realVal, (double)complexVal) * 180) / PI;
}

void Task2() {
  // redIntensity = linearMapping(realVal, 0, 1023, 0, 255);
  // greenIntensity = linearMapping(complexVal, 0, 1023, 0, 255);
  // blueIntensity = linearMapping(theta, -180, 180, 0, 255);

  // // Apply the calculated intensities to the LEDs

  // analogWrite(redPin, redIntensity);
  // analogWrite(bluePin, blueIntensity);
  // analogWrite(greenPin, greenIntensity);

  float hue = xyToHue(realVal, complexVal);
  hueToRGB(hue, redIntensity, greenIntensity, blueIntensity);
  leds.setPixelColor(0, redIntensity, greenIntensity, blueIntensity);  // Set the first pixel to RED
  leds.show();                       //Display the colors
}


// Function to convert x, y to hue
float xyToHue(float x, float y) {
  float theta = atan2(y, x);              // Angle in radians
  float hue = (theta + PI) * 180.0 / PI;  // Map to [0, 360]
  return hue;
}

// Function to convert hue to RGB
void hueToRGB(float hue, int &r, int &g, int &b) {
  float s = 1.0;  // Saturation (full)
  float v = 1.0;  // Value (full)

  int h_i = int(hue / 60.0) % 6;
  float f = hue / 60.0 - h_i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (h_i) {
    case 0:
      r = v * 255;
      g = t * 255;
      b = p * 255;
      break;
    case 1:
      r = q * 255;
      g = v * 255;
      b = p * 255;
      break;
    case 2:
      r = p * 255;
      g = v * 255;
      b = t * 255;
      break;
    case 3:
      r = p * 255;
      g = q * 255;
      b = v * 255;
      break;
    case 4:
      r = t * 255;
      g = p * 255;
      b = v * 255;
      break;
    case 5:
      r = v * 255;
      g = p * 255;
      b = q * 255;
      break;
  }
}


void Task3() {
  Serial.print(realVal);
  Serial.print(",");
  Serial.print(complexVal);
  Serial.println();
}
// to prove this, take a look at: https://docs.google.com/document/d/1q7cv2o1a1rB9mG_Ze95tSL-L3fTHGLfgBbeiYv8s_E8/edit
double linearMapping(double value, double fromMin, double fromMax, double toMin, double toMax) {
  return round((value - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin);
}