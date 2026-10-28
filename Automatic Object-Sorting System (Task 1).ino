#include <Servo.h>

// Define pin for PING))) Sensor 
const int pingPin = 2;

// Define pins for DC Motor Driver
const int motorIn1 = 7;
const int motorIn2 = 8;
const int motorEnable = 10;

Servo sortingServo;

// Threshold for empty belt distance (in cm)
const int baselineDistance = 15; 

// Product counters based on length categories
int shortCount = 0;
int mediumCount = 0;
int longCount = 0;

void setup() {
  // Set motor control pins as outputs
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorEnable, OUTPUT);

  // Attach servo to pin 9
  sortingServo.attach(9);
  sortingServo.write(90); // Center position (Home)

  // Start Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // 1. Run the Conveyor Belt
  startConveyor(200); 

  // 2. Check distance using the sensor
  int distance = getPingDistance();

  // If distance drops below baseline, an object has arrived
  if (distance > 0 && distance < (baselineDistance - 3)) {
    unsigned long startTime = millis();
    
    // Wait until the object passes completely
    while (getPingDistance() < (baselineDistance - 3)) {
      // Keep running conveyor while measuring pass time
      startConveyor(200);
    }
    
    unsigned long passDuration = millis() - startTime; // Time taken to pass

    // 3. Classify Object Based on Length (Pass Duration)
    if (passDuration < 500) {
      Serial.println("Detected: SHORT Object");
      shortCount++;
      sortObject(30); // Sort to Short Bin (Left)
    } 
    else if (passDuration >= 500 && passDuration < 1200) {
      Serial.println("Detected: MEDIUM Object");
      mediumCount++;
      sortObject(90); // Sort to Medium Bin (Center)
    } 
    else {
      Serial.println("Detected: LONG Object");
      longCount++;
      sortObject(150); // Sort to Long Bin (Right)
    }

    // Display updated counts
    printCounts();
  }
  
  delay(100);
}

// Function to measure distance using the sensor
int getPingDistance() {
  long duration;

  // Send a short trigger pulse on the signal pin
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // Switch pin to INPUT to listen for the signal reflection
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // Convert time duration into centimeters
  int cm = duration / 29 / 2; 
  return cm;
}

// Function to control DC motor speed and direction
void startConveyor(int speed) {
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorEnable, speed);
}

// Function to move the servo diverter and reset
void sortObject(int angle) {
  sortingServo.write(angle); // Direct object to bin
  delay(2000);               // Wait for object to slide into bin
  sortingServo.write(90);    // Return to neutral position
  delay(1000);
}

// Function to print total counts to Serial Monitor
void printCounts() {
  Serial.println("--- Length-Based Inventory Count ---");
  Serial.print("Short Bins: "); Serial.println(shortCount);
  Serial.print("Medium Bins: "); Serial.println(mediumCount);
  Serial.print("Long Bins: "); Serial.println(longCount);
}