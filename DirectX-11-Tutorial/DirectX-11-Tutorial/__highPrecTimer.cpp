#include "__highPrecTimer.h"

HighPrecisionTimer::HighPrecisionTimer()
{
}

HighPrecisionTimer::HighPrecisionTimer(const HighPrecisionTimer& other)
{
}

HighPrecisionTimer::~HighPrecisionTimer()
{
}

// The Initialize function will first query the system to see if it supports high frequency timers.
// If it returns a frequency then we use that value to determine how many counter ticks will occur each millisecond.
// We can then use that value each frame to calculate the frame time.
// At the end of the Initialize function we query for the start time of this frame to start the timing.
bool HighPrecisionTimer::Initialize(float interval)
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*) &m_frequency);
	if (m_frequency == 0)
		return false;

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	// Запомним интервал, по которому должен срабатывать наш таймер
	if (interval > 0.0) {
		m_Interval = interval * m_ticksPerMs;
		m_TimePassed = 0.0f;
	}
	else
		return false;

	QueryPerformanceCounter((LARGE_INTEGER*) &m_startTime);

	return true;
}

// The Frame function is called for every single loop of execution by the main program.
// This way we can calculate the difference of time between loops and determine the time it took to execute this frame.
// We query, calculate, and then store the time for this frame into m_frameTime so that it can be used by any calling object for synchronization.
// We then store the current time as the start of the next frame.

// Кроме прочего, возвращаем true, если прошел очередной интервал, заданный параметром m_Interval, сигнализируя о наступлении события onTimer
bool HighPrecisionTimer::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);
	timeDifference = (float)(currentTime - m_startTime);
	m_frameTime	   = timeDifference / m_ticksPerMs;
	m_startTime	   = currentTime;

	m_TimePassed += timeDifference;

	if( m_TimePassed >= m_Interval ) {
		m_TimePassed = 0.0f;
		return true;
	}

	return false;
}

// GetTime returns the most recent frame time that was calculated.
float HighPrecisionTimer::GetTime()
{
	return m_frameTime;
}
