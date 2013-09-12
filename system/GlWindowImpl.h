#ifndef GlWindowImpl_h
#define GlWindowImpl_h

#include "WindowEventHandler.h"
#include <string>

class GlWindowImpl {
public:
	virtual void init(WindowEventHandler* eventHandler, int width, int height,
				   	  const std::string& caption, bool fullScreen) = 0;
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void run() = 0;
};

#endif /* GlWindowImpl_h */
