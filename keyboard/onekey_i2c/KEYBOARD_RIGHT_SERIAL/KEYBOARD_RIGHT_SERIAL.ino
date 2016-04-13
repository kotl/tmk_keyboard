
#define HWSERIAL Serial1
#define DEBUG

#define LED_PIN 13


void setup()
{
  #ifdef DEBUG 
    Serial.begin(115200);
    delay(3000);
  #endif

  pinMode (LED_PIN, OUTPUT);
  digitalWrite (LED_PIN, HIGH);  
  HWSERIAL.begin(115200);
 
}

void loop()
{
  while(!HWSERIAL.available()) {
     delay(1);
  }
  Serial.print(HWSERIAL.read());
  Serial.println("");
}



