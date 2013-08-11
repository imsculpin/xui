#include <wtypes.h>
#include <assert.h>
#include <process.h>
#include "uithread.h"
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#ifndef WM_KICKIDLE
#define WM_KICKIDLE         0x036A
#endif

UINT WINAPI _G_ThreadEntry(void* pParam)
{
	CThread* pThread = (CThread*)pParam;
	assert(pThread != NULL);

	// first -- check for simple worker thread
	DWORD nResult = 0;
	if (pThread->m_pfnThreadProc != NULL)
	{
		nResult = (*pThread->m_pfnThreadProc)(pThread->m_pThreadParams);
		assert(pThread);
	}
	// else -- check for thread with message loop
	else if (!pThread->InitInstance())
	{
		assert(pThread);
		nResult = pThread->ExitInstance();
	}
	else
	{
		// will stop after PostQuitMessage called
		assert(pThread);
		nResult = pThread->Run();
	}

	// cleanup and shutdown the thread
	_endthreadex( 0 );
	return 0;   // not reached
}

BOOL CThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize,
						   LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
#ifndef _MT
	dwCreateFlags;
	nStackSize;
	lpSecurityAttrs;

	return FALSE;
#else
	assert(m_hThread == NULL);  // already created?
	// create the thread (it may or may not start to run)
	m_hThread = (HANDLE)(ULONG_PTR)_beginthreadex(lpSecurityAttrs, nStackSize,  //REVIEW
		&_G_ThreadEntry, (void*)this, dwCreateFlags, (UINT*)&m_nThreadID);
	if (m_hThread == NULL)
	{
		return FALSE;
	}

	return TRUE;
#endif //!_MT
}

void CThread::Delete()
{
	// delete thread if it is auto-deleting
	if (m_bAutoDelete)
	{
		delete this;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CThread default implementation

BOOL CThread::InitInstance()
{
	assert(this);
	return FALSE;   // by default don't enter run loop
}

// main running routine until thread exits
int CThread::Run()
{
	assert(this);
	//for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	MSG msg = {0};
	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if (!::GetMessage( m_pMsgCur, NULL, NULL, NULL))
			{
				// Note: prevents calling message loop things in 'ExitInstance'
				// will never be decremented
				return ExitInstance();
			}
			if ( !PreTranslateMessage( m_pMsgCur ) )
			{
				// process this message
				if ( m_pMsgCur->message != WM_KICKIDLE )
				{
					if ( NULL == m_hAccel
						||( NULL != m_hAccel
						&& !::TranslateAccelerator( m_pMsgCur->hwnd, m_hAccel, m_pMsgCur )
						))
					{
					}
					::TranslateMessage(m_pMsgCur);
					::DispatchMessage(m_pMsgCur);
				}
				// reset "no idle" state after pumping "normal" message
				//if (IsIdleMessage(&m_msgCur))
				if (IsIdleMessage(&msg))
				{
					bIdle = TRUE;
					lIdleCount = 0;
				}
			}
		} while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
	}
	return 0;
}

BOOL CThread::IsIdleMessage(MSG* pMsg)
{
	// redundant WM_MOUSEMOVE and WM_NCMOUSEMOVE
	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE)
	{
		// mouse move at same position as last mouse move?
		if (m_pMsgLast->pt.x == pMsg->pt.x
			&&m_pMsgLast->pt.y == pMsg->pt.y
			&& pMsg->message == m_pMsgCur->message )
		{
			return FALSE;
		}
		*m_pMsgLast = *pMsg;
		return TRUE;
	}
	// WM_PAINT and WM_SYSTIMER (caret blink)
	return pMsg->message != WM_PAINT && pMsg->message != 0x0118;
}

int CThread::ExitInstance()
{
	return (int)m_pMsgCur->wParam;
}

BOOL CThread::OnIdle(LONG lCount)
{
	assert(this);

#if defined(_DEBUG)
	// check MFC's allocator (before idle)
	if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
	{
		assert(_CrtCheckMemory());
	}
#endif

	if (lCount <= 0)
	{
		// send WM_IDLEUPDATECMDUI to the main window
		// 		HWND hMainWnd = m_hMainWnd;
		//  		if (hMainWnd!= NULL &&
		//  			IsWindowVisible( hMainWnd ))
		//  		{
		//  			AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd,
		//  				WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
		//  			pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
		//  				(WPARAM)TRUE, 0, TRUE, TRUE);
		//  		}
	}
#if defined(_DEBUG) 
	// check allocator (after idle)
	if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
	{
		assert(_CrtCheckMemory());
	}
#endif
	return lCount < 0;  // nothing more to do if lCount >= 0
}

CThread::CThread()
:m_hMainWnd(NULL)
,m_hAccel(NULL)
,m_hThread( NULL )
,m_nThreadID(0)
,m_bAutoDelete(TRUE)
,m_pThreadParams(NULL)
,m_pfnThreadProc(NULL)
{
	try
	{
		m_pMsgLast = new MSG;
		memset( m_pMsgLast, 0, sizeof(MSG) );
		m_pMsgCur = new MSG;
		memset( m_pMsgCur, 0, sizeof(MSG) );
	}
	catch( ... )
	{
		throw "no more memory!";
	}
}

CThread::CThread( THREADPROC pfnThreadProc, LPVOID pParam)
:m_hMainWnd(NULL)
,m_hAccel(NULL)
,m_hThread( NULL )
,m_nThreadID(0)
,m_bAutoDelete(TRUE)
,m_pThreadParams(pParam)
,m_pfnThreadProc(pfnThreadProc)
{
	try
	{
		m_pMsgLast = new MSG;
		memset( m_pMsgLast, 0, sizeof(MSG) );
		m_pMsgCur = new MSG;
		memset( m_pMsgCur, 0, sizeof(MSG) );
	}
	catch( ... )
	{
		throw "no more memory!";
	}
}

CThread::~CThread()
{
	delete m_pMsgCur;
	m_pMsgCur = NULL;
	delete m_pMsgLast;
	m_pMsgLast = NULL;
}

BOOL CThread::PostThreadMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
	assert( m_hThread );
	if ( m_hThread )
	{
		return ::PostThreadMessage( m_nThreadID, message, wParam, lParam );
	}
	return FALSE;
}

BOOL CThread::PreTranslateMessage( MSG* pMsg )
{
	return FALSE;
}