#include "Timer.h"
#include "Arduino.h"

Timer::Timer()
{

}
void Timer::init(long scaling)
{
	_scaling= scaling;
	interlock= false;
}


void Timer::Trg(long delay_time)
{
	if(!interlock)
	{
		timeBuffer= micros()+ delay_time* _scaling;
		interlock =true;
	}

}

bool Timer::Out()
{
	if(micros() >= timeBuffer ){
		interlock= false;
		out= true;
	}

	else{
		out= false;
		interlock= true;
	}
	return out;
	
}

void Timer::Rst()
{
	interlock= true;
	out= false;
}

void Timer::ckInit(long ckTime,int dt){
	ckPeriod= ckTime* _scaling;
	onTime= (ckPeriod* dt)/ 100;
	offTime= ckPeriod- onTime;
	ckBuffer_on= micros()+ onTime;
	ck_out=true;
	// ckBuffer_off= micros()+ offTime;
}

bool Timer::Clock(){
	if((micros() >= ckBuffer_on)&& ck_out== true){

		ck_out = !ck_out;
		ckBuffer_off= micros()+ offTime;
	}

	if((micros()>= ckBuffer_off)&& ck_out== false){

		ck_out= true;
		ckBuffer_on= micros()+ onTime;
	}
	return ck_out;
	
}

void Timer::ckSet(long ckTime,int dt){
	ckPeriod= ckTime* _scaling;
	onTime= (ckPeriod* dt)/ 100;
	offTime= ckPeriod- onTime;
}