#ifndef TimerMac_h
#define TimerMac_h

class Timer {
public:
	Timer();

	void reset();
	void update();

	double now();
	float elapsed();

private:
	float m_elapsed;
	double m_lastTime;
};
#endif /* TimerMac_h */
