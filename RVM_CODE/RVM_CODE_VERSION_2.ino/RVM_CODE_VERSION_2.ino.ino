#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor pins
const int irSensor = 2;  // Changed from capacitive proximity sensor to IR sensor
const int inductiveSensor = 3;
const int photoelectricSensor = 4;
const int buzzer = 8; // Buzzer pin

// Servo motors
Servo gateServo;       // Servo for gate
Servo sorterServo;     // Servo for sorting
Servo coinDispenser;   // Servo for coin dispenser

// Counters for tracking the number of bottles
int metalCount = 0;
int plasticCount = 0;

void setup() {
  // Initialize sensor pins
  pinMode(irSensor, INPUT);
  pinMode(inductiveSensor, INPUT);
  pinMode(photoelectricSensor, INPUT);
  pinMode(buzzer, OUTPUT);
  
  // Attach servos to pins
  gateServo.attach(6);       // Gate servo on pin 6
  sorterServo.attach(9);     // Sorting servo on pin 9
  coinDispenser.attach(10);  // Coin dispenser servo on pin 10

  // Start serial communication (for debugging)
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Metal: 0  Plastic: 0");

  // Set initial servo positions
  gateServo.write(0);      // Gate closed
  sorterServo.write(90);   // Neutral sorting position
  coinDispenser.write(0);  // Coin dispenser in initial position
}

void loop() {
  if (digitalRead(irSensor) == LOW) {  // Check if an object is detected by IR sensor
    delay(500);  // Debounce delay for stable reading

    // Check if the IR sensor is still triggered
    if (digitalRead(irSensor) == LOW) {
      Serial.println("Object detected and confirmed!");

      // Close the gate to hold the object for sensing
      gateServo.write(0);
      delay(1000);

      // Perform sensing
      bool isMetal = !digitalRead(inductiveSensor);
      bool isPlastic = digitalRead(photoelectricSensor);

      // Indicate sensing is complete with a buzzer sound
      tone(buzzer, 1000, 500);  // Sound buzzer at 1000 Hz for 500 ms
      delay(500);

      // Open the gate to release the object into sorting
      gateServo.write(90);
      delay(1000);

      // Sorting logic
      if (isMetal) {
        Serial.println("Metal detected. Sorting to Metal Bin...");
        sorterServo.write(0);  // Move servo to metal bin position
        metalCount++;  // Increment metal count
      } 
      else if (isPlastic) {
        Serial.println("Plastic detected. Sorting to Plastic Bin...");
        sorterServo.write(180);  // Move servo to plastic bin position
        plasticCount++;  // Increment plastic count
      } 
      else {
        Serial.println("Unknown object. Sorting to General Waste Bin...");
        sorterServo.write(90);  // Move servo to general bin position
      }

      delay(2000);  // Wait for sorting process to complete
      sorterServo.write(90);  // Reset sorter to neutral position

      // Update LCD with new counts
      updateLCD();

      // Check if total count is 3 to trigger coin dispenser
      if ((metalCount + plasticCount) >= 3) {
        triggerCoinDispenser();
        resetCounts();
        updateLCD();
      }
    } else {
      Serial.println("Object detection failed. No sorting triggered.");
    }
  }
  
  delay(100);  // Short delay before the next loop
}

// Function to update the LCD display with current counts
void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Metal: ");
  lcd.print(metalCount);
  lcd.print("  ");
  
  lcd.setCursor(0, 1);
  lcd.print("Plastic: ");
  lcd.print(plasticCount);
  lcd.print("  ");
}

// Function to trigger the coin dispenser
void triggerCoinDispenser() {
  Serial.println("3 bottles sorted! Dispensing coin...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dispensing Coin...");

  coinDispenser.write(90);  // Turn the servo to release the coin
  delay(2000);              // Wait for 2 seconds
  coinDispenser.write(0);   // Return servo to initial position
  delay(1000);
}

// Function to reset the bottle counts
void resetCounts() {
  metalCount = 0;
  plasticCount = 0;
  Serial.println("Counts reset.");
}
