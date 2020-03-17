//Libraries
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
#include <EEPROM.h>
#define DSADDR 0x70
extern "C" {}
//#include "utility/twi.h"  // from Wire library to enable bus scanning

//Assign Pins
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3
int hapPin = 6;
int hapPin2 = 9;
int buttonPin = 5;
int buttonState = 0;
int pwrPin = 11;
int buzzer = 0;
int buzzer2 = 0;

//Assign Variables and arrays
int addr = 0;
int value[5];
int sens[5];

//Set up Distance Sensor
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);  //Initiate Distance Sensor

void setup() {
  Serial.begin(921600);                    //Initiate Serial Monitor (debug)
  Serial.println("It turns on");

  pinMode(hapPin, OUTPUT); //Set pin modes
  pinMode(hapPin2, OUTPUT);
  pinMode(pwrPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  analogWrite(pwrPin, 143);  //Output 3.3v through pin 11 as power

  Serial.println("It sets pin modes");

  delay(1000);

  Wire.begin();   //Initialize the wire library

  for (int i = 0; i < 6; i++)
  {
    select(i);
    distanceSensor.setDistanceModeLong();  //Set distance sensor mode
    Serial.println("Sensor Mode Set");
  }

  for (int i = 0; i < 6; i++)
  {
    EEPROM.get(i , value[i]);   //Read the stored distance value from EEPROM
  }

  Serial.println("VL53L1x Test");
  for (int i = 0; i < 6; i++)
  {
    select(i);
    if (distanceSensor.init() == false)    //Check to see if the distance sensor is connected (debug)
      Serial.println("Sensor Attached");
  }

}

void loop() {
  buttonState = digitalRead(buttonPin); //Check to see if the button is pressed
  if (buttonState == HIGH)
  {
    Serial.println("Button Pressed");
    for (int a = 0; a < 6; a++)
    {
      select(a);
      distanceSensor.startRanging();
      value[a] = distanceSensor.getDistance();   //Calibrate all sensors
      distanceSensor.stopRanging();

      EEPROM.put(a, value);                      //Store the calibrated value in EEPROM

    }
  }
  else
  {
    for (int i = 1; i < 5; i++)
    {
      select(i);
      distanceSensor.startRanging();
      sens[i] = distanceSensor.getDistance();    //Check Distance
      distanceSensor.stopRanging();

      Serial.print("Distance(mm): ");             //Output Distance (debug)
      Serial.println(sens[i]);
      Serial.println(value[i]);
      Serial.println(i);
      Serial.println();
      if (i < 3)
      {
          if (sens[i] < (value[i] - 35) or sens[i] > (value[i] + 35))
           {
              buzzer = 1;
              if (buzzer2 == 1)
              {
              analogWrite(hapPin, 153); //Activate Buzzers
              delay(10);
              break;
              }
              else
              {
                analogWrite(hapPin, 86);
                break;
              }
           }

          else if (i == 2)
           {
            buzzer = 0;
            Serial.println("Sensor");
            analogWrite(hapPin, 0);
             
           }
      }

      else if (i > 2)
        {
          Serial.println("Code");
          if (sens[i] < (value[i] - 35) or sens[i] > (value[i] + 35))
           {
              buzzer2 = 1;
              if (buzzer == 1)
              {
                delay(10);
                analogWrite(hapPin2, 153);
                break;
              }
              else
              {
                analogWrite(hapPin2, 86); //Activate Buzzers
                Serial.println("Buzzers Activate");
                return;
              }
           }
          
          else if (i == 4)
          {
              buzzer2 = 0;
              analogWrite(hapPin2, 0);
          }
        }
         
      }
    }

 }
    
void select(uint8_t i)    //Select different I2C device on multiplexer
{
  if (i > 7) return;

  Wire.beginTransmission(DSADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
  //Serial.println("Multiplexer Command Sent");
}
