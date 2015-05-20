#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SPI.h>
#include <Serial.h>
#include <TimerOne.h>
#include "max6675.h"
//#include <PID_v1.h>

// pin definition for the Uno
#define cs 10
#define dc 9
#define rst 8
const int PinSW1 = 2; // Used for generating interrupts using CLK signal
const int PinSW2 = 3; // Used for reading DT signal
const int PinSW3 = 4; // Used for the push button switch
const int thermoDO = 12;
const int thermoCS = 5;
const int thermoCLK = 13;
double Output = 0.0;
double temp_read = 0.0;
 

//variables for calculation and collecting data
double TempCel = 0;
volatile int stage = 0;
volatile int button_state = 0,start = 0;
volatile int time[4] = {0};
double K[3] ={0.0}, pid[3] = {0.0},pid_sum = 0.0,error = 0.0,Sum_error = 0.0,previous_error = 0.0,ratio_setpoint = 0.0, ratio_process = 0.0;
// Software SPI (slower updates, more flexible pin options):
// pin 13 - Serial clock out (SCLK)
// pin 11 - Serial data out (DIN)
// pin 9 - Data/Command select (D/C)
// pin 10 - LCD chip select (CS)
// pin 8 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(13,11,dc,cs,rst);
// char array to print to the screen
char sensorPrintout[4];
char Printout1[2];
char Printout2[2];
char Printout3[2];
char Printout4[2];


//#define LOGO16_GLCD_HEIGHT 16
//#define LOGO16_GLCD_WIDTH  16

//static const unsigned char PROGMEM logo16_glcd_bmp[] =
//{ B00000000, B11000000,
//  B00000001, B11000000,
//  B00000001, B11000000,
//  B00000011, B11100000,
//  B11110011, B11100000,
//  B11111110, B11111000,
//  B01111110, B11111111,
//  B00110011, B10011111,
//  B00011111, B11111100,
//  B00001101, B01110000,
//  B00011011, B10100000,
//  B00111111, B11100000,
//  B00111111, B11110000,
//  B01111100, B11110000,
//  B01110000, B01110000,
//  B00000000, B00110000 };

//PID myPID(&temp_read,&Output,&TempCel,1.6,0,1,DIRECT); 
MAX6675 thermocouple(thermoCLK,thermoCS,thermoDO);
//void isr();
//void isr2();
void TimerISR();
void loop();
void setup();
// 
////function for rotary 
//void isr () {
//  if(stage == 0){
//  if (!digitalRead(PinSW1))
//  {
//  TempCel = TempCel + 5;
//  }
////  else
////  {
////  TempCel = TempCel - 5;
////  } // isr
//  }
//  if(stage == 1)
//  {
//  if (!digitalRead(PinSW1))
//  {
//  time[3] = time[3]+ 5;
//  }
//  else
//  {
//  time[3] = time[3] - 5;
//  }
//  }
//}

//void isr2(){
//if (!digitalRead(PinSW)){
//stage++;
//}
//}
void TimerISR()
{
time[3]--;
}
 
void setup() {
//begin serial
Serial.begin(9600);
// 
display.begin();
display.setContrast(50);
// write the static text to the screen
display.setTextSize(2);
display.setTextColor(BLACK);
display.setCursor(0,0);
display.println("Welcome");
display.display();
delay(2000);
display.clearDisplay();
pinMode(PinSW1,INPUT_PULLUP);
pinMode(PinSW2,INPUT_PULLUP);
pinMode(PinSW3,INPUT_PULLUP);
pinMode(6,OUTPUT);analogWrite(6,0);
//attachInterrupt(0, isr, RISING); // interrupt 0 is always connected to pin 2 on Arduino Promini
//attachInterrupt(1, isr2, FALLING);
//myPID.SetOutputLimits(0,255);
//myPID.SetSampleTime(2);
}
 
