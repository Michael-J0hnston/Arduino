#include <Servo.h>
#include <LedControl.h> // Library for MAX7219 DotMatrix-display

// Define the pins for the encoder
const int encoderPinA = 2;
const int encoderPinB = 3;
const int encoderButton = 4;

// Define PWM parameters
const int minPWM = 1000;
const int maxPWM = 1250;
const int ledCount = 64; // Number of LEDs on the LED matrix
const float pwmIncrement = float(maxPWM - minPWM) / ledCount; // Calculate the steps per LED

Servo motor; // Create a servo object to control the motor
volatile int pwmValue = minPWM; // Variable to hold the PWM value

// Define the pins for the MAX7219 DotMatrix-display
const int DATA_IN_PIN = 5;   // MOSI
const int CLK_PIN = 6;       // SCK
const int LOAD_PIN = 7;      // SS

LedControl lc = LedControl(DATA_IN_PIN, CLK_PIN, LOAD_PIN, 1); // 1 display

void setup() {
  Serial.begin(9600); // Start serial communication

  pinMode(encoderPinA, INPUT_PULLUP); // Configure pin A of the encoder as input with pull-up resistor
  pinMode(encoderPinB, INPUT_PULLUP); // Configure pin B of the encoder as input with pull-up resistor
  pinMode(encoderButton, INPUT_PULLUP); // Configure the encoder button as input with pull-up resistor

  motor.attach(9); // Attach the servo motor to pin 9
  motor.writeMicroseconds(pwmValue); // Initialize the motor position with the minimum PWM value

  // Attach interrupts to the pins for handling rotary encoder movements
  attachInterrupt(digitalPinToInterrupt(encoderPinA), handleEncoder, CHANGE);

  // Initialize the MAX7219 DotMatrix-display
  lc.shutdown(0, false); // Don't shut down the display
  lc.setIntensity(0, 8); // Set the brightness (0 is the display number, 0-15 is the brightness)
  lc.clearDisplay(0); // Clear the display
}

void loop() {
  // Limit the PWM value between the minimum and maximum values
  pwmValue = constrain(pwmValue, minPWM, maxPWM);
  
  // Send the PWM value to the motor
  motor.writeMicroseconds(pwmValue);

  // Display the current PWM value via serial monitor
  Serial.print("PWM value: ");
  Serial.println(pwmValue);

  // Display the PWM value on the MAX7219 DotMatrix-display
  displayPWMValue(pwmValue);

  // Check if the encoder button is pressed
  if (digitalRead(encoderButton) == LOW) {
    pwmValue = minPWM; // Reset the PWM value to 1000
  }

  delay(100); // Add a small delay to prevent debounce
}

// Function to display the PWM value on the MAX7219 DotMatrix-display
void displayPWMValue(int value) {
  int ledsToLight = map(value, minPWM, maxPWM, 0, ledCount); // Map the PWM value to the number of LEDs to light up

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int ledIndex = row * 8 + col;
      if (ledIndex < ledsToLight) {
        lc.setLed(0, row, col, true); // Turn on LEDs based on the number of LEDs to light up
      } else {
        lc.setLed(0, row, col, false); // Turn off LEDs based on the number of LEDs to light up
      }
    }
  }
}

// Interrupt function for handling the rotary encoder movement
void handleEncoder() {
  // Check if the other pin is in the same state as the pin where the interrupt was triggered
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    pwmValue += pwmIncrement; // Increase the PWM value when turning clockwise
  } else {
    pwmValue -= pwmIncrement; // Decrease the PWM value when turning counterclockwise
  }
}
