

#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define ON HIGH
#define OFF LOW
#define DONE "Done" 

Adafruit_MCP4725 DAC;
const int InAmpVout = A0;
const int DacVout = A1;
const int measurement_times = 2;
const int DAC_res = 4096;
const long reportInterval = 200; // How often to write the result to serial in milliseconds
const int readDiff = 100;
const int HANDSHAKE = 0;
const int SWEEP = 1;
const int USER_INPUT = 2;

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
        int inByte = Serial.read(); // user command

        if (inByte == HANDSHAKE)
        {
            if (Serial.availableForWrite())
            {
                Serial.println("Handshake received.");
            }
        }
     
        else if(inByte == USER_INPUT)
        {   
            Serial.println(inByte);
            while (!Serial.available()){
            }
            int res = Serial.read();
            while(!Serial.availableForWrite()){
            }
            Serial.println(res);
            while (!Serial.available()){                   
            }
            int DacVIn = Serial.readStringUntil('\n').toInt(); //read();
            while(!Serial.availableForWrite()){
            }
            Serial.println(DacVIn);
            while (!Serial.available()){                   
            }
            int readNum = Serial.read();
            while(!Serial.availableForWrite()){
            }
            Serial.println(readNum);
            digitalWrite(res, ON);
            
            for (int i = 0; i < readNum; i++)
            {
              while(!Serial.availableForWrite()){
              }
                Serial.println(i);
              
                DacVIn += i * readDiff;
                DAC.setVoltage(DacVIn, false);  //setVoltage(value, store flag(saves val for later use))
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
                        while(!Serial.availableForWrite()){
                   }
                        Serial.println(res); // what resistor is turned on R = 10^i
                        while(!Serial.availableForWrite()){
                   }
                        Serial.println(dacVal); // the dac val
                        while(!Serial.availableForWrite()){
                   }
                        Serial.println(adcVal);
                        measurement++;
                    }
                } //while(measurement < measurement_times)
            } // for(readNum)
            while(!Serial.availableForWrite()){
                   }
            Serial.println(DONE);
        }//if(inByte == USER_INPUT)

        else if(inByte == SWEEP)
        {
            for(int i = R2; i <= R6; i++){
                
                digitalWrite(R2, OFF);
                digitalWrite(R3, OFF);
                digitalWrite(R4, OFF);
                digitalWrite(R5, OFF);
                digitalWrite(R6, OFF);
              
                digitalWrite(i, ON);// turn on the specific resistor
        
                int DAC_value[20] = {200, 400, 600, 800, 1000,
                                     1200, 1400, 1600, 1800, 2000,
                                     2200, 2400, 2600, 2800, 3000,
                                     3200, 3400, 3600, 3800, 4000};  
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
            Serial.println(DONE);
        } //if(inByte == SWEEP)
    }//if (Serial.available() > 0)
}//void loop
