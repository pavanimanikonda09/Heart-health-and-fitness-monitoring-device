#define USE_ARDUINO_INTERRUPTS true
#include "Arduino.h"
#include "classifier.h"
#include <PulseSensorPlayground.h>
#include <SoftwareSerial.h>

const byte rxPin = 5;
const byte txPin = 3;


const int PULSE_SENSOR_PIN = 0; 
const int LED_PIN = 13;        
const int THRESHOLD = 550; 

int user_arr[3]={1,21,80};

int tot_cb = 0;
int p = 0;
int bpm_sum=0;

PulseSensorPlayground pulseSensor;
Eloquent::ML::Port::RandomForest clf;
SoftwareSerial BTSerial(rxPin, txPin); // RX TX

float get_cal_burnt(float avg_bpm)
{
    float cb;
    // T × (0.4472×H - 0.1263×W + 0.074×A - 20.4022) / 4.184 F
    if(user_arr[0]){
        cb=(0.4472*avg_bpm - 0.1263*user_arr[2] + 0.074*user_arr[1] - 20.4022) / 4.184;
    }
    // T × (0.6309×H + 0.1988×W + 0.2017×A - 55.0969) / 4.184 M
    else{
        cb=(0.6309*avg_bpm + 0.1988*user_arr[2] + 0.2017*user_arr[1] - 55.0969) / 4.184;
    }
    return cb*10/60;

}

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  BTSerial.begin(9600);
  Serial.begin(9600);
  
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.blinkOnPulse(LED_PIN);
  pulseSensor.setThreshold(THRESHOLD);
  if (pulseSensor.begin())
  {
    Serial.println("PulseSensor object created successfully!");
  }

  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -
}

void loop() {
    int currentBPM = pulseSensor.getBeatsPerMinute();
    if (pulseSensor.sawStartOfBeat())
    {
      bpm_sum += currentBPM;
      p++;
      String msg="Current BPM: "+String(currentBPM)+" Calories Burnt: "+String(tot_cb);
      Serial.println(msg);
      BTSerial.println(msg);
    }
    if (p == 200)
    {
      tot_cb+=get_cal_burnt(bpm_sum/200.0);
    }
    if((digitalRead(10) == 1)||(digitalRead(11) == 1))
    {
      //Serial.println('!');
    }
    else{
        // send the value of analog input 0:
        Serial.println(analogRead(1));
//        BTSerial.println(analogRead(1));
        }
    delay(10);
  } 
