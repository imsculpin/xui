#include "Switch.h"
#include "tray.h"
#include <utility>
using std::make_pair;
#pragma comment( lib, "shell32.lib" )
#define  TRAY_MSG _T("X_UI_TRAY")
#define  ID_TIMER_FLASH_BASE 1024
map< UINT, CTray::LISTENER > CTray::m_mapUICALLBACKs;
HWND CTray::m_hMessage = NULL;
BOOL CTray::Create( UINT nTrayId, LPCTSTR lpszTip  /*=NULL*/ )
{	
	m_TrayId = nTrayId;
	if ( !IsWindow(m_hMessage) )
	{
		WNDCLASS wndcls={0};
		HINSTANCE hInstance = (HINSTANCE)::GetModuleHandle(NULL);
		if (!(::GetClassInfo(hInstance, TRAY_MSG, &wndcls)))
		{	
			// otherwise we need to register a new class;
			wndcls.lpfnWndProc      = CTray::TrayMessage;
			wndcls.hInstance        = hInstance;
			wndcls.lpszClassName    = TRAY_MSG;

			if (!RegisterClass(&wndcls))
			{
				return FALSE;
			}
		}
		m_hMessage = CreateWindowEx( WS_EX_TOOLWINDOW, TRAY_MSG, _T(""),  WS_POPUP|WS_VISIBLE, 0, 0, 0, 0, NULL, NULL, hInstance, NULL );
		SetWindowPos( m_hMessage, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );

	}
	
	assert( NULL != m_hMessage );
	memset(&m_notifyData, 0, sizeof(m_notifyData));
	// the basic functions need only V1
#ifdef NOTIFYICONDATA_V1_SIZE
	m_notifyData.cbSize = NOTIFYICONDATA_V1_SIZE;
#else
	m_notifyData.cbSize = sizeof(m_notifyData);
#endif
	m_notifyData.hWnd = m_hMessage;
	m_notifyData.uID = nTrayId;
	m_notifyData.uCallbackMessage = nTrayId;
	int nLen = 0;
	if ( lpszTip )
	{
		nLen = (int)_tcslen( lpszTip );
		if ( nLen > 0 )
		{
#if (_WIN32_IE >= 0x0500)
			nLen = min( 127, nLen );
#else
			nLen = min( 63, nLen );
#endif
			++nLen;
			memcpy( m_notifyData.szTip, lpszTip,nLen*sizeof(TCHAR) );
		}
	}
	return TRUE;
}

BOOL CTray::SetTipInfo( const tstring& strTip )
{
	int nLen = (int)strTip.length();
	nLen = min( sizeof( m_notifyData.szTip )/sizeof(TCHAR)-1, nLen );
	++nLen;
	m_notifyData.szTip[0] = 0;
	memcpy( m_notifyData.szTip, strTip.c_str(),nLen*sizeof(TCHAR) );
	m_notifyData.uFlags = NIF_TIP;
	return Shell_NotifyIcon( NIM_MODIFY, &m_notifyData );
}

BOOL CTray::SetImage( const HIMAGE* pHImages, int nImages )
{
	m_vecImage.clear();
	for ( int _i = 0; _i < nImages; ++_i  )
	{
		m_vecImage.push_back( pHImages[_i] );
	}
	if ( m_vecImage.size() )
	{
		if ( m_hIcon )
		{
			DestroyIcon( m_hIcon );
			m_hIcon = NULL;
		}
		m_hIcon = (*m_vecImage.begin())->GetIcon();
		m_notifyData.hIcon = m_hIcon;
	}
	if ( m_bVisable
		&& IsWindow(m_hMessage)
		&& m_vecImage.size()
		&& NULL != *(m_vecImage.begin()))
	{
		m_notifyData.uFlags = NIF_ICON;
		assert( m_hIcon );
		return Shell_NotifyIcon(NIM_MODIFY, &m_notifyData);
	}
	return FALSE;
}

BOOL CTray::Show( bool bShow )
{
	if ( !IsWindow(m_hMessage) )
	{
		return FALSE;
	}
	if ( bShow != m_bVisable )
	{
		m_bVisable = bShow;
		if ( m_bVisable )
		{		
			m_notifyData.uFlags = NIF_MESSAGE|NIF_ICON | NIF_TIP;
			return Shell_NotifyIcon(NIM_ADD, &m_notifyData);
		}
		else
		{
			return Shell_NotifyIcon(NIM_DELETE, &m_notifyData);
		}
	}
	return FALSE;
}

