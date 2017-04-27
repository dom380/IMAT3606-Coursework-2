#include <utils\Timer.h>

Timer::Timer() 
{
#if defined(_WIN32) || defined(_WIN64)
	QueryPerformanceFrequency(&frequency);
	startCount.QuadPart = 0;
	endCount.QuadPart = 0;
#else
	startCount.tv_sec = startCount.tv_usec = 0;
	endCount.tv_sec = endCount.tv_usec = 0;
#endif

	stopped = true;
	startTimeInMicroSec = 0;
	endTimeInMicroSec = 0;
}

void Timer::start()
{
	stopped = 0; // reset stop flag
#if defined(_WIN32) || defined(_WIN64)
	QueryPerformanceCounter(&startCount);
#else
	gettimeofday(&startCount, NULL);
#endif
}



void Timer::stop()
{
	stopped = true;

#if defined(_WIN32) || defined(_WIN64)
	QueryPerformanceCounter(&endCount);
#else
	gettimeofday(&endCount, NULL);
#endif
}

bool Timer::isRunning()
{
	return !stopped;
}

double Timer::getElapsedTimeMicroSec() 
{
#if defined(_WIN32) || defined(_WIN64)
	if (!stopped)
		QueryPerformanceCounter(&endCount);

	startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
	endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
	if (!stopped)
		gettimeofday(&endCount, NULL);

	startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
	endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

	return endTimeInMicroSec - startTimeInMicroSec;
}

double Timer::getElapsedTime()
{
	return this->getElapsedTimeMicroSec() * 0.000001;
}

double Timer::getElapsedTimeMilliSec()
{
	return this->getElapsedTimeMicroSec() * 0.001;
}
