#include <Wire.h>

#include <LiquidCrystal_I2C.h>



// LCD Setup (change I2C address if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
const int buttonPins[] = {2, 3, 4, 5};   // Buttons
const int ledPins[] = {11, 10, 9, 8};    // LEDs (Correct Order)
const int buzzerPin = 12;

const int maxLevel = 10;
int sequence[maxLevel];
int userInput[maxLevel];

const int ledOnTime = 500;     // LED ON for 500ms (visible)
const int ledOffTime = 300;    // Delay between blinks
const int inputDebounce = 200; // Button debounce delay

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);  // Using external 10k pull-down resistors
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  randomSeed(analogRead(A0));  // Random seed

  // LCD Initialization
  lcd.init();
  lcd.backlight();
  welcomeScreen();
}

void loop() {
  for (int level = 1; level <= maxLevel; level++) {
    showLevel(level);
    generateSequence(level);
    if (!playAndCheck(level)) {
      gameOver();
      return;  // Restart the game
    }
    delay(1000);  // Pause before next level
  }
  gameWin();
}

void welcomeScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome to  ");
  lcd.setCursor(0, 1);
  lcd.print(" Simon Says!  ");
  delay(2000);
  lcd.clear();
}

void showLevel(int level) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Level: ");
  lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print(" Repeat Pattern ");
  delay(1500);
}

void generateSequence(int level) {
  for (int i = 0; i < level; i++) {
    sequence[i] = random(0, 4);
  }
}

bool playAndCheck(int level) {
  // Show Sequence with proper delays
  for (int i = 0; i < level; i++) {
    blinkLED(sequence[i]);
    delay(ledOffTime);  // Delay between blinks
  }

  // User Input
  for (int i = 0; i < level; i++) {
    int input = waitForButtonPress();
    userInput[i] = input;
    if (userInput[i] != sequence[i]) {
      return false;  // Incorrect input
    }
  }
  return true;  // Correct sequence
}

int waitForButtonPress() {
  while (true) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == HIGH) {  // Button pressed
        blinkLED(i);  // Feedback on correct button press
        while (digitalRead(buttonPins[i]) == HIGH);  // Wait for release
        delay(inputDebounce);  // Debounce
        return i;
      }
    }
  }
}

void blinkLED(int index) {
  digitalWrite(ledPins[index], HIGH);
  tone(buzzerPin, 440 + index * 100, ledOnTime);  // Sound Feedback
  delay(ledOnTime);  // LED stays ON long enough to be seen
  digitalWrite(ledPins[index], LOW);
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   You Lose!   ");
  lcd.setCursor(0, 1);
  lcd.print("Try Again :)");

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], HIGH);
    }
    tone(buzzerPin, 200, 300);
    delay(300);
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], LOW);
    }
    delay(300);
  }
  delay(2000);
  lcd.clear();
}

void gameWin() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  You Win! :)  ");
  lcd.setCursor(0, 1);
  lcd.print("Congratulations!");

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], HIGH);
    }
    tone(buzzerPin, 1000, 300);
    delay(300);
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], LOW);
    }
    delay(300);
  }
  delay(3000);
  lcd.clear();
}
