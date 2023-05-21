#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define ON HIGH
#define OFF LOW

Adafruit_MCP4725 DAC; 
const int analogVin = A0;
const int DacMeasurement = A1;
const int measurement_times = 2;
const int DAC_RES = 4096;
const long reportInterval = 200; // How often to write the result to serial in milliseconds
const int VCC = 5;

const int HANDSHAKE = 0;
const int SWEEP = 1;
const int USER_COMMAEND  = 2;

//depends on the digital pins but the values must be continues 
const int R2 = 2;// 100 ohm
const int R3 = 3; // 1 K*ohm
const int R4 = 4; // 10 K*ohm
const int R5 = 5; // 100 K*ohm
const int R6 = 6; // 1 M*ohm


void setup() {
  Serial.begin(115200);
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
  //int test = 0;
  if(inByte == SWEEP) {test = 1;}
  if (inByte == HANDSHAKE)
     { 
      if (Serial.availableForWrite())
        {
			Serial.println("Message received.");
        }
    test = 0;
  }
  else if(inByte == USER_COMMAEND)
  {
	  
	  digitalWrite(R1, OFF);
      digitalWrite(R2, OFF);
      digitalWrite(R3, OFF);
      digitalWrite(R4, OFF);
      digitalWrite(R5, OFF);
	
	  while(true){
	  int res = Serial.read();
	  if(res == 0){break;}//if res = 0 end USER_COMMAEND ask in python what to do (will get into inByte)
	  digitalWrite(res, ON);// turn on the specific resistor
	  float user_volt= Serial.read(); // can be int 
	  int DAC_value = DAC_RES*(user_volt/VCC);
	  DAC.setVoltage(DAC_value, false);  //setVoltage(value, storeflag(saves val for later use)) 
      unsigned long timeLastWrite = millis();

      while(Serial.available() == 0) // i need to check if this will work
      {
          unsigned long currTime = millis(); // Grab the current time
          if(currTime - timeLastWrite >= reportInterval)
          {
            timeLastWrite = currTime;
            int adcVal = analogRead(analogVin);
            int dacVal = analogRead(DacMeasurement);
            Serial.println(res);
            Serial.println(dacVal); // the dac val 
            Serial.println(adcVal); 
            
          }  
      }
	}
	
  }
  else if(inByte == SWEEP)
  {
    
    for(int i = R1; i <= R5; i++)
    {
      
      digitalWrite(R1, OFF);
      digitalWrite(R2, OFF);
      digitalWrite(R3, OFF);
      digitalWrite(R4, OFF);
      digitalWrite(R5, OFF);

      digitalWrite(i, ON);// turn on the specific resistor
      
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
            int adcVal = analogRead(analogVin);
            int dacVal = analogRead(DacMeasurement);
            Serial.println(i); // what resistor is turned on R = 10^i
            Serial.println(dacVal); // the dac val 
            Serial.println(adcVal); 
            measurement++;
          }  
      }
     } 
    }//for(Res)
    Serial.println("Done");
  } //if(test)
 }//if (Serial.available() > 0)
}//void loop
