#include "UIWindow.h"
#include <WindowsX.h>
#include "UIView.h"
#include <process.h>
#include "Aplication.h"
void CALLBACK CheckExitTimer(HWND, UINT, UINT_PTR, DWORD)
{
	PostQuitMessage( 0 );
}
void DoExit(  )
{
	SetTimer( NULL, 0, 10, CheckExitTimer );
}
map<HWND, CUIWindow*> CUIWindow::m_mapWindows;
BOOL CUIWindow::Create( const RECT& rect, unsigned long nStyle /*= WS_DEFAULT*/, unsigned long nExStyle /*= WS_EX_DEFAULT*/, HWND parent /*= NULL */ )
{
	WNDCLASS wndcls;
	if (!(::GetClassInfo(NULL, X_UI, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = CUIWindow::WinProcess;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = (HINSTANCE)::GetModuleHandle(NULL);
		wndcls.hIcon            = NULL;
#ifndef _WIN32_WCE_NO_CURSOR
		wndcls.hCursor          = LoadCursor( NULL, IDC_ARROW );
#else
		wndcls.hCursor          = NULL;
#endif
		wndcls.hbrBackground    = NULL;
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = X_UI;
		RegisterClass(&wndcls);
	}
	if ( WS_CAPTION&nStyle )
	{
		m_dwStyle |= XWS_CAPTION;
		nStyle &= (~WS_CAPTION);
	}
	m_hWnd = CreateWindowEx( nExStyle, X_UI, _T(""),  nStyle, 0, 0, 0, 0, parent, NULL, (HINSTANCE)::GetModuleHandle(NULL), NULL );
	if ( m_hWnd )
	{
		Attach( m_hWnd );
		SetWindowPos( m_hWnd, HWND_TOP, rect.left, rect.top, RECT_W(rect), RECT_H(rect), SWP_NOZORDER|SWP_NOACTIVATE );
		OnInitWnd();
 		if ( XWS_CAPTION & m_dwStyle )
 		{
 			Show( SW_MINIMIZE );
 			Show( SW_HIDE );
 		}
		if ( nStyle&WS_VISIBLE )
		{
			SetWindowPos( m_hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE );
		}
	}
	return m_hWnd != NULL;
}

LRESULT CUIWindow::WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CUIWindow* pWin = CUIWindow::FromHandle( hWnd );
	if ( pWin )
	{
		return pWin->WinProc( message, wParam, lParam );
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT CUIWindow::WinProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_SIZE:
		{
			DefWindowProc( m_hWnd, message, wParam, lParam );
			if ( m_hRgn )
			{
				DeleteObject( m_hRgn );
				m_hRgn = NULL;
			}
			if( m_arcTop > 0 || m_arcBottom > 0 )
 			{
 				RECT rcWin;
 				GetRect( &rcWin );
				if( RECT_W(rcWin) < 6
					|| RECT_H(rcWin) < 6 )
				{
					break;
				}
				if ( m_arcTop > 4
					|| m_arcBottom > 4 )
				{
					m_hRgn = ::CreateRoundRectRgn( 0, 0, RECT_W(rcWin), RECT_H(rcWin), m_arcTop<<1, m_arcBottom<<1 );
				}
				else
				{
 					RECT_TO(rcWin, 0, 0 );
 					POINT points[]=
 					{
 						{ rcWin.left+m_arcTop, rcWin.top },
 						{ rcWin.right-m_arcTop, rcWin.top },
 						{ rcWin.right, rcWin.top+m_arcTop },
 						{ rcWin.right, rcWin.bottom-m_arcBottom },
 						{ rcWin.right-m_arcBottom, rcWin.bottom },
 						{ rcWin.left+m_arcBottom, rcWin.bottom },
 						{ rcWin.left, rcWin.bottom-m_arcBottom },
 						{ rcWin.left, rcWin.top + m_arcTop },
					};
					m_hRgn = CreatePolygonRgn( points, 8, ALTERNATE );
				}
				
				if ( m_hRgn )
				{
					OnSize( (UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
					HRGN hrgn = CreateRectRgn( 0,0,0,0 );
					CopyRgn( hrgn, m_hRgn );
					SetWindowRgn( hrgn, TRUE );
					return S_OK;
				}
			}
			OnSize( (UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
			return S_OK;
		}
		break;
	case WM_NCHITTEST:
		{
#define			BORDER -3
			if ( m_dwStyle & XWS_CAPTION )
			{
				RECT rcWin;
				GetRect( &rcWin );
				RECT_INFLATE( rcWin, BORDER, BORDER, BORDER-1, BORDER-1 );
				POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				if ( point.x < rcWin.left  )
				{
					if ( point.y < rcWin.top  )
					{
						return HTTOPLEFT;
					}
					else if ( point.y > rcWin.bottom )
					{
						return HTBOTTOMLEFT;
					}
					return HTLEFT;
				}
				else if( point.x > rcWin.right )
				{
					if ( point.y < rcWin.top  )
					{
						return HTTOPRIGHT;
					}
					else if ( point.y > rcWin.bottom )
					{
						return HTBOTTOMRIGHT;
					}
					return HTRIGHT;
				}
				else
				{
					if ( point.y < rcWin.top  )
					{
						return HTTOP;
					}
					else if ( point.y > rcWin.bottom )
					{
						return HTBOTTOM;
					}
					return HTCLIENT;
				}
			}
		}
		break;
 	case WM_NCACTIVATE:
 	case WM_NCMOUSEMOVE:
 	case WM_NCPAINT:
 	case WM_NCRBUTTONDOWN:
 		{
  			return TRUE;
 		}
	case WM_LBUTTONDOWN:
		{
			if ( m_dwStyle & XWS_CAPTION )
			{
				POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				RECT rcClient;
				GetClient( &rcClient );
				::InflateRect( &rcClient, -3, -3 );
				if ( ::PtInRect( &rcClient, point ) )
				{
					PostMessage( WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y)); 
				}
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			if ( m_dwStyle & XWS_CAPTION )
			{
				POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				RECT rcClient;
				GetClient( &rcClient );
				::InflateRect( &rcClient, -3, -3 );
				if ( ::PtInRect( &rcClient, point ) )
				{
					PostMessage( WM_NCLBUTTONDBLCLK,HTCAPTION,MAKELPARAM(point.x,point.y)); 
				}
			}
		}
		break;
	case WM_NCCALCSIZE:
		{
 			if ( wParam )
 			{
 				return WVR_REDRAW;
 			}
		}
		break;
	case WM_GETMINMAXINFO:
		{
  			MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
  			if ( pInfo )
  			{
  				RECT rcArea;
  				SystemParametersInfo( SPI_GETWORKAREA, 0, (PVOID)&rcArea, 0 );
 				pInfo->ptMaxSize.x = m_nMaxX>0?m_nMaxX:RECT_W(rcArea);
   				pInfo->ptMaxSize.y = m_nMaxY>0?m_nMaxY:RECT_H(rcArea);
   				pInfo->ptMaxTrackSize.x = m_nMaxX>0?m_nMaxX:RECT_W(rcArea);
   				pInfo->ptMaxTrackSize.y = m_nMaxY>0?m_nMaxY:RECT_H(rcArea);
 				pInfo->ptMinTrackSize.x = m_nMinX>0?m_nMinX:0;
 				pInfo->ptMinTrackSize.y = m_nMinY>0?m_nMinY:0;
 				pInfo->ptMaxPosition.x = 0;
 				pInfo->ptMaxPosition.y = 0;
  			}
			return S_OK;
		}
		break;
	case WM_NCDESTROY:
		{
			if ( GLGetApp()
				&& GLGetApp()->m_hMainWnd == m_hWnd )
			{
				PostQuitMessage( 0 );
			}
			Show( SW_HIDE );
		}
		break;
	default:
		;
	}
	return DefWindowProc( m_hWnd, message, wParam, lParam );
}

void CUIWindow::SetBkGnd( const IMG& image, bool bUpdate /*= true*/ )
{
	
	if( image.hImage )
	{
		if ( m_imageBkGnd.hImage != image.hImage )
		{
			m_imageBkGnd = image;
			m_imageBkGnd.imageAvgClr = GetAvgColor( image.hImage );
#ifdef SURPORT_EXT
			HIMAGE imgTmp = image.hImage;
			HDC hdc = GetDC(  );
			m_imageBkGnd.hImage = GetTileImage( hdc, imgTmp, m_imageBkGnd.imageAvgClr, 50 );
			ReleaseDC( hdc );
#endif
		}	
		ImageInfo imageInfo = {0};
		image.hImage->GetImageInfo( &imageInfo );
		if ( (int)imageInfo.Width < image.rcValid.right )
		{
			m_imageBkGnd.rcValid.right = imageInfo.Width;
		}
		if ( (int)imageInfo.Height < image.rcValid.bottom )
		{
			m_imageBkGnd.rcValid.bottom = imageInfo.Height;
		}
	}
	else
	{
		if ( image.imageAvgClr == m_imageBkGnd.imageAvgClr
			&& m_imageBkGnd.hImage == NULL )
		{
			return;
		}
		m_imageBkGnd = image;
	}

	if ( X_TRANSPARENT != m_imageBkGnd.imageAvgClr )
	{
		HBRUSH hBr = CreateSolidBrush( m_imageBkGnd.imageAvgClr );
		hBr = (HBRUSH)(DWORD_PTR)SetClassLongPtr( m_hWnd, GCL_HBRBACKGROUND, (LONG)(LONG_PTR)hBr );
		if ( hBr )
		{
			DeleteObject( hBr );
		}
	}
	if ( bUpdate )
	{
		::UpdateBkGnd( m_hWnd );
		InvalidRect( NULL );
	}
}

void CUIWindow::SetRgn( HRGN hRgn )
{
	m_hRgn = hRgn;
	if ( m_hRgn )
	{
		DeleteObject( m_hRgn );
		m_hRgn = NULL;
	}
	CopyRgn( m_hRgn, hRgn );
	SetWindowRgn( hRgn, TRUE );
}

BOOL CUIWindow::EnableBlurWindow( bool bEnable )
{
	IMG image;
	image.imageAvgClr = Color( 254, 0,0,0).GetValue();
	SetBkGnd( image );
#ifdef SURPORT_DWM
	EnableComposition(true);
	return S_OK == ::EnableBlurWindow( m_hWnd, bEnable );
#else
	return FALSE;
#endif
}

int CUIWindow::DoModal( const RECT& rect, ULONG nStyle, ULONG nExStyle )
{
	m_bModal = true;
	m_nResult = IDCANCEL;
	
	if ( !GLGetApp() )
	{
		return m_nResult;
	}
	HWND hwndParent = GLGetApp()->m_hMainWnd;
	HWND hParent = NULL;
	// a popup window cannot be owned by a child window
	while (hwndParent != NULL && (::GetWindowLong(hwndParent, GWL_STYLE) & WS_CHILD))
	hwndParent = ::GetParent(hwndParent);

	// determine toplevel window to disable as well
	HWND hWndTop = hwndParent, hWndTemp = hwndParent;
	for (;;)
	{
		if (hWndTemp == NULL)
			break;
		else
			hWndTop = hWndTemp;
		hWndTemp = ::GetParent(hWndTop);
	}

	// get last active popup of first non-child that was found
	if (hParent == NULL && hwndParent != NULL)
	hwndParent = ::GetLastActivePopup(hwndParent);

	bool bEnableWindow = false;
	if ( hwndParent && IsWindowEnabled( hwndParent ) )
	{
		EnableWindow( hwndParent, FALSE );
		bEnableWindow = true;
	}
	if( !Create( rect, nStyle, nExStyle, hwndParent ) )
	{
		return m_nResult;
	}
	if ( GLGetApp()
		&& GLGetApp()->m_hMainWnd == NULL )
	{
		GLGetApp()->m_hMainWnd = m_hWnd;
		Show( GLGetApp()->m_nShowCmd );
		UpdateWindow(  );
		SetForegroundWindow( m_hWnd );
	}
	else
	{
		Show( SW_SHOW );
	}
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	MSG msg = {0};
	MSG msgLast = {0};
	MSG msgCur = {0};
	//for tracking the idle time state
	// acquire and dispatch messages until a WM_QUIT message is received.
	while (m_bModal)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!SendMessage(0x036A, MSGF_DIALOGBOX, lIdleCount++))
			{
				// stop idle processing next time
				bIdle = FALSE;
			}
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if (!::GetMessage( &msgCur, NULL, NULL, NULL))
			{
				// Note: prevents calling message loop things in 'ExitInstance'
				// will never be decremented
				m_bModal = false;
				break;
			}
			// process this message
			if ( msgCur.message != 0x036A )
			{
				::TranslateMessage(&msgCur);
				::DispatchMessage(&msgCur);
			}

			// reset "no idle" state after pumping "normal" message
			//if (IsIdleMessage(&m_msgCur))
			// reset "no idle" state after pumping "normal" message
			if (msg.message == WM_MOUSEMOVE || msg.message == WM_NCMOUSEMOVE)
			{
				// mouse move at same position as last mouse move?
				if (msgLast.pt.x != msg.pt.x
					||msgLast.pt.y != msg.pt.y
					||msgCur.message != msg.message )
				{
					msgLast = msg;
					bIdle = TRUE;
					lIdleCount = 0;
				}
			}
			else if ( msg.message != WM_PAINT && msg.message != 0x0118 )
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}
		} while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
	}
	if (m_hWnd != NULL)
	{
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}
	if ( bEnableWindow )
	{
		EnableWindow( hwndParent, TRUE );
	}
	if ( m_hWnd == GLGetApp()->GetMainWnd() )
	{
		GLGetApp()->m_hMainWnd = NULL;
	}
	if ( m_hWnd )
	{
		DestroyWindow( m_hWnd );
	}
	return m_nResult;
}

void CUIWindow::OnOK()
{
	if ( m_bModal )
	{
		m_nResult = IDOK;
		m_bModal = false;
		DoExit(  );
		return;
	}
	if ( GLGetApp() )
	{
		if ( m_hWnd == GLGetApp()->GetMainWnd() )
		{
			DoExit(  );
			return;
		}
	}
	if (m_hWnd != NULL)
	{
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void CUIWindow::OnCancel()
{
	if ( m_bModal )
	{
		m_nResult = IDCANCEL;
		m_bModal = false;
		DoExit();
		return;
	}
	if ( GLGetApp() )
	{
		if ( m_hWnd == GLGetApp()->GetMainWnd() )
		{
			DoExit();
			return;
		}
	}
	if (m_hWnd != NULL)
	{
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void CUIWindow::EndModal( int nResult )
{
	assert( m_bModal );
	if ( m_bModal )
	{
		m_nResult = nResult;
		if ( GLGetApp() )
		{
			if ( m_hWnd == GLGetApp()->GetMainWnd() )
			{
				DoExit();
			}
			return;
		}
		if (m_hWnd != NULL)
		{
			SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
}

BOOL CUIWindow::Attach( HWND hWnd )
{
	Detach();
	CUIWindow* pWin = NULL;
	map<HWND, CUIWindow*>::iterator _itFound = m_mapWindows.end();
	if( m_mapWindows.size() )
	{
		_itFound = m_mapWindows.find( hWnd );
	}
	if( _itFound != m_mapWindows.end() )
	{
		pWin = _itFound->second;
	}
	if ( NULL == pWin )
	{
		try
		{
			m_mapWindows[hWnd] = this;
			return TRUE;
		}
		catch( ... )
		{
			return FALSE;
		}
	}
	return FALSE;
}

HWND CUIWindow::Detach()
{
	CUIWindow* pWin = NULL;
	if ( m_mapWindows.size() )
	{
		map<HWND, CUIWindow*>::iterator _itFound = m_mapWindows.find( m_hWnd );
		if( _itFound != m_mapWindows.end() )
		{
			pWin = _itFound->second;
		}
		if ( pWin )
		{
			m_mapWindows.erase( m_hWnd );
		}
	}
	return m_hWnd;
}

CUIWindow* CUIWindow::FromHandle( HWND hWnd )
{
	CUIWindow* pWin = NULL;
	if ( m_mapWindows.size() )
	{
		map<HWND, CUIWindow*>::iterator _itFound = m_mapWindows.find( hWnd );
		if( _itFound != m_mapWindows.end() )
		{
			return _itFound->second;
		}
	}
	return NULL;
}