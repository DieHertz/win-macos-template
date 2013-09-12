#ifndef GlWindowImplFactory_h
#define GlWindowImplFactory_h

#include "GlWindowImpl.h"

class GlWindowImplFactory {
	GlWindowImplFactory() = delete;

public:
	static GlWindowImpl* getInstance();
};

#endif /* GlWindowImplFactory_h */
