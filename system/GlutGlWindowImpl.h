#ifndef GlutGlWindowImpl_h
#define GlutGlWindowImpl_h

#include "GlWindowImpl.h"
#include "Timer.h"

class GlutGlWindowImpl : public GlWindowImpl {
private:
	GlutGlWindowImpl(const GlutGlWindowImpl&) = delete;
	GlutGlWindowImpl& operator =(const GlutGlWindowImpl&) = delete;

	static void onUpdate();
	static void onDraw();
	static void onResize(int width, int height);

public:
	GlutGlWindowImpl();

	virtual void init(WindowEventHandler* eventHandler, int width, int height,
					  const std::string& caption, bool fullScreen) override;
	virtual void show() override;
	virtual void hide() override;
	virtual void run() override;

private:
	static WindowEventHandler* eventHandler;
	static Timer mTimer;
};

#endif /* GlutGlWindowImpl_h */
