#include "GL/glew.h"
#include "system/GlWindow.h"
#include <iostream>

class EventHandler : public WindowEventHandler {
public:
	virtual void onCreate() override {
		std::cout << glGetString(GL_VERSION) << '\n';
	}

	virtual void onDraw() override {
		glClearColor(0.63f, 0.35f, 0.54f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}
};

int main(int argc, char** argv) {
	EventHandler eventHandler;
	GlWindow window(&eventHandler, 400, 400, "GlWindow");

	window.show();
	window.run();

	return 0;
}