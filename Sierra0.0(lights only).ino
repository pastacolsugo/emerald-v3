//Sierra0.0(lights only)
#include <Timer.h>//custom lib micros()-based
Timer t1;

#include <DS3231.h>
DS3231 rtc(A4, A5);
Time t;

#include <EEPROM.h>//using eeprom to save value and states

#define dw digitalWrite
#define dr digitalRead
#define ar analogRead

#define statLedPin 3 //blinking led indicating loop running
#define fanLowPower 90
#define fanHighPower 255

#define lightPin 9
#define EE_light_addr 00 //available 00 to 09
int l_OnH= 18;
int l_OnM= 00;
int l_OffH= 10;
int l_OffM= 00;

void setup()
{
	Serial.begin(9600);
	Serial.print("boot up");
	rtc.begin();
	t1.init(1000);
	t1.ckSet(1000,20);

	pinMode(lightPin,OUTPUT);
	pinMode(statLedPin,OUTPUT);
	
	dw(lightPin, EEPROM.read( EE_light_addr));

}

void loop()
{
	t = rtc.getTime();

	dw(statLedPin, t1.Clock());
	lights_Core(l_OnH,l_OnM,l_OffH,l_OffM);

}

void lights_Core(int OnHour,
				 int OnMinute,
				 int OffHour, 
				 int OffMinute){
    if (t.min == OnMinute & t.hour == OnHour) 
	  {
     	dw(lightPin,1);
     	EEPROM.write(EE_light_addr,1);
      }
  
    if (t.min == OffMinute & t.hour == OffHour) 
      {
     	dw(lightPin,0); 
     	EEPROM.write(EE_light_addr,0);
       }

}