LRESULT CALLBACK CTray::TrayMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( WM_TIMER == message )
	{
		map< UINT, LISTENER >::iterator _lab = m_mapUICALLBACKs.find( (UINT)(wParam-ID_TIMER_FLASH_BASE) );
		if ( _lab == m_mapUICALLBACKs.end() )
		{
			return S_OK;
		}
		CTray* pTray = _lab->second.pThis;
		if ( pTray )
		{
			pTray->m_notifyData.uFlags = NIF_ICON;
			if ( pTray->m_vecImage.size() > 1 )
			{
				pTray->m_curFrame = (pTray->m_curFrame+1)%(int)pTray->m_vecImage.size();
				HIMAGE hImage = *(pTray->m_vecImage.begin()+pTray->m_curFrame);
				if ( hImage )
				{
					if ( pTray->m_hIcon )
					{
						DestroyIcon( pTray->m_hIcon );
						pTray->m_hIcon = NULL;
					}
					pTray->m_hIcon = hImage->GetIcon();
				}
				pTray->m_notifyData.hIcon = pTray->m_hIcon;
			}
			else
			{
				if ( pTray->m_vecImage.size() > 0 )
				{
					pTray->m_curFrame = (pTray->m_curFrame+1)%2;
					if ( pTray->m_curFrame == 0 )
					{
						pTray->m_notifyData.hIcon = pTray->m_hIcon;
					}
					else
					{
						pTray->m_notifyData.hIcon = NULL;
					}
				}
			}
			Shell_NotifyIcon(NIM_MODIFY, &pTray->m_notifyData);
		}
		return S_OK;
	}
	if ( m_mapUICALLBACKs.size() )
	{
		map< UINT, LISTENER >::iterator _lab = m_mapUICALLBACKs.find( message );
		if ( _lab != m_mapUICALLBACKs.end() )
		{
			if ( _lab->second.listener )
			{
				if ( WM_RBUTTONDOWN == lParam )
				{
					SendMessage( GLGetApp()->m_hMainWnd, WM_ACTIVATEAPP, TRUE, GetCurrentThreadId() );
					SetForegroundWindow( m_hMessage );
				}
				POINT point;
				GetCursorPos( &point );
				NM_PARAM param = {0};
				param.point = point;
				param.nCtrlId = message;
				param.wParam = wParam;
				(*_lab->second.listener)( (UINT)lParam, message, (LONG)(LONG_PTR)&param, _lab->second.pThis->m_dwParam );
			}
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

#if (_WIN32_IE >= 0x0500)
BOOL CTray::ShowBalloonTip( const tstring& strTitle, const tstring& strInfo, int nTimeOut /*= 1000*/, TIPTYPE type /*= E_NONE */ )
{
 	int nLen = (int)strTitle.length();
 	nLen = min( sizeof(m_notifyData.szInfoTitle)/sizeof(TCHAR)-1, nLen );
 	++nLen;
 	m_notifyData.szInfoTitle[0] = 0;
 	memcpy( m_notifyData.szInfoTitle, strTitle.c_str(),nLen*sizeof(TCHAR) );
 
 	nLen = (int)strInfo.length();
 	nLen = min( sizeof(m_notifyData.szInfo)/sizeof(TCHAR)-1, nLen );
 	++nLen;
 	memcpy( m_notifyData.szInfo, strInfo.c_str(), nLen*sizeof(TCHAR) );
	m_notifyData.cbSize = sizeof( m_notifyData );
 	m_notifyData.uFlags = NIF_INFO;
 	m_notifyData.dwInfoFlags = type;
 	m_notifyData.uTimeout = nTimeOut;
 	m_notifyData.szTip[0] = 0;
 	return Shell_NotifyIcon( NIM_MODIFY, &m_notifyData );
}

#endif

void CTray::BeginFlash( int nRate /*= 100 */ )
{
	assert( IsWindow( m_hMessage ) );
	if ( IsWindow( m_hMessage ) )
	{
		StopFlash();
		SetTimer( m_hMessage, ID_TIMER_FLASH_BASE+m_TrayId, nRate, NULL );
	}
	return;
}

void CTray::StopFlash( )
{
	if ( m_hMessage )
	{
		assert( IsWindow( m_hMessage ) );
		if ( IsWindow( m_hMessage ) )
		{
			KillTimer( m_hMessage, ID_TIMER_FLASH_BASE+m_TrayId );
			if ( m_curFrame == 0 )
			{
				return;
			}
			m_notifyData.uFlags = NIF_ICON;
			m_notifyData.hIcon = m_hIcon;
			if ( m_vecImage.size() > 1 )
			{
				if ( m_hIcon )
				{
					DestroyIcon( m_hIcon );
					m_hIcon = NULL;
				}
				m_hIcon = (*m_vecImage.begin())->GetIcon();
				m_notifyData.hIcon = m_hIcon;
			}
			m_curFrame = 0;
			Shell_NotifyIcon(NIM_MODIFY, &m_notifyData);
		}
	}

}

void CTray::SetUICALLBACK( UICALLBACK listener, DWORD dwParam )
{
	m_dwParam = dwParam;
	LISTENER _listener;
	_listener.listener = listener;
	_listener.pThis = this;
	m_mapUICALLBACKs.insert( make_pair(m_TrayId, _listener) );
}

void CTray::Destroy()
{
	StopFlash();
	if ( m_hMessage )
	{
		Show( false );
		m_vecImage.clear();
		if ( m_mapUICALLBACKs.size() )
		{
			m_mapUICALLBACKs.erase( m_TrayId );
		}
		m_TrayId = 0;
		if ( m_hIcon )
		{
			DestroyIcon( m_hIcon );
			m_hIcon = NULL;
		}
		if ( m_mapUICALLBACKs.empty() )
		{
			DestroyWindow( m_hMessage );
			m_hMessage = NULL;
		}
	}
}