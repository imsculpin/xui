#include <wtypes.h>
#include <assert.h>
#include <windows.h>
#include <process.h>
#include "Aplication.h"
#ifdef _DEBUG
#include <crtdbg.h>
#endif
CAplication* g_Application = NULL;
CAplication* GLGetApp()
{
	return g_Application;
}

CAplication::~CAplication()
{
	if ( m_pszCommandLine )
	{
		free(m_pszCommandLine); 
		m_pszCommandLine = NULL;
		m_nShowCmd = 0;
	}
}

CAplication::CAplication()
	:m_pszCommandLine(NULL)
	,m_nShowCmd( 0 )
	,m_hInstance( NULL )
{
	assert( NULL == g_Application );
	g_Application = this;
	m_hThread	= ::GetCurrentThread();
	m_nThreadID = ::GetCurrentThreadId();
}


