const int buttonPin = 2;                          // Digital pin connected to the button
const int pumpPin = 7;                            // Digital pin connected to the MOSFET gate
const int pumpRunTime = 4000;                     // Pump run time in milliseconds (4 seconds)
const int ledPin = LED_BUILTIN;                   // Usually pin 13 on Nano Every
const unsigned long buttonTimeout = 1200000;      // Reset timer before pump runs again - 20 minutes (1,200,000 milliseconds)
const unsigned long distanceReadInterval = 5000;  // Reads the distance every 5 seconds

unsigned long lastButtonPressTime = 0;    // Variable to store the last time the button was pressed
unsigned long lastDistanceReadTime = 0;  // Variable to store the last time the distance was read
bool buttonPressed = false;               // Flag to indicate if the button has been pressed

// HC-SR04 pins & settings (ultrasonic distance sensor)
const int trigPin = 8;
const int echoPin = 9;
const int maxDistance = 15;  // Maximum distance to consider the bowl present (in cm)

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);  // Enable internal pull-up resistor for the button pin
  pinMode(pumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Serial Comms established");
}

void loop() {
  // Read the button state (LOW when pressed because of INPUT_PULLUP)
  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    Serial.println("Button Pressed!");
    delay(500);  // half-second delay to prevent rapid/ held button press
    buttonPressed = true;
    lastButtonPressTime = millis();

    // Check for bowl presence using HC-SR04
    float distance = getDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= maxDistance) {
      Serial.println("Bowl detected!");
      activatePump();
      flashLED(); // Flash the LED to indicate button press and successful activation
    } else {
      Serial.println("Bowl not detected!");
    }
  }

  // Read distance every 5 seconds
  if (millis() - lastDistanceReadTime >= distanceReadInterval) {
    float distance = getDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    lastDistanceReadTime = millis(); // Update the last distance read time
  }

  checkButtonTimeout(); // Check if the button timeout has elapsed
}

void checkButtonTimeout() {
  if (buttonPressed && (millis() - lastButtonPressTime >= buttonTimeout)) {
    buttonPressed = false;
    Serial.println("Button timeout reset");
  }
}

void activatePump() {
  Serial.println("Pump Activated!");
  digitalWrite(pumpPin, HIGH);  // Turn the pump ON
  delay(pumpRunTime);           // Wait for the specified time
  digitalWrite(pumpPin, LOW);   // Turn the pump OFF
  Serial.println("Pump Deactivated");
  delay(100);  // Add a small delay for stability
}

void flashLED() {
  digitalWrite(ledPin, HIGH);  // Turn the LED on
  delay(250);
  digitalWrite(ledPin, LOW);
  delay(250);
}

float getDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  float distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)

  return distance;
}
