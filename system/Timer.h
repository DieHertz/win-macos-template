#ifndef Timer_h
#define Timer_h

#if defined(__WIN32__) || defined (__WIN64__)
#include "TimerWin.h"
#elif defined(__MACH__)
#include "TimerMac.h"
#else
#error "Operating System is not supported"
#include "TimerLinux.h"
#endif

#endif /* Timer_h */
