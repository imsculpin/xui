#ifndef __TRACE_DEFINE_H__
#define __TRACE_DEFINE_H__
#if !defined(UNICODE) && !defined( _UNICODE )
#define __T_FILE__  __FILE__
#define __T_FUNCTION__ __FUNCTION__
#else
#define __T_FILE__ __FILEW__
#define __T_FUNCTION__ __FUNCTIONW__
#endif

#ifdef _XTRACEAPI
#undef _XTRACEAPI
#endif 

//#define _XTRACE_LIB
#ifdef _XTRACE_LIB
#include "XTrace.h"
#pragma comment( lib, "..\\lib\\xtrace.lib" )
#define _XTRACEAPI XTRACE::
#else
#define _XTRACEAPI 
#endif
#ifndef _XTRACE_LIB
enum TRACETO{_NULL, _CONSOLE = 1025,_FILE,_OUTDGB,};//{};
enum TRACETYPE{_WARNING, _ERROR, _INFO,};
EXTERN_C typedef void (*InitXTrace)( TRACETO traceTo, LPCTSTR pszTraceTo );
EXTERN_C typedef void (*UninitXTrace)(  );
EXTERN_C typedef void (*XTrace)( TRACETYPE _type, LPCTSTR _format, ... );
EXTERN_C typedef void (*XTraceEx)( TRACETYPE _type, LPCTSTR _File, LPCTSTR _Function, int _line, LPCTSTR _format, ... );
EXTERN_C typedef void (*XVTrace)( TRACETYPE _type, LPCTSTR _format, va_list list );
EXTERN_C typedef void (*XVTraceEx)( TRACETYPE _type, LPCTSTR _File, LPCTSTR _Function, int _line, LPCTSTR _format, va_list list );
#endif

extern _XTRACEAPI TRACETYPE g_traceType;
#define _TRACEMORE
#ifdef _TRACEMORE
extern int g_traceLine;
extern LPCTSTR g_traceFunction;
extern LPCTSTR g_tracefilename;
#endif
#define OUTDB
class CThisTrace
{
public:
	CThisTrace( _XTRACEAPI TRACETO traceTo = 
#ifdef _DEBUG
#ifndef  OUTDB
		_XTRACEAPI _CONSOLE
#else   
		_XTRACEAPI _OUTDGB
#endif
		, LPCTSTR pszTraceTo = NULL )
#else 
		_XTRACEAPI _FILE, LPCTSTR pszTraceTo = _T(".\\Trace.txt") )
#endif 
#ifndef _XTRACE_LIB
		:m_InitXTrace(NULL)
		,m_hLogLib(NULL)
		,m_XVTrace(NULL)
		,m_UninitXTrace(NULL)
		,m_XVTraceEx(NULL)
#endif
	{
#ifndef _XTRACE_LIB
		m_hLogLib = LoadLibrary( _T("XTrace.dll") );
		if ( m_hLogLib )
		{
			m_InitXTrace = (InitXTrace)GetProcAddress( m_hLogLib, _T("InitXTrace") );
			m_UninitXTrace = (UninitXTrace)GetProcAddress( m_hLogLib, _T("UninitXTrace") );
			m_XVTrace = (XVTrace)GetProcAddress( m_hLogLib, _T("XVTrace") );
			m_XVTraceEx = (XVTraceEx)GetProcAddress( m_hLogLib, _T("XVTraceEx") );
		}
		if ( m_InitXTrace )
		{
			(*m_InitXTrace)( traceTo, pszTraceTo );
		}
#else
		_XTRACEAPI InitXTrace( traceTo, pszTraceTo );
#endif
	}
	~CThisTrace(  )
	{
#ifndef _XTRACE_LIB
		if ( m_UninitXTrace )
		{
			(*m_UninitXTrace)();
		}
		if ( m_hLogLib )
		{
			FreeLibrary( m_hLogLib );
			CloseHandle( m_hLogLib );
		}
#else
		_XTRACEAPI UninitXTrace();
#endif
	}

	void Trace( LPCTSTR _format, ... )
	{
		va_list list;
		va_start( list, _format );
		
#ifdef _TRACEMORE
#ifndef  _XTRACE_LIB
		if ( m_XVTraceEx )
		{
			(*m_XVTraceEx)( g_traceType, g_tracefilename, g_traceFunction, g_traceLine, _format, list );
		}
#else
		_XTRACEAPI XVTraceEx( g_traceType, g_tracefilename, g_traceFunction, g_traceLine, _format, list );
#endif
#else
#ifndef  _XTRACE_LIB
		if ( m_XVTrace )
		{
			(*m_XVTrace)( g_traceType, _format, list );
		}
#else
		_XTRACEAPI XVTrace( g_traceType, _format, list );
#endif
#endif
		va_end( list );
	}
private:
#ifndef _XTRACE_LIB
	HMODULE m_hLogLib;
	InitXTrace m_InitXTrace;
	UninitXTrace m_UninitXTrace;
	XVTrace		 m_XVTrace;
	XVTraceEx    m_XVTraceEx;
#endif
	
};
extern CThisTrace g_trace;

#ifdef INFO
#undef INFO
#endif 
#ifdef ERROR
#undef ERROR
#endif 
#ifdef WARNING
#undef WARNING
#endif 
#ifndef _TRACEMORE
#define INFO g_traceType = _XTRACEAPI _INFO;g_trace.Trace
#define WARNING g_traceType = _XTRACEAPI _WARNING;g_trace.Trace
#define ERROR g_traceType = _XTRACEAPI _ERROR;g_trace.Trace
#else
#define WARNING g_traceType = _XTRACEAPI _WARNING;g_tracefilename = __T_FILE__;g_traceLine=__LINE__;g_traceFunction=__T_FUNCTION__;g_trace.Trace 
#define ERROR g_traceType = _XTRACEAPI _ERROR;g_tracefilename = __T_FILE__;g_traceLine=__LINE__;g_traceFunction=__T_FUNCTION__;g_trace.Trace
#define INFO g_traceType = _XTRACEAPI _INFO;g_tracefilename = __T_FILE__;g_traceLine=__LINE__;g_traceFunction=__T_FUNCTION__;g_trace.Trace
#endif

#endif