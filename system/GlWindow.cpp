#include "GlWindow.h"
#include "GlWindowImplFactory.h"

GlWindow::GlWindow(WindowEventHandler* eventHandler, int width, int height,
				   const std::string& caption, bool fullScreen)
: mImpl(GlWindowImplFactory::getInstance()) {
	mImpl->init(eventHandler, width, height, caption, fullScreen);
}

void GlWindow::show() {
	mImpl->show();
}

void GlWindow::hide() {
	mImpl->hide();
}

void GlWindow::run() {
	mImpl->run();
}