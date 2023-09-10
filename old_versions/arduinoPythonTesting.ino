int data;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
}

void loop() {
  while(Serial.available() <= 4){
  }
   data = Serial.readString().toInt();
   int sum = data + 1;
   Serial.println(sum); 
}
