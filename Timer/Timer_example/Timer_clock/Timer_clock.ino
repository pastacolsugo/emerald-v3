#include <Timer.h>
#define pin 13
Timer t1;
bool s;


void setup() {
  t1.init(1000);
  t1.ckInit(100,1);
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
    digitalWrite(pin, t1.Clock());  
}