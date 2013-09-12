#ifndef WindowsGlWindowImpl_h
#define WindowsGlWindowImpl_h

#include "GlWindowImpl.h"
#include "Timer.h"

#include <windows.h>

class WindowsGlWindowImpl : public GlWindowImpl {
private:
	WindowsGlWindowImpl(const WindowsGlWindowImpl&) = delete;
	WindowsGlWindowImpl& operator =(const WindowsGlWindowImpl&) = delete;

	virtual LRESULT processEvent(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK windowCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void createOpenGlContext();
	void destroyOpenGlContext();

public:
	WindowsGlWindowImpl();
	~WindowsGlWindowImpl();

	virtual void init(WindowEventHandler* eventHandler, int width, int height,
				   	  const std::string& caption, bool fullScreen) override;
	virtual void show() override;
	virtual void hide() override;
	virtual void run() override;

private:
	WindowEventHandler* mEventHandler;
	
	bool mFullScreen;
	HWND mHWnd;
	HDC mHDC;
	HGLRC mHRC;
	bool mQuit;
	bool mActive;

	Timer mTimer;
};

#endif /* WindowsGlWindowImpl_h */
