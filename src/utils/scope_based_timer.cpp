#include "scope_based_timer.hpp" 

TimerLog& TimerLog::getInstance()
{
		static TimerLog instance;
		return instance;
}

Timer::Timer(bool logData): LOGGING(logData)
{
	TimerLog& logger = TimerLog::getInstance();
	log = &logger.GetData();
	id = generateID();
	startTime = chrono::high_resolution_clock::now();
}

Timer::Timer(vector<timer_struct> &log_vec, bool logData): log(&log_vec), LOGGING(logData)
{
	id = generateID();
	startTime = chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	auto duration_us = getDuration();
	double duration_ms = duration_us * 0.001;
	
	cout << "ID: " << id << " | Duration: " << duration_us << "us (" << duration_ms << "ms)\n";
	if (LOGGING)
		logFn(id, duration_us);
}

long long Timer::getDuration(TimeUnit unit)
{
	endTime = chrono::high_resolution_clock::now();

	auto start = chrono::time_point_cast<chrono::microseconds>(startTime).time_since_epoch().count();
	auto end = chrono::time_point_cast<chrono::microseconds>(endTime).time_since_epoch().count();
	auto duration_us = end - start;

	switch (unit)
	{
	case TimeUnit::us:
		return duration_us;
		break;
	case TimeUnit::ms:
		return duration_us * static_cast<long long>(0.001);
		break;
	case TimeUnit::s:
		return duration_us * static_cast<long long>(0.000001);
		break;
	default:
		return duration_us;
		break;
	}
}

int Timer::generateID()
{
	static int count = 0;
	return count++;
}

void Timer::logFn(int id, long long dur)
{
	timer_record.id = id;
	timer_record.duration_us = dur;
	(*log).push_back(timer_record);
}