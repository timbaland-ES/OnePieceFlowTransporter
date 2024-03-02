
#include <Arduino.h>
#define IR_SENSOR_RIGHT 25
#define IR_SENSOR_LEFT 13

int motorRightPin1 = 26; 
int motorRightPin2 = 27; 
int enableRightMotor = 14;
int motorLeftPin1 = 21; 
int motorLeftPin2 = 22; 
int enableLeftMotor = 23;
int linearActuator1 = 5;
int linearActuator2 = 18;

int loadSensor = 19; //IR (Infrarot)-Sensor, der prüft, ob Transporter beladen ist
int loadStatus = LOW; 
 
// Einstellung der Pulsweitenmodulation
const int freq = 30000;
const int pwmChannel0 = 0;
const int pwmChannel1 = 1;
const int resolution = 8;
 
// Steuert den Antrieb
void runWheels (int signalRightPin1,int signalRightPin2,int signalLeftPin1,int signalLeftPin2){
  digitalWrite(motorRightPin1,signalRightPin1);
  digitalWrite(motorRightPin2,signalRightPin2);
  digitalWrite(motorLeftPin1,signalLeftPin1);
  digitalWrite(motorLeftPin2,signalLeftPin2);
}

void setup() {
  // Ein- Ausgänge setzten
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);
  pinMode(enableRightMotor, OUTPUT);
  pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(loadSensor, INPUT);
  pinMode(linearActuator1, OUTPUT);
  pinMode(linearActuator2, OUTPUT);
  
  // PWM Funktionalitäten definieren
  ledcSetup(pwmChannel0, freq, resolution);
  ledcSetup(pwmChannel1, freq, resolution);

  // PWM-Kanäle den GPIO´s zuordnen
  ledcAttachPin(enableRightMotor, pwmChannel0);
  ledcAttachPin(enableLeftMotor, pwmChannel1);
  ledcWrite(pwmChannel0, 230); 
  ledcWrite(pwmChannel1, 240);  

  Serial.begin(9600);
 
  // Für Tests
  Serial.print("Test setup()");
}
 
void loop() {

  int rightIRSensorValue = digitalRead(IR_SENSOR_RIGHT);
  int leftIRSensorValue = digitalRead(IR_SENSOR_LEFT);

  // IR-Sensor-Werte: 
  // HIGH == schwarz == keine Reflexion,   LOW == hell == Reflexion

  //Wenn kein Sensor die schwarze Linie erkennt, dann fahre geradeaus:
  if (rightIRSensorValue == LOW && leftIRSensorValue == LOW)
  {
    Serial.print("kein schwarz   ");
    runWheels(HIGH,LOW,LOW,HIGH);
  }
  //wenn der rechte Sensor die Linie erkennt, dann fahre nach rechts:
  else if (rightIRSensorValue == HIGH && leftIRSensorValue == LOW )
  {
      Serial.print("schwarz => nach rechts   ");
      runWheels(LOW,HIGH,LOW,HIGH);
  }
  //Wenn der linke Sensor die Linie erkennt, dann fahre nach links: 
  else if (rightIRSensorValue == LOW && leftIRSensorValue == HIGH )
  {
      Serial.print("schwarz => nach links   ");
      runWheels(HIGH,LOW,HIGH,LOW);
  } 
  //Wenn beide Sensoren schwarzen Untergrund erkennen (== else), dann stoppe:
  else 
  { 
    Serial.print("alles schwarz => stopp   ");
    runWheels(LOW,LOW,LOW,LOW);

    if (digitalRead(loadSensor) == LOW ) //Reflexion am Sensor, also Status beladen
      {
        Serial.print("unload   ");
        // Transporter wird entladen:
        digitalWrite(linearActuator1,HIGH);
        digitalWrite(linearActuator2,LOW);
        delay(10000); //vorher 10000
        digitalWrite(linearActuator1,LOW);
        digitalWrite(linearActuator2,HIGH);
        delay(10000);
      }
    
    loadStatus = digitalRead(loadSensor);
    while (loadStatus == HIGH) //Keine Reflexion am Sensor, also auf Beladung warten
      {
        Serial.print("while-Schleife entladen, warten auf Beladung   ");
        delay(250);
        loadStatus = digitalRead(loadSensor);
      }
    //Nach Beladung aus dem schwarzen Bereich fahren
    runWheels(HIGH,LOW,LOW,HIGH);
    delay(500);
    //Solange beide Sensoren schwarz erkennen weiterfahren 
    while (digitalRead(IR_SENSOR_RIGHT) == HIGH && digitalRead(IR_SENSOR_LEFT) == HIGH)
      {
        Serial.print("   while-Schleife noch alles schwarz => weiterfahren ");    
      }
  }
}