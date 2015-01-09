#include <TFT.h> // Arduino LCD library
#include <SPI.h>
#include <Serial.h>
#include <TimerOne.h>
#include "max6675.h"
#include <PID_v1.h>

// pin definition for the Uno
#define cs 10
#define dc 9
#define rst 8
const int PinCLK = 2; // Used for generating interrupts using CLK signal
const int PinDT = 4; // Used for reading DT signal
const int PinSW = 3; // Used for the push button switch
const int thermoDO = 12;
const int thermoCS = 5;
const int thermoCLK = 13;
double Output = 0.0;
double temp_read = 0.0;
 


double TempCel = 0;
volatile int stage = 0;
volatile int button_state = 0;
volatile int time[4] = {0};
// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
 
// char array to print to the screen
char sensorPrintout[4];
char Printout1[2];
char Printout2[2];
char Printout3[2];
char Printout4[2];

PID myPID(&temp_read,&Output,&TempCel,2,0,0,DIRECT); 
MAX6675 thermocouple(thermoCLK,thermoCS,thermoDO);
void isr();
void isr2();
void TimerISR();
void loop();
void setup();
 
 
void isr () {
  if(stage == 0){
  if (!digitalRead(PinDT))
  {
  TempCel = TempCel + 5;
  }
  else
  {
  TempCel = TempCel - 5;
  } // isr
  }
  if(stage == 1)
  {
  if (!digitalRead(PinDT))
  {
  time[3] = time[3]+ 5;
  }
  else
  {
  time[3] = time[3] - 5;
  }
  }
}

void isr2(){
if (!digitalRead(PinSW))
stage++;
}
void TimerISR()
{
time[3]--;
}
 
void setup() {
// Put this line at the beginning of every sketch that uses the GLCD:
Serial.begin(9600);
TFTscreen.begin();
// clear the screen with a black background
TFTscreen.background(0, 0, 0);
// write the static text to the screen
// set the font color to white
TFTscreen.stroke(255,255,255);
pinMode(PinCLK,INPUT);
pinMode(PinDT, INPUT);
pinMode(PinSW, INPUT);
pinMode(6,OUTPUT);
attachInterrupt(0, isr, FALLING); // interrupt 0 is always connected to pin 2 on Arduino Promini
attachInterrupt(1, isr2, FALLING);
myPID.SetOutputLimits(0,255);
myPID.SetSampleTime(2);
}
 
void loop() {
Serial.println(stage);
while(stage == 0)
{
TFTscreen.background(0, 0, 0);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(2);
TFTscreen.text("Temperature",50,30);
// Read the value of the sensor on A0
String sensorVal = String(TempCel);
// convert the reading to a char array
sensorVal.toCharArray(sensorPrintout, 4);
 
// set the font color
TFTscreen.stroke(255,255,255);
// set the font size
TFTscreen.setTextSize(3);
// print the sensor value
TFTscreen.text(sensorPrintout, 50, 50);
// wait for a moment
delay(200);
// erase the text you just wrote
// TFTscreen.stroke(0,0,0);
// TFTscreen.text(sensorPrintout, 0, 20);
}
while (stage == 1)
{
TFTscreen.background(0, 0, 0);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(2);
TFTscreen.text("Time", 50, 30);
if(time[0] >= 0 && time[1] >= 0 && time[2] >= 0 && time[3] >= 0)
{
if (time[3] > 9)
{
time[2]++;
time[3] = 0;
}
if (time[2] >5)
{
time[1]++;
time[2] = 0;
time[3] = 0;
}
if (time[1] > 9)
{
time[0]++;
time[1] = 0;
time[2] = 0;
time[3] = 0;
}
}
else
{
time[0] = 0;
time[1] = 0;
time[2] = 0;
time[3] = 0;
}
// String secondVal = String(second);
// secondVal.toCharArray(secondPrintout,3);
String second_one = String(time[3]);
second_one.toCharArray(Printout4,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout4, 90, 70);
String second_two = String(time[2]);
second_two.toCharArray(Printout3,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout3, 70, 70);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(":", 55, 70);
String minute_one = String(time[1]);
minute_one.toCharArray(Printout2,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout2, 40, 70);
String minute_two = String(time[0]);
minute_two.toCharArray(Printout1,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout1, 20, 70);
delay(200);
}
while(stage == 2)
{
    TFTscreen.background(0, 0, 0);
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(TimerISR);
      if((time[0] == 0 && time[1] == 0 && time[2] == 0 && time[3] < 0))
      {
      Timer1.stop();
      stage++;
      break;
      }
      if (time[0] > 0 && time[1] ==0 && time[2] ==0 && time[3] < 0)
      {
      time[0]--;
      time[1] = 9;
      time[2] = 5;
      time[3] = 9;
      }
      if (time[0] >= 0 && time[1] > 0 && time[2] == 0 && time[3] < 0)
      {
      time[1]--;
      time[2] = 5;
      time[3] = 9;
      }
      if ((time[1] == 0 && time[2] >= 0 || time[2] <6) && time[3] < 0)
      {
      time[2]--;
      time[3] = 9;
      }

digitalWrite(cs,LOW);
digitalWrite(thermoCS,HIGH);
temp_read = thermocouple.readCelsius();
Serial.println(temp_read);
digitalWrite(thermoCS,LOW);
digitalWrite(cs,HIGH);
myPID.SetMode(AUTOMATIC);
myPID.Compute();
analogWrite(6,Output);
String second_one = String(time[3]);
second_one.toCharArray(Printout4,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout4, 90, 70);
String second_two = String(time[2]);
second_two.toCharArray(Printout3,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout3, 70, 70);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(":", 55, 70);
String minute_one = String(time[1]);
minute_one.toCharArray(Printout2,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout2, 40, 70);
String minute_two = String(time[0]);
minute_two.toCharArray(Printout1,2);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text(Printout1, 20, 70);

delay(500);
}
while(stage == 3)
{
TFTscreen.background(0, 0, 0);
TFTscreen.stroke(255,255,255);
TFTscreen.setTextSize(3);
TFTscreen.text("Finished", 20, 70);
}
} 
