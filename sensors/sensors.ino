#ifdef ESP32
//https://github.com/ERROPiX/ESP32_AnalogWrite
#include <analogWrite.h>
#endif 

//piezo max range
#define MAX 4500
//pin where piezo output is connected
#define PIN_PIEZO A0
#define PIN_BUZZER 12
//num of samples before running offline algorithms
#define SAMPLE_TIME 10
#define VECTOR_SIZE 50
#define ALARM_TRIGGER 300
#define ALARM_OFFSET 50

size_t value = 0;
size_t loopNum = 0;
size_t alarmLoopNum = 0;
size_t vector[VECTOR_SIZE];
size_t alarmVector[ALARM_TRIGGER];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Start");

  #ifdef ESP32
  analogWriteFrequency(PIN_BUZZER, 10000);
  #endif
  
  pinMode(PIN_PIEZO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  analogWrite(PIN_BUZZER, 100);
  delay(2000);
  analogWrite(PIN_BUZZER, 0);
}

void checkAlarm() {
  float sum = 0;
  for(size_t i = 0; i < ALARM_TRIGGER; i++) {
    sum = sum + alarmVector[i];
  }
  float mean = sum / ALARM_TRIGGER;
  size_t sumDistance[ALARM_TRIGGER];
  size_t finalSum = 0;
  for(size_t i = 0; i < ALARM_TRIGGER; i++) {
    sumDistance[i] = (alarmVector[i] - mean) * (alarmVector[i] - mean);
    finalSum += sumDistance[i];
  }

  float finalMean = finalSum / ALARM_TRIGGER;
  
  if(sqrt(finalMean) < ALARM_OFFSET) {
      analogWrite(PIN_BUZZER, 100);
  } else {
      analogWrite(PIN_BUZZER, 0);
  }
}

void loop() {
  value = analogRead(PIN_PIEZO);
  vector[loopNum] = value;
  
  delay(SAMPLE_TIME);
  loopNum++;
  
  if(loopNum == VECTOR_SIZE) {
    loopNum = 0;
  }

  float sum = 0;
  for(size_t i = 0; i < VECTOR_SIZE; i++) {
    sum = sum + vector[i];
  }
  float mean = sum / VECTOR_SIZE;
  
  alarmVector[alarmLoopNum] = mean;
  alarmLoopNum++;
  if(alarmLoopNum == ALARM_TRIGGER) {
    alarmLoopNum = 0;
    checkAlarm();
  }
  Serial.print(mean);
}
