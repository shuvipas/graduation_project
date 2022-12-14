#include <Wire.h>             //wire library
#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define analogVin A0          // Analog voltage input to A0

Adafruit_MCP4725 MCP4725; 

int MCP4725_value = 0;//if it doesn't work we could try diffrent data types like 'uint32_t'
int adcValueRead = 0;
float voltageRead = 0;
float MCP4725_expected_output;
float diff;

    
void setup(void) {
  Serial.begin(9600);
  MCP4725.begin(0x60); // Default I2C Address of MCP4725 
  Serial.println("Generating different voltages:");
  
}

void loop(void) {
    
    if(MCP4725_value < 4096){
      MCP4725_expected_output = (5.0/4096.0) * MCP4725_value;
      MCP4725.setVoltage(MCP4725_value, false);  //setVoltage(value, storeflag(saves val for later use)) 
      delay(500);
      adcValueRead = analogRead(analogVin);
      voltageRead = (adcValueRead * 5.0 )/ 1024.0;
      
      
      Serial.print("Expected Voltage: ");
      Serial.print(MCP4725_expected_output,3);
      
        
      Serial.print("\t DAC Voltage from the ADC: ");      
      Serial.println(voltageRead,3);

      diff = voltageRead - MCP4725_expected_output;
      if(diff >0){
      Serial.print("the diffrence is: ");
      diff = voltageRead - MCP4725_expected_output;
      Serial.println(abs(diff));
      }
      else{
        Serial.println("there is no diffrence");
        }
      Serial.println();
      delay(500);
      MCP4725_value = MCP4725_value + 250;
      }
}
