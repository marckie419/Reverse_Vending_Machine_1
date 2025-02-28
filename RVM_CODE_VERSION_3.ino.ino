#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor pins
const int irSensor = 2;  
const int inductiveSensor = 3;
const int photoelectricSensor = 4;
const int buzzer = 5; 

// Servo motors
Servo gateServo;       
Servo sorterServo;     
Servo coinDispenser;   

// Ultrasonic Sensor Pins
const int trigPlastic = 9;   // Trigger pin for Plastic Bin
const int echoPlastic = 10;  // Echo pin for Plastic Bin
const int trigMetal = 11;    // Trigger pin for Metal Bin
const int echoMetal = 12;    // Echo pin for Metal Bin

// Counters for tracking the number of bottles
int metalCount = 0;
int plasticCount = 0;

// Bin full distance threshold (Now set to 10 cm)
const int binFullThreshold = 10;  

void setup() {
  // Initialize sensor pins
  pinMode(irSensor, INPUT);
  pinMode(inductiveSensor, INPUT);
  pinMode(photoelectricSensor, INPUT);
  pinMode(buzzer, OUTPUT);
  
  // Initialize ultrasonic sensors
  pinMode(trigPlastic, OUTPUT);
  pinMode(echoPlastic, INPUT);
  pinMode(trigMetal, OUTPUT);
  pinMode(echoMetal, INPUT);

  // Attach servos to pins
  gateServo.attach(6);       
  sorterServo.attach(7);     
  coinDispenser.attach(8);  

  // Start serial communication (for debugging)
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Metal: 0  Plastic: 0");

  // Set initial servo positions
  gateServo.write(0);      
  sorterServo.write(90);   
  coinDispenser.write(0);  
}

void loop() {
  if (digitalRead(irSensor) == LOW) {  
    delay(500);  

    if (digitalRead(irSensor) == LOW) {
      Serial.println("Object detected and confirmed!");

      // Close the gate to hold the object for sensing
      gateServo.write(0);
      delay(2000);

      // Perform sensing
      bool isMetal = !digitalRead(inductiveSensor);
      bool isPlastic = digitalRead(photoelectricSensor);

      // Indicate sensing is complete with a buzzer sound
      tone(buzzer, 1000, 500);  
      delay(500);

      // Open the gate to release the object into sorting
      gateServo.write(90);
      delay(1000);

      // Get bin distances
      int distancePlastic = getDistance(trigPlastic, echoPlastic);
      int distanceMetal = getDistance(trigMetal, echoMetal);

      // Sorting logic with bin full check
      if (isMetal && !isBinFull(distanceMetal)) {
        Serial.println("Metal detected. Sorting to Metal Bin...");
        sorterServo.write(0);  
        metalCount++;  
      } 
      else if (isPlastic && !isBinFull(distancePlastic)) {
        Serial.println("Plastic detected. Sorting to Plastic Bin...");
        sorterServo.write(180);  
        plasticCount++;  
      } 
      else {
        Serial.println("Unknown object or bin full. Sorting to General Waste Bin...");
        sorterServo.write(90);  
      }

      delay(2000);  
      sorterServo.write(90);  

      // Update LCD
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

  // Print bin distances to Serial Monitor
  Serial.print("Plastic Bin Distance: ");
  Serial.print(getDistance(trigPlastic, echoPlastic));
  Serial.println(" cm");

  Serial.print("Metal Bin Distance: ");
  Serial.print(getDistance(trigMetal, echoMetal));
  Serial.println(" cm");

  delay(1000);  
}

// Function to get distance from ultrasonic sensor
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;  // Convert to cm

  return distance;
}

// Function to check if a bin is full
bool isBinFull(int distance) {
  return distance <= binFullThreshold;  // Returns true if distance is ≤ 10 cm
}

// Function to update the LCD display with current counts and distances
void updateLCD() {
  int distancePlastic = getDistance(trigPlastic, echoPlastic);
  int distanceMetal = getDistance(trigMetal, echoMetal);

  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.print(metalCount);
  lcd.print(" D:");
  lcd.print(distanceMetal);
  lcd.print("cm");
  if (isBinFull(distanceMetal)) lcd.print(" F");

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(plasticCount);
  lcd.print(" D:");
  lcd.print(distancePlastic);
  lcd.print("cm");
  if (isBinFull(distancePlastic)) lcd.print(" F");
}

// Function to trigger the coin dispenser
void triggerCoinDispenser() {
  Serial.println("3 bottles sorted! Dispensing coin...");

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("DISPENSING");
  lcd.setCursor(5, 1);
  lcd.print("COIN...");

  coinDispenser.write(90);  
  delay(2500);
  
  coinDispenser.write(0);   
  delay(1000);

  lcd.clear();
  updateLCD();  
}

// Function to reset the bottle counts
void resetCounts() {
  metalCount = 0;
  plasticCount = 0;
  Serial.println("Counts reset.");
}
