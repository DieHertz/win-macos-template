#ifndef WindowEventHandler_h
#define WindowEventHandler_h

class WindowEventHandler
{
public:
	virtual ~WindowEventHandler() {}

	virtual void onCreate() {}
	virtual void onDestroy() {}
	virtual void onUpdate(float elapsed) {}
	virtual void onDraw() {}
	virtual void onResize(int width, int height) {}
	
	virtual void onMouseDown(...) {}
	virtual void onMouseUp(...) {}
	virtual void onMouseMove(int x, int y) {}
	virtual void onMouseWheel(int steps) {}
	virtual void onKeyDown(...) {}
	virtual void onKeyUp(...) {}
};
#endif /* WindowEventHandler_h */
