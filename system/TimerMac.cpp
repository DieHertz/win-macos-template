#include "TimerMac.h"
#include <sys/time.h>

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
	timeval now;
	if (gettimeofday(&now, NULL)) {
		return -1;
	}

	return now.tv_sec + now.tv_usec * 1.0e-6;
}

float Timer::elapsed() {
	return m_elapsed;
}
