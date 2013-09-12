#include "TimerWin.h"
#include <windows.h>

Timer::Timer()
: m_elapsed(0.0)
, m_lastTime(0.0) {
}

void Timer::reset() {
	m_elapsed = 0.0;
	m_lastTime = this->now();
}

void Timer::update() {
	double currentTime = this->now();
	m_elapsed = currentTime - m_lastTime;
	m_lastTime = currentTime;
}

double Timer::now() {
	LARGE_INTEGER timeVal;
	LARGE_INTEGER frequency;
	::QueryPerformanceCounter(&timeVal);
	::QueryPerformanceFrequency(&frequency);
	return static_cast<double>(timeVal.QuadPart) / frequency.QuadPart;
}

float Timer::elapsed() {
	return m_elapsed;
}
