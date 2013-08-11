#ifndef __UI_THREAD_H__
#define __UI_THREAD_H__
#include <windows.h>
#include <assert.h>
typedef UINT (WINAPI *THREADPROC)(LPVOID);
class CThread
{
public:
	// Constructors
	CThread();
	BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0,
		LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

	// Attributes
	HWND m_hMainWnd;       // main window (usually same AfxGetApp()->m_pMainWnd)
	BOOL m_bAutoDelete;     // enables 'delete this' after thread termination
	HACCEL m_hAccel;
	// only valid while running
	HANDLE m_hThread;       // this thread's HANDLE
	operator HANDLE() const;
	DWORD m_nThreadID;      // this thread's ID

	int GetThreadPriority();
	BOOL SetThreadPriority(int nPriority)
	{ assert(m_hThread != NULL); return ::SetThreadPriority(m_hThread, nPriority); }

	// Operations
	DWORD SuspendThread();
	DWORD ResumeThread();
	BOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);

	// Overridables
	// thread initialization
	virtual BOOL InitInstance();

	// running and idle processing
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
	virtual BOOL IsIdleMessage(MSG* pMsg);  // checks for special messages
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// thread termination
	virtual int ExitInstance(); // default will 'delete this'

	// Advanced: virtual access to m_pMainWnd
	virtual HWND GetMainWnd()
	{
		return m_hMainWnd;
	}

	// Implementation
public:
	virtual ~CThread();
	virtual void Delete();
	// 'delete this' only if m_bAutoDelete == TRUE

public:
	// constructor used by implementation of AfxBeginThread
	CThread(THREADPROC pfnThreadProc, LPVOID pParam);

	// valid after construction
	LPVOID m_pThreadParams; // generic parameters passed to starting function
	THREADPROC m_pfnThreadProc;
private:
	MSG*    m_pMsgLast;
	MSG*    m_pMsgCur;
};

#endif