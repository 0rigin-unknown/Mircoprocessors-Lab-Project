// Include necessary libraries for I2C communication and LCD display

#include <LiquidCrystal_I2C.h>

// Define pin assignments
#define MOTOR_PIN 9          // PWM pin controlling the motor (connected to TIP120)
#define IR_SENSOR_PIN 2      // Digital pin for IR sensor input
#define POT_PIN A0           // Analog pin for potentiometer
#define LED_PIN 12           // Digital pin for LED indicator

int seconds = 0;

// Initialize LCD with I2C address 0x20, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(0x20,16,2);

int motorSpeed = 0;         // Variable to store motor speed value
unsigned long prevBlinkTime = 0; // Stores the previous time the LED was toggled
bool ledState = false;      // Stores the current state of the LED
bool speedWarningDisplayed = false; // Flag to track if speed warning is displayed

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);     // Set motor pin as output
  pinMode(IR_SENSOR_PIN, INPUT_PULLUP);  // Set IR sensor pin as input with internal pull-up resistor
  pinMode(LED_PIN, OUTPUT);       // Set LED pin as output
  Serial.begin(9600);             // Start serial communication at 9600 baud
  
  lcd.init();   // Initialize the LCD
  lcd.clear();  // Clear any previous display
  lcd.backlight(); // Turn on LCD backlight
}

void loop() {
  // Read potentiometer value and map it to PWM range (0-255)
  motorSpeed = map(analogRead(POT_PIN), 0, 1023, 0, 255);
  analogWrite(MOTOR_PIN, motorSpeed);  // Set motor speed using PWM
  
  // LED Blinking without delay when speed exceeds 195
  if (motorSpeed >= 195) {
    if (!speedWarningDisplayed) {
      lcd.clear(); // Clear the LCD before displaying speed warning
      speedWarningDisplayed = true;
    }
    
    if (millis() - prevBlinkTime >= 500) { // Check if 500ms has passed
      ledState = !ledState; // Toggle LED state
      digitalWrite(LED_PIN, ledState); // Update LED state
      prevBlinkTime = millis(); // Reset timer
      
      // Display warning message on LCD
      lcd.setCursor(2,0); 
      lcd.print("Speed Limit");
      lcd.setCursor(2,1); 
      lcd.print("Slow down");
    }
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off LED if speed is below 195
    speedWarningDisplayed = false; // Reset flag
  }

  // IR Sensor Check - Detect object presence
  if (digitalRead(IR_SENSOR_PIN) == LOW) {
    Serial.print("Object Detected - Speed: ");
    Serial.println(motorSpeed);
    
    if (!speedWarningDisplayed) { // Only show speed if no speed warning is displayed
      lcd.clear();
      lcd.setCursor(2,0); 
      lcd.print("Speed:");
      lcd.setCursor(2,1); 
      lcd.print(motorSpeed);
    }
  } else {
    Serial.println("No Object Detected");
  }

  delay(100); // Short delay for stability
}
