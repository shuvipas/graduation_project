#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define ON HIGH
#define OFF LOW
Adafruit_MCP4725 DAC; 
const int analogVin = A0;
//float voltageRead = 0;
//const float vcc = 5.0;
const int measurement_times = 5;
const int DAC_res = 4096;
const long reportInterval = 200; // How often to write the result to serial in milliseconds
//const int HANDSHAKE = 0;
//const int CASE_1 = 1;

//depends on the digital pins but the values must be continues 
const int R2 = 0;// 100 ohm
const int R3 = 1; // 1000 ohm == 1 k*ohm
const int R4 = 2; // 10000 ohm == 10 k*ohm
const int R5 = 3; // 100000 ohm == 100 k*ohm
const int R6 = 4; // 1000000 ohm == 1 M*ohm


void setup() {
  Serial.begin(9600);
  DAC.begin(0x60); // Default I2C Address of MCP4725 
  Serial.setTimeout(1);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  pinMode(R5, OUTPUT);
  pinMode(R6, OUTPUT);
  
}

void loop() {

if (Serial.available() > 0) {
  int inByte = Serial.read();
  int test = 0;
  if(inByte ==1) {test=1;}
  else if (inByte ==0 &&Serial.availableForWrite())
  {
    Serial.println("Message received.");
    test=0;
  }
  
  if(test)
  {
    for(int i=R2; i<=R6; i++)
    {

      digitalWrite(R2, OFF);
      digitalWrite(R3, OFF);
      digitalWrite(R4, OFF);
      digitalWrite(R5, OFF);
      digitalWrite(R6, OFF);

      digitalWrite(i, ON);// turn on the specific ressitor


      int DAC_value = 0;  // If it doesn't work we could try diffrent data types like 'uint32_t'     
      while(DAC_value<DAC_res)
      {  

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
            Serial.println(i); // what resistor is turned on R = 10^i
            Serial.println(DAC_value); // the dac val 
            Serial.println(adcVal); 
            measurement++;
          }  
      }
      DAC_value += 200;
      } 
    }
    Serial.println("Done");
  } //if(test)
}//if (Serial.available() > 0)
}//void loop
