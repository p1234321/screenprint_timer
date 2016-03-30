/* Count Up/Down Timer */

/*
 The MIT License (MIT)
 Copyright (c) 2016 Andrew Mascolo Jr
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

/*
 Helpful Corrections by:
	Github user Mannelito - Correction to ResumeTimer function
    modified features by p1234321
*/

#ifndef CountUpDownTimer_h
#define CountUpDownTimer_h

#include<Arduino.h>
#define UP 1
#define DOWN 0

enum {CD_STOP, CD_RUNNING, CD_PAUSED, CD_FINISHED};
int event;

class CountUpDownTimer
{
  public:
    CountUpDownTimer(bool type) : _type(type)
	{ 
	  SetStopTime(0xFFFF); // 18h 12m 15s
	  time = micros();
	  Clock = 0;
	  Reset = false, Stop = true, Paused = true;
	  timeFlag = false;
	  duration = 1000000;
	}
	
	boolean Timer()
	{
	  timeFlag = false;
	  if (!Stop && !Paused) // if not Stopped or Paused, run timer
	  {
	    if(Paused)
	      time = micros();
		
		if ((_micro = micros()) - time > duration ) // check the time difference and see if 1 second has elapsed
		{
		  _type == UP? Clock++ : Clock--;
			
		  timeFlag = true;

		  if ((_type == DOWN && Clock == 0) || TimeCheck(STh, STm, STs)) // check to see if the clock is 0
          {
			Stop = true; // If so, stop the timer
            Finished = true;
          }
			
		  time = _micro;
		  //Start= false;
		  if(_micro < time) 
		    time = 0;  // check to see if micros() has rolled over, if not, then increment "time" by duration
		}
	  }
		
	  return !Stop; // return the state of the timer
	}

	void ResetTimer()
	{
	  if(_type==UP)
	    Clock = 0;
	  else
	    SetTimer(R_clock);
	  Stop = false;
      Finished = false;
      Paused = true;
	}

	void StartTimer()
	{
	  Watch = micros();
	  Stop = false;
	  Paused = false;
      Finished = false;
	  if(_type == UP) 
	    Clock = 0;
	}

	void StopTimer()
	{
	  Stop = true;
	}

	void StopTimerAt(unsigned long hours, unsigned long minutes, unsigned long seconds)
	{
	  if (TimeCheck(hours, minutes, seconds) )
		Stop = true;
	}

	void PauseTimer()
	{
	  time = micros();
	  Paused = true;
	}
    
    int StateTimer()
    {
        if (Finished==true) event = CD_FINISHED;
        if (Stop && !Finished) event=CD_STOP;
        if (Paused && !Stop) event=CD_PAUSED;
        //event=CD_PAUSED;
        if (!Stop && !Paused && !Finished) event=CD_RUNNING;
        return event;
    }

	void ResumeTimer() // You can resume the timer if you ever stop it.
	{
	  Paused = false;
	  time = micros();
	}
    
    boolean isitpaused ()
    {
        return Paused;
    }
    boolean isitfinished ()
    {
        return Finished;
    }
    boolean isitstopped ()
    {
        return Stop;
    }
    boolean isitresetted ()
    {
        return Reset;
    }

	void SetTimer(unsigned long hours, unsigned long minutes, unsigned long seconds)
	{
	  // This handles invalid time overflow ie 1(H), 0(M), 120(S) -> 1h, 2m, 0s
	  unsigned int _S = (seconds / 60), _M = (minutes / 60);
	  if(_S) minutes += _S;
	  if(_M) hours += _M;
	  
	  Clock = (hours * 3600) + (minutes * 60) + (seconds % 60);
	  R_clock = Clock;
	  Stop = false;
      Finished = false;
      Paused = true;
	}

	void SetTimer(unsigned long seconds)
	{
	 // StartTimer(seconds / 3600, (seconds / 3600) / 60, seconds % 60);
	  Clock = seconds;
	  R_clock = Clock;
	  Stop = false;
      Finished = false;
      Paused = true;
	}
	
	void SetStopTime(unsigned long seconds)
	{
	  STh = seconds / 3600;
	  STm = (seconds / 60) % 60;
	  STs = seconds % 60;
	}
	
	void SetStopTime(unsigned long hours, unsigned long minutes, unsigned long seconds)
	{
      STh = hours;
	  STm = minutes;
	  STs = seconds;
	}
    
    unsigned long ShowClock()
    {
      return Clock;
    }

	unsigned long ShowHours()
	{
	  return Clock / 3600;
	}

	unsigned long ShowMinutes()
	{
	  return (Clock / 60) % 60;
	}

	unsigned long ShowSeconds()
	{
	  return Clock % 60;
	}

	unsigned long ShowMilliSeconds()
	{
	  return ((_micro - Watch)/ 1000.0) + 1;
	}

	unsigned long ShowMicroSeconds()
	{
	  return (_micro - Watch) + 1;
	}

	boolean TimeHasChanged()
	{
	  return timeFlag;
	}

	boolean TimeCheck(unsigned int hours, unsigned int minutes, unsigned int seconds) // output true if timer equals requested time
	{
	  return (hours == ShowHours() && minutes == ShowMinutes() && seconds == ShowSeconds());
	}
	

    private:
	    unsigned long duration;
	    unsigned long STh, STm, STs;
		unsigned long Watch, _micro, time;
		unsigned long Clock, R_clock;
		boolean Reset, Stop, Paused, Finished, Start;
		volatile boolean timeFlag;
		bool _type;
};

#endif