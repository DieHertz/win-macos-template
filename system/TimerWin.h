#ifndef TimerWin_h
#define TimerWin_h

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
#endif /* TimerWin_h */
