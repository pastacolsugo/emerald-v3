//////pins///

#define lightPin 9
#define waterPin 10
#define auxPin 2

#define water1EnPin 7
#define waterSense1Pin 14
#define water2EnPin 8
#define waterSense2Pin 15

#define statLedPin 3


/////light parameters

#define defaultLightOnMinutes 0
#define defaultLightOnHours 0
#define defaultLightOffMinutes 0
#define defaultLightOffHours 18
#define EE_light_addr 00


/////water parameters

#define defaultWaterOnMinutes 0
#define defaultWaterOnHours 22
#define defaultWaterTime 1		//[s]
#define defaultWaterSafetyTime 18   //[s]
#define defaultWaterMaxMoisture 80.00
#define defaultWaterMinMoisture 20.00
#define EE_water_addr 10
#define defaultWaterSampleTime 1200 //[s]


/*
Serial commands list:
T,hh,mm,ss //sets RTC time at hh.mm.ss
L,hh[on],mm[on],hh[duration] //set light on hour and minutes+ duration time
W,state[0-1],hh[on],mm[on],ss[duration],  //water on time,minutes+ duration[s]

*/
