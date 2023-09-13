#include <Adafruit_MCP4725.h>
#define CHIP_VOLT A0  
#define DIG_POT_OUT A1         
#define RET_In A2

Adafruit_MCP4725 MCP4725;
 
float ADCres = 1024.0;
float Vref = 5.0;
int dacOutRead = 0;
int feedbackVoltRead = 0;
int voltageChipRead = 0;
float dacOut = 0.0;
float feedbackVolt = 0.0;
float voltageOutInA = 0.0;
float voltageChip = 0.0;

void setup(void){
  Serial.begin(9600);
  MCP4725.begin(0x60); 
  Serial.println("---Generating different currents---");
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop(void){
    for(int i = 2; i <= 6; ++i){
      // The pins D2-D6 are used to switch between the five different resistors.
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
           
      digitalWrite(i, HIGH);             
      MCP4725.setVoltage(2048, false); //The integer value range is 0-4095 (2^12).       
      dacOutRead = analogRead(DIG_POT_OUT);
      feedbackVoltRead = analogRead(RET_In);
      voltageChipRead = analogRead(CHIP_VOLT); 
      dacOut = (dacOutRead * Vref) / ADCres;
      feedbackVolt = (feedbackVoltRead * Vref) / ADCres;
      voltageOutInA = (voltageChipRead * Vref) / ADCres;
      voltageChip = voltageOutInA * (1 / (18.0 / (47 + 18))); //Calculating the voltage drop bebore the voltage divider.
      Serial.print("DIG_POT_OUT = ");
      Serial.println(dacOut);
      Serial.print("RET_In = ");
      Serial.println(feedbackVolt);
      Serial.print("CHIP_VOLT = ");
      Serial.println(voltageOutInA);
      Serial.print("InAmp output = ");
      Serial.println(voltageChip);
      Serial.print("R = ");
      Serial.println(i);
      delay(1000);
    }
}
