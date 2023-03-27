

#include <Wire.h>             //wire library
#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define analogVin A0          // Analog voltage input to A0

Adafruit_MCP4725 MCP4725; 

int MCP4725_value = 0;//if it doesn't work we could try diffrent data types like 'uint32_t'
int adcValueRead = 0;
float voltageRead = 0;
float vcc = 5.0;
float MCP4725_max = 4096.0;
float voltege_in =0;
int resistor =0;
int res_scale =0;
int cur_scale = 0;
float current =0;

void setup(void) {
  Serial.begin(9600);
  MCP4725.begin(0x60); // Default I2C Address of MCP4725 
  Serial.println("Generating different currents:");
  
}

void loop(void) {
    Serial.print("Write voltege for the circuit:\n")
    while (Serial.available() == 0) 
    {
    }
    voltege_in = Serial.parseFloat();
    MCP4725_value = voltege_in*(MCP4725_max/vcc);        
    MCP4725.setVoltage(MCP4725_value, false);  //setVoltage(value, storeflag(saves val for later use)) 
                
    Serial.print("Write integer value of the resistor: \n")
    while (Serial.available() == 0) 
    {
    }
    resistor = Serial.parseInt()
    Serial.print("Write scale value of the resistor: \n") //for nano write -9
    while (Serial.available() == 0) 
    {
    }
    res_scale = Serial.parseInt();
    current = voltege_in/resistor;
    cur_scale = 1/res_scale;
        
    Serial.print("expected current: %d*10^%d [A]\n",current, cur_scale);
    // delay(500);// 2 sec. we dont need becuse were waiting for user input.

    adcValueRead = analogRead(analogVin);
    voltageRead = (adcValueRead * vcc)/ 1024.0;
    Serial.print("The voltege read from the CAL: %f\n", voltegeRead);
    Serial.print("\n\n\n");


}
