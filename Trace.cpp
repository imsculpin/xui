#include "stdafx.h"
#include "Trace.h"

_XTRACEAPI TRACETYPE g_traceType = _XTRACEAPI _INFO;
#ifdef _TRACEMORE
int g_traceLine = 0;
LPCTSTR g_traceFunction = NULL;
LPCTSTR g_tracefilename = NULL;
#endif
CThisTrace g_trace;