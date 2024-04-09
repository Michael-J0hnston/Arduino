#include <Servo.h>
#include <LedControl.h> // Bibliotheek voor MAX7219 DotMatrix-display

// Definieer de pinnen voor de encoder
const int encoderPinA = 2;
const int encoderPinB = 3;
const int encoderButton = 4;

// Definieer PWM-parameters
const int minPWM = 1000;
const int maxPWM = 1250;
const int ledCount = 64; // Aantal LED's op de LED-matrix
const float pwmIncrement = float(maxPWM - minPWM) / ledCount; // Bereken de stappen per LED

Servo motor; // Maak een servo object aan om de motor aan te sturen
volatile int pwmValue = minPWM; // Variabele om de PWM-waarde bij te houden

// Definieer de pinnen voor het MAX7219 DotMatrix-display
const int DATA_IN_PIN = 5;   // MOSI
const int CLK_PIN = 6;       // SCK
const int LOAD_PIN = 7;      // SS

LedControl lc = LedControl(DATA_IN_PIN, CLK_PIN, LOAD_PIN, 1); // 1 display

void setup() {
  Serial.begin(9600); // Start de seriële communicatie

  pinMode(encoderPinA, INPUT_PULLUP); // Configureer pin A van de encoder als input met pull-up weerstand
  pinMode(encoderPinB, INPUT_PULLUP); // Configureer pin B van de encoder als input met pull-up weerstand
  pinMode(encoderButton, INPUT_PULLUP); // Configureer de drukknop van de encoder als input met pull-up weerstand

  motor.attach(9); // Sluit de servo motor aan op pin 9
  motor.writeMicroseconds(pwmValue); // Initialiseer de motorpositie met de minimale PWM-waarde

  // Koppel interrupts aan de pinnen voor het verwerken van draaibewegingen van de encoder
  attachInterrupt(digitalPinToInterrupt(encoderPinA), handleEncoder, CHANGE);

  // Initialiseer het MAX7219 DotMatrix-display
  lc.shutdown(0, false); // Schakel het display niet uit
  lc.setIntensity(0, 8); // Stel de helderheid in (0 is het displaynummer, 0-15 is de helderheid)
  lc.clearDisplay(0); // Wis het display
}

void loop() {
  // Beperk de PWM-waarde tussen de minimale en maximale waarden
  pwmValue = constrain(pwmValue, minPWM, maxPWM);
  
  // Stuur de PWM-waarde naar de motor
  motor.writeMicroseconds(pwmValue);

  // Geef de huidige PWM-waarde weer via de seriële monitor
  Serial.print("PWM-waarde: ");
  Serial.println(pwmValue);

  // Geef de PWM-waarde weer op het MAX7219 DotMatrix-display
  displayPWMValue(pwmValue);

  // Controleer of de drukknop van de encoder is ingedrukt
  if (digitalRead(encoderButton) == LOW) {
    pwmValue = minPWM; // Zet de PWM-waarde terug naar 1000
  }

  delay(100); // Voeg een kleine vertraging toe om debounce te voorkomen
}

// Functie om de PWM-waarde weer te geven op het MAX7219 DotMatrix-display
void displayPWMValue(int value) {
  int ledsToLight = map(value, minPWM, maxPWM, 0, ledCount); // Map de PWM-waarde naar het aantal LED's om in te schakelen

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int ledIndex = row * 8 + col;
      if (ledIndex < ledsToLight) {
        lc.setLed(0, row, col, true); // Zet LED's aan op basis van het aantal in te schakelen LED's
      } else {
        lc.setLed(0, row, col, false); // Zet LED's uit op basis van het aantal in te schakelen LED's
      }
    }
  }
}

// Interrupt-functie voor het verwerken van de draaibeweging van de rotary encoder
void handleEncoder() {
  // Controleer of de andere pin in dezelfde toestand is als de pin waarop de interrupt is geactiveerd
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    pwmValue += pwmIncrement; // Verhoog de PWM-waarde bij het draaien met de klok mee
  } else {
    pwmValue -= pwmIncrement; // Verlaag de PWM-waarde bij het draaien tegen de klok in
  }
}
