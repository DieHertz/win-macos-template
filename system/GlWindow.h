/*
 * GlWindow.h
 *
 *  Created on: 26.08.2012
 *      Author: Hertz
 */

#ifndef GlWindow_h
#define GlWindow_h

#include "GlWindowImpl.h"
#include <memory>

class GlWindow {
public:
	GlWindow(WindowEventHandler* eventHandler, int width, int height,
			 const std::string& caption = "", bool fullScreen = false);

	void show();
	void hide();
	void run();

private:
	std::unique_ptr<GlWindowImpl> mImpl;
};

#endif /* GlWindow_h */