void loop() {
//Serial.println(stage);
while(stage == 0)
{
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(0,0);
display.println("Temperature");
if(!digitalRead(PinSW1))
{
TempCel = TempCel + 5;
delay(100);
}
if(!digitalRead(PinSW2))
{
TempCel = TempCel - 5;
delay(100);
}
display.setTextSize(2);
display.setTextColor(BLACK);
display.setCursor(10,20);
display.print(TempCel);
//display.clear();
display.display();
analogWrite(6,0);
if(!digitalRead(PinSW3))
{
stage++;
delay(500);
}
//delay(100);
// erase the text you just wrote
// TFTscreen.stroke(0,0,0);
// TFTscreen.text(sensorPrintout, 0, 20);
}
while (stage == 1)
{
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(BLACK);
display.setCursor(20,0);
display.println("Time"); 
if(start == 0)
{
time[0] = 0;  
time[1] = 0;
time[2] = 0;
time[3] = 0;
start++;
}
  if(!digitalRead(PinSW1))
  {
  time[3] = time[3] + 5;
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
  delay(40);
  }
  if(!digitalRead(PinSW2))
  {
  time[3] = time[3] - 5;
    if(time[2] > 0 && time[3] < 0)
    {
    time[2]--;
    time[3] = 5;
    }
    if(time[1] > 0 && time[2] == 0 && time[3] < 0)
    {
    time[1]--;
    time[2] = 5;
    time[3] = 5;
    }
    if(time[0] > 0 && time[1] == 0 && time[2] == 0 && time[3] < 0)
    {
    time[0] --;  
    time[1] = 9;
    time[2] = 5;
    time[3] = 5;
    }
    if(time[0] == 0 && time[1] == 0 && time[2] == 0 && time[3] < 0)
    {
    time[0] = 0;  
    time[1] = 0;
    time[2] = 0;
    time[3] = 0;
    }
  delay(40);
  }  

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
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(20,30);
display.print(time[0]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(30,30);
display.print(time[1]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(35,30);
display.print(":");
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(40,30);
display.print(time[2]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(50,30);
display.print(time[3]);
display.display();
analogWrite(6,0);
if(!digitalRead(PinSW3))
{
stage++;
delay(500);
}
//delay(200);
}
while(stage == 2)
{
    display.clearDisplay();
    
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(TimerISR);
    
    if(time[2] > 0 && time[3] < 0)
    {
    time[2]--;
    time[3] = 9;
    }
    if(time[1] > 0 && time[2] == 0 && time[3] < 0)
    {
    time[1]--;
    time[2] = 5;
    time[3] = 9;
    }
    if(time[0] > 0 && time[1] == 0 && time[2] == 0 && time[3] < 0)
    {
    time[0] --;  
    time[1] = 9;
    time[2] = 5;
    time[3] = 9;
    }
    if(time[0] == 0 && time[1] == 0 && time[2] == 0 && time[3] == 0)
    {
    stage++;
    }
temp_read = thermocouple.readCelsius();    
Serial.print(temp_read);
K[0] = 0.8;
K[1] = 0.0;
K[2] = 0.3;
ratio_setpoint = TempCel/200.0;
ratio_process = temp_read/200.0;
error = ratio_setpoint - ratio_process;
pid[0] = K[0] * error;
pid[1] = (K[1] * (Sum_error + error)/2);
pid[2] = (K[2] * 2 *(error - previous_error));
pid_sum = pid[0] + pid[1] + pid[2];
if (pid_sum > 1.0)
{
pid_sum = 1.0;
}
if (pid_sum < 0)
{
pid_sum = 0;
}
Output = pid_sum *255.0;
analogWrite(6,Output);
Serial.print(",");
Serial.println(Output);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(20,10);
display.print(time[0]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(30,10);
display.print(time[1]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(35,10);
display.print(":");
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(40,10);
display.print(time[2]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(50,10);
display.print(time[3]);
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(20,30);
display.print(temp_read);
display.display();
if(!digitalRead(PinSW3))
{
stage++;
delay(500);
}
delay(500);
}
while(stage == 3 && digitalRead(PinSW3))
{
Timer1.stop();  
Timer1.detachInterrupt();
analogWrite(6,0);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(20,20);
display.println("Finished");
display.display();
delay(3000);
start = 0;
stage = 0;
}
while(stage == 3 && !digitalRead(PinSW3))
{
Timer1.stop();  
Timer1.detachInterrupt();
analogWrite(6,0);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(20,20);
display.println("Restart");
display.display();
delay(3000);
start = 0;
stage = 0;
}
}
