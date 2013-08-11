#ifndef __DWM_DEFINE__
#define __DWM_DEFINE__
#include <Windows.h>

#ifndef DWM_EC_DISABLECOMPOSITION 
#define DWM_EC_DISABLECOMPOSITION 0 
#endif /// DWM_EC_DISABLECOMPOSITION 

#ifndef DWM_EC_ENABLECOMPOSITION 
#define DWM_EC_ENABLECOMPOSITION 1 
#endif /// DWM_EC_ENABLECOMPOSITION 

#ifndef DWM_BB_ENABLE
#define DWM_BB_ENABLE 0x00000001
#endif

#ifndef DWM_BB_BLURREGION
#define DWM_BB_BLURREGION 0x00000002
#endif

#ifndef DWM_BB_TRANSITIONONMAXIMIZED
#define DWM_BB_TRANSITIONONMAXIMIZED 0x00000004
#endif

__if_not_exists(DWM_BLURBEHIND)
{
	typedef struct _DWM_BLURBEHIND {
		DWORD dwFlags;
		BOOL  fEnable;
		HRGN  hRgnBlur;
		BOOL  fTransitionOnMaximized;
	} DWM_BLURBEHIND, *PDWM_BLURBEHIND;
};

typedef HRESULT (WINAPI *DwmEnableBlurBehindWindow)(
	HWND hWnd,
	 const DWM_BLURBEHIND *pBlurBehind
	);

typedef HRESULT (WINAPI *DwmEnableComposition)(
	UINT uCompositionAction
	);

class CDwmProxy
{
public:
	CDwmProxy()
		:m_hDwmDll(NULL)
		,m_enableBlurWindow(NULL)
		,m_enableComposition(NULL)
	{

	}
	~CDwmProxy()
	{
		m_enableBlurWindow = NULL;
		m_enableComposition = NULL;
		if ( m_hDwmDll )
		{
			FreeLibrary( m_hDwmDll );
			m_hDwmDll = NULL;
		}
	}
	HRESULT EnableComposition( bool bEnable )
	{
		if ( NULL == m_enableComposition )
		{
			m_hDwmDll = LoadLibrary( _T( "Dwmapi.dll " ) );
			if ( m_hDwmDll )
			{
				m_enableComposition = (DwmEnableComposition)GetProcAddress( m_hDwmDll, "DwmEnableComposition" );
			}
		}
		if ( NULL != m_enableComposition )
		{
			UINT uAction = bEnable?DWM_EC_ENABLECOMPOSITION:DWM_EC_DISABLECOMPOSITION;
			return m_enableComposition( uAction );
		}
		return S_FALSE;

	}
	HRESULT EnableBlurWindow( HWND hWnd, bool bEnable )
	{
		if ( NULL == m_enableBlurWindow )
		{
			m_hDwmDll = LoadLibrary( _T( "Dwmapi.dll " ) );
			if ( m_hDwmDll )
			{
				m_enableBlurWindow = (DwmEnableBlurBehindWindow)GetProcAddress( m_hDwmDll, "DwmEnableBlurBehindWindow" );
			}
		}
		if ( NULL != m_enableBlurWindow )
		{
			DWM_BLURBEHIND bb = {0};
			// Enable Blur Behind and apply to the entire client area
			bb.dwFlags = DWM_BB_ENABLE;
			bb.fEnable = bEnable;
			bb.hRgnBlur = NULL;
			return m_enableBlurWindow( hWnd, &bb );
		}
		return S_FALSE;
	}
private:
	HMODULE m_hDwmDll;
	DwmEnableComposition m_enableComposition;
	DwmEnableBlurBehindWindow m_enableBlurWindow;
};

#endif