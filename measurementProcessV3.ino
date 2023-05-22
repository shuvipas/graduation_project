#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define ON HIGH
#define OFF LOW

Adafruit_MCP4725 DAC; 
const int InAmpVout = A0;
const int DacVout = A1;
const int measurement_times = 2;
const int DAC_res = 4096;
const long reportInterval = 200; // How often to write the result to serial in milliseconds

const int HANDSHAKE = 0;
const int CASE_1 = 1;

//depends on the digital pins but the values must be continues 
const int R2 = 2;// 100 ohm
const int R3 = 3; // 1 K*ohm
const int R4 = 4; // 10 K*ohm
const int R5 = 5; // 100 K*ohm
const int R6 = 6; // 1 M*ohm


void setup() {
  Serial.begin(115200);
  DAC.begin(0x60); // Default I2C Address of MCP4725 
  Serial.setTimeout(2);
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
  if(inByte == CASE_1) {test = 1;}
  else if (inByte == HANDSHAKE)
     { 
      if (Serial.availableForWrite())
        {
    Serial.println("Message received.");
        }
    test = 0;
  }
  
  if(test)
  {
  
    for(int i = R2; i <= R6; i++){
      //int i = 4;
      digitalWrite(R2, OFF);
      digitalWrite(R3, OFF);
      digitalWrite(R4, OFF);
      digitalWrite(R5, OFF);
      digitalWrite(R6, OFF);
      
      digitalWrite(i, ON);// turn on the specific resistor
    //  digitalWrite(R4, ON);
      int DAC_value[20] = {200, 400, 600, 800, 1000, 
                           1200, 1400, 1600, 1800, 2000,
                           2200, 2400, 2600, 2800, 3000,
                           3200, 3400, 3600, 3800, 4000};  // If it doesn't work we could try diffrent data types like 'uint32_t'     
      for(int j = 0; j < 20; j++)
      {  

      DAC.setVoltage(DAC_value[j], false);  //setVoltage(value, storeflag(saves val for later use)) 
      int measurement = 0;
      unsigned long timeLastWrite = millis();

      while(measurement < measurement_times)
      {
          unsigned long currTime = millis(); // Grab the current time
          if(currTime - timeLastWrite >= reportInterval)
          {
            timeLastWrite = currTime;
            int adcVal = analogRead(InAmpVout);
            int dacVal = analogRead(DacVout);
            Serial.println(i); // what resistor is turned on R = 10^i
            Serial.println(dacVal); // the dac val 
            Serial.println(adcVal); 
            measurement++;
          }  
      }
     } //for(DAC_value) 
   } //for(R)
    Serial.println("Done");
  } //if(test)
 }//if (Serial.available() > 0)
}//void loop
