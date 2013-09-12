#include "WindowsGlWindowImpl.h"
#include "GL/glew.h"
#include "GL/wglew.h"
#include <stdexcept>

static const char* windowClassName = "OpenGlWindow";
static const int openGlVersionMajor = 4;
static const int openGlVersionMinor = 3;

WindowsGlWindowImpl::WindowsGlWindowImpl()
: mEventHandler(nullptr)
, mFullScreen(false)
, mHWnd(nullptr)
, mHDC(nullptr)
, mHRC(nullptr)
, mQuit(false)
, mActive(false) {
}

WindowsGlWindowImpl::~WindowsGlWindowImpl() {
	if (mHRC) {
		this->destroyOpenGlContext();
	}
}

void WindowsGlWindowImpl::init(WindowEventHandler* eventHandler,
							   int width, int height,
						 	   const std::string& caption, bool fullScreen) {
	mEventHandler = eventHandler;
	mFullScreen = fullScreen;

	DWORD styleEx;
	DWORD style;
	RECT windowRect { 0, 0, width, height };

	if (!mEventHandler) {
		throw std::runtime_error("WindowEventHandler reference not set to an instance of an object");
	}

	WNDCLASS wc;
	wc.style        	= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc      = WindowsGlWindowImpl::windowCallback;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 0;
	wc.hInstance        = ::GetModuleHandle(nullptr);
	wc.hIcon        	= ::LoadIcon(nullptr, IDI_WINLOGO);
	wc.hCursor      	= ::LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground    = nullptr;
	wc.lpszMenuName     = nullptr;
	wc.lpszClassName    = windowClassName;

    if (!::RegisterClass(&wc)) {
    	throw std::runtime_error("Could not register window class");
    }

	if (mFullScreen) {
		DEVMODE screenSettings;
		screenSettings.dmSize = sizeof(screenSettings);
		screenSettings.dmPelsWidth	= width;
		screenSettings.dmPelsHeight	= height;
		screenSettings.dmBitsPerPel	= 32;
		screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (::ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			mFullScreen = false;
		}
	}

	if (mFullScreen) {
		styleEx = WS_EX_APPWINDOW;
		style = WS_POPUP;
		::ShowCursor(FALSE);
	} else {
		styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		style = WS_OVERLAPPEDWINDOW;
	}

	if (width != CW_USEDEFAULT and height != CW_USEDEFAULT) {
		::AdjustWindowRectEx(&windowRect, style, FALSE, styleEx);
	}

    mHWnd = CreateWindowEx(styleEx, windowClassName, (caption + "-WinAPI").c_str(),
		style, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, ::GetModuleHandle(nullptr), this);

    if (!mHWnd) {
    	throw std::runtime_error("Could not create window");
    }
}

void WindowsGlWindowImpl::show() {
	::ShowWindow(mHWnd, SW_SHOW);
	::UpdateWindow(mHWnd);
}

void WindowsGlWindowImpl::hide() {
	::ShowWindow(mHWnd, SW_HIDE);
	::UpdateWindow(mHWnd);
}

void WindowsGlWindowImpl::run() {
	MSG msg;

	//	Reset timer and start processing
	mTimer.reset();
	while (!mQuit) {
		/*	Currently window is peeking all messages before invoking
		 *	WindowEventHandler::onUpdate()
		 */
		while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				mQuit = true;
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		/*	If we're not currently quitting the app and window
		 *	is active - update and redraw it
		 */
		if (!mQuit && mActive) {
			mTimer.update();
			float elapsed = mTimer.elapsed();
			mEventHandler->onUpdate(elapsed);
			mEventHandler->onDraw();

			::SwapBuffers(mHDC);
		}
	}
}

LRESULT WindowsGlWindowImpl::processEvent(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		this->createOpenGlContext();
		glewExperimental = true;
		if (::glewInit() != GLEW_OK) {
			throw std::runtime_error("Could not initialize GLEW");
		}
		mEventHandler->onCreate();
		return 0;

	case WM_DESTROY:
		mEventHandler->onDestroy();
		/*	Return cursor and video mode back to normal
		*	if we were in full screen mode
		*/
		if (mFullScreen) {
			::ChangeDisplaySettings(nullptr, 0);
			::ShowCursor(TRUE);
		}
		::PostQuitMessage(0);
		return 0;

	case WM_ACTIVATE:
		mActive = !HIWORD(wParam);
		return 0;

	case WM_SIZE:
		mEventHandler->onResize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_MOUSEMOVE:
		mEventHandler->onMouseMove(lParam & 0xffff, (lParam >> 16) & 0xffff);
		return 0;

	case WM_MOUSEWHEEL:
		mEventHandler->onMouseWheel(static_cast<short>((wParam >> 16) & 0xffff) / WHEEL_DELTA);
		return 0;

	default:
		return DefWindowProc(mHWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK WindowsGlWindowImpl::windowCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	WindowsGlWindowImpl* window = nullptr;

	if (message == WM_NCCREATE) {
		window = reinterpret_cast<WindowsGlWindowImpl*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		::SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<long>(window));
		window->mHWnd = hWnd;
	}

	window = reinterpret_cast<WindowsGlWindowImpl*>(::GetWindowLong(hWnd, GWL_USERDATA));

	return window->processEvent(message, wParam, lParam);
}

void WindowsGlWindowImpl::createOpenGlContext() {
	mHDC = ::GetDC(mHWnd);

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,	//	RGBA_8888
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,	//	depth
		8,	//	stencil
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int format = ::ChoosePixelFormat(mHDC, &pfd);
	if (!::SetPixelFormat(mHDC, format, &pfd)) {
		throw std::runtime_error("Pixel format is available but could not be set");
	}

	//	Create dummy context in order to use wglGetProcAddress
	HGLRC tmpContext = ::wglCreateContext(mHDC);
	if (!::wglMakeCurrent(mHDC, tmpContext)) {
		throw std::runtime_error("Could not create temporary OpenGL context");
	}

	//	Now let's get necessary procs' addresses (context is required for using wglGetProcAddress)
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");

	//	Addresses cached, dummy context is not necessary anymore
	::wglMakeCurrent(nullptr, nullptr);
	::wglDeleteContext(tmpContext);

	if (!wglCreateContextAttribsARB) {
		throw std::runtime_error("Can't create OpenGL context, wglCreateContextAttribsARB is not available");
	}

	//	We're good to go, let's set appropriate pixel format
	int pixelAttributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};
	int pixelFormat;
	unsigned int numFormats;
	wglChoosePixelFormatARB(mHDC, pixelAttributes, nullptr, 1, &pixelFormat, &numFormats);
	if (numFormats <1) {
		throw std::runtime_error("Desired pixel format is not present");
	}

	if (!::SetPixelFormat(mHDC, pixelFormat, nullptr)) {
		throw std::runtime_error("Pixel format is available but could not be set");
	}

	//	Now create desired OpenGL context
	int contextAttributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, openGlVersionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, openGlVersionMinor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,/*WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,*/
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
	mHRC = wglCreateContextAttribsARB(mHDC, nullptr, contextAttributes);
	if (!mHRC) {
		throw std::runtime_error("Could not create OpenGL context with specified attributes");
	}

	if (!::wglMakeCurrent(mHDC, mHRC)) {
		throw std::runtime_error("Could not make context current");
	}
}

void WindowsGlWindowImpl::destroyOpenGlContext() {
	::wglMakeCurrent(nullptr, nullptr);
	::wglDeleteContext(mHRC);
	::ReleaseDC(mHWnd, mHDC);
}
