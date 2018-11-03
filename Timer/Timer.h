#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer
{
	public:
		Timer();
		void init(long scaling);//scaling sets the uS/mS range(1/1000)
		void Trg(long delay_time);
		void Rst();
		bool Out();
		void ckInit(long ckTime,int dt);
		bool Clock();
		void ckSet(long ckTime,int dt);

	private:
		long _scaling;
		bool interlock;
		long timeBuffer;
		bool out;

		long ckPeriod, onTime, offTime, ckBuffer_on, ckBuffer_off;
		bool ck_out;
};
#endif
