#include "UICtrl.h"
#include "Aplication.h"
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{	
	assert(GLGetApp());
	int nResult = 0;
	if ( GLGetApp() )
	{
		GLGetApp()->m_pszCommandLine = _tcsdup( lpCmdLine );
		GLGetApp()->m_nShowCmd = nCmdShow;
		GLGetApp()->m_hInstance = hInstance;
		if (!GLGetApp()->InitInstance())
		{
			nResult = GLGetApp()->ExitInstance();
		}
		if ( GLGetApp()->m_hMainWnd )
		{
			ShowWindow( GLGetApp()->m_hMainWnd, nCmdShow);
			UpdateWindow( GLGetApp()->m_hMainWnd );
			SetForegroundWindow( GLGetApp()->m_hMainWnd );
			nResult = GLGetApp()->Run();
		}
	}
	return nResult;
}