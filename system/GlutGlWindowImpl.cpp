#include "GlutGlWindowImpl.h"
#include "GL/freeglut.h"

static const int openGlVersionMajor = 2;
static const int openGlVersionMinor = 1;

WindowEventHandler* GlutGlWindowImpl::eventHandler = nullptr;
Timer GlutGlWindowImpl::mTimer;

GlutGlWindowImpl::GlutGlWindowImpl() {
}

void GlutGlWindowImpl::init(WindowEventHandler* eventHandler,
							   int width, int height,
						 	   const std::string& caption, bool fullScreen) {
	GlutGlWindowImpl::eventHandler = eventHandler;

	int argc = 0;
	char* argv = nullptr;

	glutInit(&argc, &argv);
	glutInitContextVersion(openGlVersionMajor, openGlVersionMinor);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow((caption + "-GLUT").c_str());

	if (fullScreen) {
		glutFullScreen();
	}

	glutIdleFunc(GlutGlWindowImpl::onUpdate);
	glutDisplayFunc(GlutGlWindowImpl::onDraw);
	glutReshapeFunc(GlutGlWindowImpl::onResize);
}

void GlutGlWindowImpl::show() {

}

void GlutGlWindowImpl::hide() {

}

void GlutGlWindowImpl::run() {
	eventHandler->onCreate();
	mTimer.reset();
	glutMainLoop();
	//	In most cases onDestroy will never be called
	eventHandler->onDestroy();
}

void GlutGlWindowImpl::onUpdate() {
	mTimer.update();
	eventHandler->onUpdate(mTimer.elapsed());
	glutPostRedisplay();
}

void GlutGlWindowImpl::onDraw() {
	eventHandler->onDraw();
	glutSwapBuffers();
}

void GlutGlWindowImpl::onResize(int width, int height) {
	eventHandler->onResize(width, height);
}