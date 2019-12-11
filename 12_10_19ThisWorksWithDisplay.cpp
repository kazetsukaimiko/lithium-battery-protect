/*
Program to turn relays on and off based on voltage and temperature
*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"


//=======================================================================Relays
int relPin1 = 2;                        //Main battery cutoff NO
int relPin2 = 3;                        //Cabin cooling fan NO
int relPin3 = 4;                        //Heating pad for batteries NO
int relPin4 = 5;                        //TBD  NO

int relPin1Val;
int relPin2Val;
int relPin3Val;
int relPin4Val;


#define RA8875_INT 8
#define RA8875_CS 10
#define RA8875_RESET 9
#define ONE_WIRE_BUS 6             //pin for DS18B20
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//================================================================VOLTS 1
int analogInput1 = A1;
float vout = 0.0;
float vin = 0.0;
float R1 = 35000.0; 
float R2 = 7500.0; 
int value = 0;
//================================================================VOLTS 2
int analogInput2 = A3;
float vout2 = 0.0;
float vin2 = 0.0;
float R12 = 35000.0; 
float R22 = 7500.0; 
int value2 = 0;

//=============================
void setup() {
  Serial.begin(9600);
  sensors.begin();
    if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
    tft.textTransparent(RA8875_WHITE);
  tft.textMode();
  sensors.begin();
  pinMode(relPin1, OUTPUT);      //Output Mode for Relay1
  pinMode(relPin2, OUTPUT);      //Output Mode for Relay2
  pinMode(relPin3, OUTPUT);      //Output Mode for Relay3
  pinMode(relPin4, OUTPUT);      //Output Mode for Relay4
  pinMode(analogInput1, INPUT);
  pinMode(analogInput2, INPUT);
}
//===============================================================TEMPERATURE
void loop() {
  sensors.requestTemperatures();
  float temperature1 = sensors.getTempFByIndex(0);
  Serial.print ("Battery temp ");
  Serial.print(sensors.getTempFByIndex(0));
  Serial.println ("F   ");


  float temperature2 = sensors.getTempFByIndex(1);
  Serial.print ("Cabin temp ");

  Serial.print(sensors.getTempFByIndex(1));
  Serial.println ("F   ");


  float temperature3 = sensors.getTempFByIndex(2);
  Serial.print ("Heater temp ");
  Serial.print(sensors.getTempFByIndex(2));
  Serial.println ("F   ");

  float temperature4 = sensors.getTempFByIndex(3);
  Serial.print ("Other temp ");
  Serial.print(sensors.getTempFByIndex(3));
  Serial.println ("F   ");
  Serial.println();
/*
      +++++++++++++++++++++++++++++++++++++++++++++++++++++      
      +                    NOTE                           +
      +     Temp and voltage values used are for testing  +
      +      final values are commented.                  +
      +++++++++++++++++++++++++++++++++++++++++++++++++++++
  */
  //============================================================Volt sense 1
  // read the value at analog input
  value = analogRead(analogInput1);
  vout = (value * 5.0) / 1024.0; 
  vin = vout / (R2 / (R1 + R2));
 
  //============================================================Volt sense 2
  value2 = analogRead(analogInput2);
  vout2 = (value2 * 5.0) / 1024.0; 
  vin2 = vout2 / (R22 / (R12 + R22));

  //Depending on the temperature switch============= Rel 1 Battery Temp high and low unhooks batteries from everything
  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(10, 0);
  tft.textEnlarge(1);
  tft.print ("Battery Volts ");
  tft.print(vin);
  tft.textSetCursor(10, 30);
  tft.print ("Battery Temp  ");
  tft.print(sensors.getTempFByIndex(0));
    tft.textSetCursor(10, 60);
  tft.print ("Room Temp     ");
  tft.print(sensors.getTempFByIndex(1));
  tft.textSetCursor(10, 90);
  tft.print ("Heater        ");
  tft.print(sensors.getTempFByIndex(2));

   //==============================================================TFT battery temperature
  if (temperature1 > 80)//should be 158
   

  {

    tft.textSetCursor(350, 30);
    tft.textTransparent(RA8875_RED);
    tft.print("  HIGH");

  }
  else if (temperature1 > 65 && temperature1 < 80)// should be 34 and 158
  {
    tft.textSetCursor(350, 30);
    tft.textTransparent(RA8875_GREEN);
    tft.print("  OK");
  }
  else if (temperature1 < 65) //should be 34
  {
    tft.textSetCursor(350, 30);
    tft.textTransparent(RA8875_RED);
    tft.print("  LOW");
  }
  //==============================================================TFT Volts
  if (vin > 2.3)//should be 15
    

  {

    tft.textSetCursor(350, 0);
    tft.textTransparent(RA8875_RED);
    tft.print("  HIGH");

  }
  else if (vin > 1 && vin < 2.3)// should be 11 and 15
  {
    tft.textSetCursor(350, 0);
    tft.textTransparent(RA8875_GREEN);
    tft.print("  OK");
  }
  else if (vin < 1) //should be 11
  {
    tft.textSetCursor(350, 0);
    tft.textTransparent(RA8875_RED);
    tft.print("  LOW");
  }
  
  if (temperature1 > 80 || vin > 2.3)//should be 158 and 15
  {
    relPin1Val =     LOW;
    Serial.println("Temp or Volts High");
  }
  else if (temperature1 > 65 && temperature1 < 80 && vin > 1 && vin < 2.3)// should be 34 and 158 and 11 and 15
  {
    relPin1Val = HIGH;
    Serial.println ("Temp and Volts OK");
  }
  else if (temperature1 < 65 || vin < 1) //should be 34 and 11
  {
    relPin1Val = LOW;
    Serial.println ("Temp or Volts Low");
  }

  //Depending on the temperature switch======================== Rel 2 Cabin Temp turns on cooling fan
  if (temperature2 > 80)//should be 90
  {
    relPin2Val = LOW;
    Serial.println("Cabin temp High");
  }
  else if (temperature2 < 80)  //should be 35 and 90
  relPin2Val = HIGH;
    Serial.println ("Cabin temp OK");
  {
    
  }
  //=======================================================================Cabin Temp TFT

  if (temperature2 > 80)//should be 90
  {
    tft.textSetCursor(350, 60);
    tft.textTransparent(RA8875_RED);
    tft.print("Fan ON");
  }
  else if (temperature2 < 80)  //should be 35 and 90
  {
    tft.textSetCursor(350, 60);
    tft.textTransparent(RA8875_GREEN);
    tft.print ("Fan Off");
 

  }
  //Depending on the temperature switch=========================== Rel 3 Battery Heater turns on heating pads
  if (temperature3 > 65)//should be 40
  {
    relPin3Val = HIGH;
    Serial.println("Heater Off");
  }
  else if ( temperature3 < 65)  //should be  40
  {
    relPin3Val = LOW ;

    Serial.println ("Heater On");
  }
  //===============================================================heat tft
  if (temperature3 > 65)//should be 40
  {
   tft.textSetCursor(350, 90);
    tft.textTransparent(RA8875_GREEN);
    tft.print("Heat Off");
  }
  else if (temperature3 < 65)  //should be  40
  {
  
 tft.textSetCursor(350, 90);
    tft.textTransparent(RA8875_RED);
    tft.print ("Heat On");
  }
  //==============================================================relay 4 unused
  {
    relPin4Val = HIGH;
    Serial.println("Relay 4 unused");
  }

  Serial.print("Battery Volts1 ");
  Serial.println(vin);

  Serial.print("Battery Volts2 ");
  Serial.println(vin2);
   tft.textSetCursor(350, 120);
    tft.textTransparent(RA8875_RED);
    tft.print (vin2);
  
  {

    digitalWrite(relPin1, relPin1Val);
    digitalWrite(relPin2, relPin2Val);
    digitalWrite(relPin3, relPin3Val);
    digitalWrite(relPin4, relPin4Val);
    delay(5000);
    tft.fillScreen(RA8875_BLACK);
    delay(1);
  }
}
  
