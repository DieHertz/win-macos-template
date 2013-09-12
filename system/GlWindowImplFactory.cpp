#include "GlWindowImplFactory.h"

#if (defined (__WIN32__) || defined (__WIN64__)) && !defined(FORCE_GLUT)
#include "WindowsGlWindowImpl.h"
#else
#include "GlutGLWindowImpl.h"
#endif

GlWindowImpl* GlWindowImplFactory::getInstance() {
#if (defined (__WIN32__) || defined (__WIN64__)) && !defined(FORCE_GLUT)
	return new WindowsGlWindowImpl;
#else
	return new GlutGlWindowImpl;
#endif
}
