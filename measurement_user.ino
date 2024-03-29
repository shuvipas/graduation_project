#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#define ON HIGH
#define OFF LOW
#define END_PROGRAM "Done" 

Adafruit_MCP4725 DAC;

const int InAmpVout = A0;
const int DacVout = A1;
const int VRs = A2;
const int measurement_times = 5;
const int DAC_res = 4096;
const long reportInterval = 200; // How often to write the result to serial in milliseconds
const int READ_DIFF = 100;

const int HANDSHAKE = 0;
const int SWEEP = 1;
const int USER_INPUT = 2;

const int R2 = 2;// 100 ohm
const int R3 = 3; // 1 K*ohm
const int R4 = 4; // 10 K*ohm
const int R5 = 5; // 100 K*ohm
const int R6 = 6; // 1 M*ohm


/**
 * Perform voltage measurements and transmit results to the python script.
 * @param res The resistor being used for measurements.
 */
void measurements(int res){
    int meas_num = 0;
    unsigned long time_last_write = millis();
    
    while(meas_num < measurement_times){ 
        unsigned long currTime = millis(); // Grab the current time
        if(currTime - time_last_write >= reportInterval){
            time_last_write = currTime;
            int adcVal = analogRead(InAmpVout);
            int dacVal = analogRead(DacVout);
            int Volt_Rs = analogRead(VRs);
            while(!Serial.availableForWrite()){}
            Serial.println(res); // what resistor is turned on R = 10^i
            Serial.println(dacVal); // the DAC val
            Serial.println(Volt_Rs);
            Serial.println(adcVal);
            meas_num++;
        }
    }
}
/**
 * Switch the specified resistor on and turning off the others.
 * @param res_on The resistor to turn on.
 */
void resistors_switching(int res_on){
    digitalWrite(R2, OFF);
    digitalWrite(R3, OFF);
    digitalWrite(R4, OFF);
    digitalWrite(R5, OFF);
    digitalWrite(R6, OFF);
              
    digitalWrite(res_on, ON);// turn on the specific resistor
}

/**
 * Handle user input from the python script for resistor selection and DAC voltage values.
 */
void user_input(){
    while (!Serial.available()){}
    int res = Serial.read();
    resistors_switching(res);
    while (Serial.available() < 4){}
    byte data[4];
    Serial.readBytes(data, 4);  // Read the 4 bytes into the data array
    int dac_vin = *((int*)data);  // Convert the byte array back to an integer
   
    
    while (!Serial.available()){}
    int read_num = Serial.read();          
    for (int i = 0; i < read_num; i++){
        dac_vin += i * READ_DIFF;
        DAC.setVoltage(dac_vin, false);  //setVoltage(value, store flag(saves val for later use))
        measurements(res);    
    } 
    Serial.println(END_PROGRAM);
}

/**
 * General sweep across predefined current values.
 */
void sweep(){
    for(int i = R2; i <= R6; i++){
        resistors_switching(i);        
        int DAC_value[] = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000,
                            2200, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 3800, 4000};
        int arr_len = sizeof(DAC_value)/sizeof(DAC_value[0]);  
        for(int j = 0; j < arr_len; j++){
            DAC.setVoltage(DAC_value[j], false);  //setVoltage(value, storeflag(saves val for later use))
            measurements(i);
        } 
    }
    Serial.println(END_PROGRAM);
}


void setup() {
    Serial.begin(115200);
    DAC.begin(0x60); // Default I2C Address of MCP4725
    Serial.setTimeout(5000); //maximum milliseconds to wait for serial data.
    pinMode(R2, OUTPUT);
    pinMode(R3, OUTPUT);
    pinMode(R4, OUTPUT);
    pinMode(R5, OUTPUT);
    pinMode(R6, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        int in_byte = Serial.read(); // user command
        switch (in_byte){
            // validate communication with python
            case HANDSHAKE:   
                while(!Serial.availableForWrite()){}
                Serial.println("Lets start!");
                break;
            case USER_INPUT:
                user_input();
                break;
            case SWEEP:
                sweep();
                break;
            default:
                break;
        }
     }
}
