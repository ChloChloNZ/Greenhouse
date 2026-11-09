#include <Servo.h>

const int BUTTON_PIN       = 3;    
const int BUZER_PIN        = 8;    
const int TEMP_PIN         = A0;   
const int SERVO_PIN        = 9;    
const int LED_PIN          = 13;  
const float TARGET_TEMP_NORMAL = 25.0; 
const float CRITICAL_TEMP_HIGH = 40.0; 

Servo ventServo;
int ventAngle = 0;
bool systemOverrideActive = false;
int lastButtonState = HIGH; 

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(BUZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);        
  
  ventServo.attach(SERVO_PIN);
  ventServo.write(0); 
  
  Serial.println("GREENHOUSE READY: Starting automated loop...");
}

void loop() {
  int currentButtonReading = digitalRead(BUTTON_PIN);

  if (currentButtonReading == LOW && lastButtonState == HIGH) {
    systemOverrideActive = !systemOverrideActive; 
    
    if (systemOverrideActive) {
      Serial.println("ALERT: System Override Activated by Operator.");
    } else {
      Serial.println("SYSTEM: Resuming Automated Processing Loops.");
    }
    delay(50); 
  }
  lastButtonState = currentButtonReading; 

  int rawAnalogInput = analogRead(TEMP_PIN);
  float voltageOutput = rawAnalogInput * (5.0 / 1023.0);
  float currentTemperatureC = (voltageOutput - 0.5) * 100.0; 

  Serial.print("Logs -> Temp: ");
  Serial.print(currentTemperatureC);
  Serial.print("C | Override status: ");
  Serial.println(systemOverrideActive ? "ACTIVE" : "INACTIVE");

  if (systemOverrideActive) {
    digitalWrite(LED_PIN, LOW);    
    ventAngle = 90;
    ventServo.write(ventAngle);
    noTone(BUZER_PIN);
  } else {
    
    if (currentTemperatureC >= CRITICAL_TEMP_HIGH) {
      digitalWrite(LED_PIN, HIGH); // TURN WARNING LIGHT ON
      ventAngle = 180;
      ventServo.write(ventAngle);
      tone(BUZER_PIN, 1000);
    } 
    else if (currentTemperatureC > TARGET_TEMP_NORMAL && currentTemperatureC < CRITICAL_TEMP_HIGH) {
      digitalWrite(LED_PIN, LOW);  
      noTone(BUZER_PIN);
      float calculationFactor = (currentTemperatureC - TARGET_TEMP_NORMAL) / (CRITICAL_TEMP_HIGH - TARGET_TEMP_NORMAL);
      ventAngle = (int)(calculationFactor * 180.0);
      ventAngle = constrain(ventAngle, 0, 180); 
      ventServo.write(ventAngle);
    } 
    else {
      digitalWrite(LED_PIN, LOW);  
      ventAngle = 0;
      ventServo.write(ventAngle);
      noTone(BUZER_PIN);
    }
  }

  delay(30); 
}
