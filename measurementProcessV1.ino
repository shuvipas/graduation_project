#include <Wire.h>             //wire library
#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define analogVin A0          // Analog voltage input to A0

Adafruit_MCP4725 MCP4725; 

int MCP4725_value = 0;//if it doesn't work we could try diffrent data types like 'uint32_t'
int adcValueRead = 0;
float voltageRead = 0;
float vcc = 5.0;
float MCP4725_res = 4096.0;
float ADC_res = 1024.0;
float voltege_in =0;
int resistor =0;
int res_scale =0;
int cur_scale = 0;
float current =0;
float vol_mean =0;
int measurement_times = 5;

void setup(void) 
{
  Serial.begin(9600);
  MCP4725.begin(0x60); // Default I2C Address of MCP4725 
  Serial.println("Generating different currents:");
}

void loop(void) 
{
    Serial.print("Write float voltege for the circuit:\n")
    while (Serial.available() == 0) {}
    voltege_in = Serial.parseFloat();
    MCP4725_value = MCP4725_res*(voltege_in/vcc);        
    MCP4725.setVoltage(MCP4725_value, false);  //setVoltage(value, storeflag(saves val for later use))       
    
    Serial.print("Write integer value of the resistor: \n")//for R = 10k >> write 10
    while (Serial.available() == 0) {}
    resistor = Serial.parseInt()
    
    Serial.print("Write integer scale value of the resistor: \n") //for K write 3
    while (Serial.available() == 0){}
    res_scale = Serial.parseInt();
    
    current = voltege_in/resistor;
    cur_scale = 1/res_scale;
        
    Serial.print("expected current: %d*10^%d [A]\n",current, cur_scale);
    // delay(2000);// 2 sec. we dont need because were waiting for user input.

    for(int i=0; i<measurement_times;i++)
    {
        adcValueRead = analogRead(analogVin);
        voltageRead = (adcValueRead * vcc)/ ADC_res;
        vol_mean += voltageRead;
        Serial.print("%d) The voltege read from the DUT: %f\n",i, voltegeRead); // DUT device under test
        dealy(2000);// 2 sec wait
    }
    vol_mean = vol_mean/measurement_times;
    Serial.print("The mean voltege read from the DUT: %f\n", vol_mean); // DUT device under test
    Serial.print("\n\n\n");
}
