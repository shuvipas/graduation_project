#include <Adafruit_MCP4725.h>
#define CHIP_VOLT A0  // volt on the chip
#define DIG_POT_OUT A1  // output from the DAC needs to be equal to A2
#define RET_In A2 //The feedback voltege needs to be equal to A1
#define ON HIGH
#define OFF LOW

Adafruit_MCP4725 MCP4725;
 
float ADC_res = 1024.0;
float V_ref = 5.0;
int dac_out_read = 0;
int feedback_volt_read = 0;
int voltage_chip_read = 0;
float dac_out = 0.0;
float feedback_volt = 0.0;
float voltage_out_inA = 0.0;
float voltage_chip = 0.0;

//depends on the digital pins but the values must be continues 
const int R2 = 2;// 100 ohm
const int R3 = 3; // 1 K*ohm
const int R4 = 4; // 10 K*ohm
const int R5 = 5; // 100 K*ohm
const int R6 = 6; // 1 M*ohm

 //The integer value range is 0-4095 (2^12). 
const int DAC_volt = 2048; // DAC_volt= 2.5(v)   .DAC(volt) =  4096/5  (4096 <= 12bit, 5(v) from the arduino



void setup(void){
  Serial.begin(115200);
  MCP4725.begin(0x60); // Default I2C Address of MCP4725
  Serial.println("---Generating different currents---");
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  pinMode(R5, OUTPUT);
  pinMode(R6, OUTPUT);
}

void loop(void){
    for(int i = 2; i <= 6; ++i){
      // The pins D2-D6 are used to switch between the five different resistors.
      digitalWrite(R2, OFF);
      digitalWrite(R3, OFF);
      digitalWrite(R4, OFF);
      digitalWrite(R5, OFF);
      digitalWrite(R6, OFF);
      
      digitalWrite(i, ON);// turn on the specific resistor         
      
      MCP4725.setVoltage(DAC_volt, false);      
      dac_out_read = analogRead(DIG_POT_OUT);
      feedback_volt_read = analogRead(RET_In);
      voltage_chip_read = analogRead(CHIP_VOLT); 
      dac_out = (dac_out_read * V_ref) / ADC_res;
      feedback_volt = (feedback_volt_read * V_ref) / ADC_res;
      voltage_out_inA = (voltage_chip_read * V_ref) / ADC_res;
      voltage_chip = voltage_out_inA * (1 / (18.0 / (47 + 18))); //Calculating the voltage drop bebore the voltage divider.
      
      
      Serial.println(" ");
      Serial.print("DIG_POT_OUT (DAC output) = ");
      Serial.println(dac_out);
      Serial.print("RET_In (feedback voltege) = ");
      Serial.println(feedback_volt);
      Serial.print("CHIP_VOLT = ");
      Serial.println(voltage_out_inA);
      Serial.print("InAmp output = ");
      Serial.println(voltage_chip);
      Serial.print("R = ");
      Serial.println(i);
      Serial.println(" ");
      
      delay(1000);
    }
}
