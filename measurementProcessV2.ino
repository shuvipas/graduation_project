#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit

Adafruit_MCP4725 DAC; 
const int analogVin = A0;
//float voltageRead = 0;
const float vcc = 5.0;
const int measurement_times = 5;
const float DAC_res = 4096.0;
const long reportInterval = 200; // How often to write the result to serial in milliseconds




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
  DAC.begin(0x60); // Default I2C Address of MCP4725 
  Serial.setTimeout(1);
  
}

void loop() {
  while(Serial.available() == 0){}
  

  double current[6] = {0.01, 0.001, 0.0001,0.00001,0.000001,0.0000001};
  for(int i=0; i<6; i++)
  {
    int resistor = resistor_finder(current[i]);// R = 1*10^resistor (ohm) 
    float v_dac = current[i]*resistor;
    int DAC_value = DAC_res*(v_dac/vcc);  // If it doesn't work we could try diffrent data types like 'uint32_t'     
    DAC.setVoltage(DAC_value, false);  //setVoltage(value, storeflag(saves val for later use)) 
    int measurement = 0;
    unsigned long timeLastWrite = millis();
    
    while(measurement < measurement_times)
    {
        unsigned long currTime = millis(); // Grab the current time
        if(currTime - timeLastWrite >= reportInterval)
        {
        timeLastWrite = currTime;
        int adcVal = analogRead(analogVin);
        Serial.println(adcVal); 
        measurement++;
        }  
    }
 } 
  
}
