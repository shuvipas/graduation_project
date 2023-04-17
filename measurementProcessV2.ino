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
  Serial.setTimeout(1);
}

void loop() {
  while(Serial.available() <= 4){}

  double current[6] = {0.01, 0.001, 0.0001,0.00001,0.000001,0.0000001};
  for(int i=0; i<6; i++)
  {
    int resistor = resistor_finder(current[i]);// R = 1*10^resistor (ohm) 
    float v_dac = current[i]*resistor; 
    
  } 
  
}
