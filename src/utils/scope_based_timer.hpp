/*
Scope Based Timer
inspiration: https://www.youtube.com/watch?v=YG4jexlSAjc
*/

#ifndef SCOPEBASEDTIMER_H
#define SCOPEBASEDTIMER_H

#include <iostream>
#include <vector> 
#include <chrono>
using namespace std;

enum class TimeUnit
{
	us,
	ms,
	s
};

struct timer_struct {
	int id;
	long long duration_us;
};

// A TimerLog class following the Singleton pattern
// because we only want one log for the whole program
class TimerLog
{
private:
	vector<timer_struct> mData;

public:
	static TimerLog& getInstance();
	/*{
		static TimerLog instance;
		return instance;
	}*/
	inline vector<timer_struct>& GetData() {return mData;}
private:
	inline TimerLog() {};
	TimerLog(TimerLog const&);
	void operator=(TimerLog const&);
};


// Timer class using Resource aquisition initialization
// starts recording at the time of calling and stop at the end of the scope
class Timer
{
public:
	Timer(bool logData = true);
	Timer(vector<timer_struct> &log_vec, bool logData = true);
	inline ~Timer() {stop();}

	long long getDuration(TimeUnit unit = TimeUnit::us);

private:
	chrono::time_point<chrono::high_resolution_clock> startTime, endTime;
	int id;
	bool LOGGING = true;
	timer_struct timer_record;
	vector<timer_struct> * log;

	void stop();

	int generateID();

	void logFn(int id, long long dur);
};

#endif