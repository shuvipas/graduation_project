#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define analogVin A0          // Analog voltage input to A0

Adafruit_MCP4725 MCP4725; 

float vol_mean =0;
float voltageRead = 0;
float vcc = 5.0;
int measurement_times = 5;
int MCP4725_value = 0;//if it doesn't work we could try diffrent data types like 'uint32_t'
const long R2 = 100;
const long R3 = 1000;
const long R4 = 10000;
const long R5 = 100000;
const long R6 = 1000000;

long resistor_finder(double current)
{
    if(current >= 0.01) 
    {
      return R2;
    }
    else if(current< 0.01) 
    {
      return R3;
    }
    else if(current< 0.001) 
    
    {
      return R4; 
    }
    else if(current< 0.0001) 
    
    {
      return R5; 
    }
     else if(current< 0.00001) 
    
    {
      return R6; 
    }
    
}
void setup() {
  Serial.begin(9600);
  MCP4725.begin(0x60); // Default I2C Address of MCP4725 
  Serial.setTimeout(1);
}

void loop() {
  while(Serial.available() == 0){}


  double current[6] = {0.01, 0.001, 0.0001,0.00001,0.000001,0.0000001};
  for(int i=0; i<6; i++)
  {
    int resistor = resistor_finder(current[i]);// R = 1*10^resistor (ohm) 
    float v_dac = current[i]*resistor;
    MCP4725_value = MCP4725_res*(v_dac/vcc);        
    MCP4725.setVoltage(MCP4725_value, false);  //setVoltage(value, storeflag(saves val for later use)) 
    for(int i=0; i<measurement_times;i++)
    {
        adcValueRead = analogRead(analogVin);
        Serial.println(adcValueRead); 
        //voltageRead = (adcValueRead * vcc)/ resistor;
       // vol_mean += voltageRead;
       delay(200);// 2 sec wait
        
        
        
    }
   
    
  } 
  
}
