
#include "stdafx.h"
// UIWnd.cpp : 实现文件
#include <algorithm>
#include "UIMenu.h"
#include <CommCtrl.h>
#include <tchar.h>
#include "UIdefine.h"
#include <assert.h>
#include <WindowsX.h>

map<HWND, CUIMenu*> CUIMenu::m_mapWindows;
// CUIMenu
CUIMenu::CUIMenu()
:m_nCmdRet(0)
,m_hBufferBmp(NULL)
,m_hOldBufferBmp( NULL )
,m_bPoping( FALSE )
,m_hWnd( NULL )
,m_subMenu(NULL)
,m_nMenuCount( 0 )
,m_pMenuData( NULL )
,m_pAccel(NULL)
,m_nAccelCount(0)
,m_fuFlags(0)
,m_nMouseIn(-1)
,m_nLbDown(-1)
{
	LOGFONT lgFont;
	memset(&lgFont, 0, sizeof(LOGFONT));
	lgFont.lfHeight = -12;
	lstrcpy(lgFont.lfFaceName, _T("Arial"));
	lgFont.lfOutPrecision = OUT_TT_PRECIS;
	lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lgFont.lfQuality = PROOF_QUALITY;
	lgFont.lfQuality = ANTIALIASED_QUALITY;
	lgFont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
	lgFont.lfWeight = FW_NORMAL;
	m_font = CreateFontIndirect( &lgFont );
}


BOOL CUIMenu::InsertMenu( const tstring& strText, HIMAGE& hImage, UINT nCmdId, BOOL bEnable /*= TRUE*/ )
{
	for ( vector<MENUDATA>::iterator _lab = m_vecMenuData.begin();
		_lab !=  m_vecMenuData.end(); ++_lab )
	{
		if ( _lab->nCmdId == nCmdId )
		{
			return TRUE;
		}
	}
	//添加菜单项
	MENUDATA menuData;
	menuData.cbSize = sizeof( menuData );
	m_vecMenuData.push_back( menuData );
	RtlCopyMemory( m_vecMenuData.back().pszText, strText.c_str(), (strText.length()+1)*sizeof(TCHAR) );
	m_vecMenuData.back().hImage = hImage;
	m_vecMenuData.back().nCmdId = nCmdId;
	m_vecMenuData.back().pSubMenu = NULL;
	m_vecMenuData.back().nSubCount = 0;
	m_vecMenuData.back().pSubAccel = NULL;
	m_vecMenuData.back().nSubAccelcount = 0;
	m_vecMenuData.back().bEnable = bEnable;
	return TRUE;
}

BOOL CUIMenu::InsertSpaceMenu( const HIMAGE& hImage )
{
	MENUDATA menuData;
	menuData.cbSize = sizeof( menuData );
	m_vecMenuData.push_back( menuData );
	m_vecMenuData.back().hImage = hImage;
	m_vecMenuData.back().nCmdId = (UINT)-1;
	m_vecMenuData.back().pszText[0] = 0;
	return TRUE;
}

BOOL CUIMenu::InsertSubMenu( UINT nCmdId, const CUIMenu* pSubMenu )
{
	if ( pSubMenu )
	{
		for ( vector<MENUDATA>::iterator _lab = m_vecMenuData.begin();
			_lab !=  m_vecMenuData.end(); ++_lab )
		{
			if ( _lab->nCmdId == nCmdId )
			{
				_lab->pSubMenu = (LPVOID)&(pSubMenu->m_vecMenuData[0]);
				_lab->nSubCount = (int)pSubMenu->m_vecMenuData.size();
				_lab->pSubAccel = (LPVOID)&(pSubMenu->m_vecAccel[0]);
				_lab->nSubAccelcount = (int)pSubMenu->m_vecAccel.size();
				break;	
			}
		}
	}
	return TRUE;
}

void CUIMenu::RemoveMenu( UINT nCmdId )
{
	//移除菜单
	for ( vector<MENUDATA>::iterator _lab = m_vecMenuData.begin();
		_lab !=  m_vecMenuData.end(); ++_lab )
	{
		if ( _lab->nCmdId == nCmdId )
		{
			m_vecMenuData.erase( _lab );
			break;	
		}
	}

	//移除快捷键
	for ( vector<ACCEL>::iterator _lab = m_vecAccel.begin();
		_lab !=  m_vecAccel.end(); ++_lab )
	{
		if ( _lab->cmd == nCmdId )
		{
			m_vecAccel.erase( _lab );
			break;	
		}
	}
}

UINT CUIMenu::TrackPopupMenu( int x, int y, UINT uFlags )
{
	if ( m_vecMenuData.size() )
	{
		ACCEL* pAccels = NULL;
		if ( m_vecAccel.size() )
		{
			pAccels = &m_vecAccel[0];
		}
		//弹起Popmenu
		return TrackPopupMenu( &m_vecMenuData[0], (int)m_vecMenuData.size(), x, y, uFlags, pAccels, (int)m_vecAccel.size()  );
	}
	return 0;
}

BOOL CUIMenu::AddAccel( ACCEL& accel )
{
	for ( int _i = 0; _i < (int)m_vecAccel.size(); ++_i )
	{
		if( m_vecAccel[_i].cmd == accel.cmd
			&& accel.fVirt == m_vecAccel[_i].fVirt 
			&& accel.key == m_vecAccel[_i].key )	
		{
			return TRUE;
		}
	}
	m_vecAccel.push_back( accel );
	return TRUE;
}

CUIMenu::~CUIMenu()
{
	if ( m_bufferDC )
	{
		if ( m_hBufferBmp )
		{
			SelectObject( m_bufferDC, m_hOldBufferBmp );
			DeleteObject( m_hBufferBmp );
			m_hOldBufferBmp = NULL;
		}
	}
	if ( m_font )
	{
		DeleteObject( m_font );
		m_font = NULL;
	}
}

UINT CUIMenu::TrackPopupMenu( MENUDATA* pMenuData, int nCount, int x, int y, int fuFlags, ACCEL* pAccels /*= NULL*/, int nAccelCount /*= 0*/ )
{
	if ( NULL == pMenuData 
		|| nCount <= 0 )
	{
		return 0;
	}
	m_bPoping = TRUE;
	m_nCmdRet = 0;
	m_pAccel = pAccels;
	m_nAccelCount = nAccelCount;
	m_pMenuData = pMenuData;
	m_nMenuCount = nCount;
	m_fuFlags = fuFlags;
	SetFocus( NULL );
	if( !CreateMenu(  ) )
	{
		return m_nCmdRet;
	}
	//设置顶层窗口
	SetWindowPos(m_hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_HIDEWINDOW|
		SWP_NOSIZE|SWP_NOACTIVATE);
	HACCEL hAccel = NULL;
	if ( m_pAccel )
	{
		hAccel = CreateAcceleratorTable( m_pAccel, m_nAccelCount );
	}
	ShowWindow( m_hWnd, SW_SHOW );
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	MSG msg = {0};
	MSG msgLast = {0};
	MSG msgCur = {0};
	//for tracking the idle time state
	// acquire and dispatch messages until a WM_QUIT message is received.
	SetFocus( m_hWnd );
	while (m_bPoping)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!SendMessage( m_hWnd, 0x036A, MSGF_DIALOGBOX, lIdleCount++))
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
				m_bPoping = FALSE;
				break;
			}

			// process this message
			if ( msgCur.message != 0x036A
				&&( NULL == hAccel
				|| ( NULL != hAccel
				&& !::TranslateAccelerator( msgCur.hwnd, hAccel, &msgCur)))
				)
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

	if ( m_hWnd )
	{
		if ( m_mapWindows.size() )
		{
			m_mapWindows.erase( m_hWnd );
		}
		if (m_hWnd != NULL)
		{
			SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
		DestroyWindow( m_hWnd );
	}
	return m_nCmdRet;
}

BOOL CUIMenu::CreateMenu( HWND hParent )
{
	HWND hWnd = NULL;
	WNDCLASS wndcls;
	if (!(::GetClassInfo(NULL, _T("UI_MENU"), &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
		wndcls.lpfnWndProc      = CUIMenu::WinProcess;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = (HINSTANCE)::GetModuleHandle(NULL);
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = LoadCursor( NULL, IDC_ARROW );
		wndcls.hbrBackground    = (HBRUSH)GetStockObject( WHITE_BRUSH );
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = _T("UI_MENU");
		RegisterClass(&wndcls);
	}

	m_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, _T("UI_MENU"), _T(""),  \
		WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, 150, 200, hParent, NULL,\
		(HINSTANCE)::GetModuleHandle(NULL), NULL );
	m_itemsRect.clear();
	if ( m_hWnd )
	{
		HDC hHelpDC = GetDC( m_hWnd );
		HFONT hOld = (HFONT)SelectObject( hHelpDC, m_font );
		RECT rectItem = { 0,0,1,1 };
		int nMaxWidth = 0;
		for ( int _i = 0; _i < m_nMenuCount; ++_i )
		{
			rectItem.top = 0;
			if( m_pMenuData[_i].pszText[0] != 0 )
			{ 
				DrawText( hHelpDC, m_pMenuData[_i].pszText, (int)_tcslen(m_pMenuData[_i].pszText), &rectItem,\
					DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_CALCRECT );
				rectItem.bottom = 16;
			}
			else
			{
				if ( m_pMenuData[_i].hImage )
				{
					ImageInfo info = {0};
					m_pMenuData[_i].hImage->GetImageInfo( &info );
					rectItem.bottom = info.Height;
				}
			}
			if ( RECT_W(rectItem) > nMaxWidth )
			{
				nMaxWidth = RECT_W(rectItem);
			}
			if ( m_itemsRect.size() )
			{
				RECT_TO( rectItem, m_itemsRect.back().left, m_itemsRect.back().bottom );
			}
			m_itemsRect.push_back( rectItem );
		}
		for ( int _j = 0; _j < (int)m_itemsRect.size(); ++_j )
		{
			m_itemsRect.at( _j ).right = nMaxWidth;
		}
		::SelectObject( hHelpDC, hOld );
		ReleaseDC( m_hWnd, hHelpDC );
		rectItem = m_itemsRect.back();
		SetWindowPos( m_hWnd, NULL, 0, 0, nMaxWidth, m_itemsRect.back().bottom, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_HIDEWINDOW );
		m_mapWindows[m_hWnd] = this;
	}
	return m_hWnd != NULL;
}

void CUIMenu::DrawMenu( HDC hDC )
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );
	HBRUSH hBr = CreateSolidBrush( RGB( 0,255, 0 ) );
	FillRect( hDC, &rect, hBr);	
	if ( m_nMouseIn >= 0 )
	{
		RECT rc = m_itemsRect[m_nMouseIn];
		HBRUSH hBr = CreateSolidBrush( RGB( 0,255, 0 ) );
		FillRect( hDC, &m_itemsRect[m_nMouseIn], hBr );
		DeleteObject( hBr );
	}
	DeleteObject( hBr );
	SetTextColor( hDC, RGB( 0, 0, 0 ) );
	SetBkMode( hDC, TRANSPARENT );
	HFONT hOld = (HFONT)SelectObject( hDC, m_font );
	for ( int _i = 0; _i < m_nMenuCount; ++_i )
	{
		if( m_pMenuData[_i].pszText[0] != 0 )
		{ 
			DrawText( hDC, m_pMenuData[_i].pszText, (int)_tcslen(m_pMenuData[_i].pszText), &m_itemsRect.at(_i),\
				DT_VCENTER|DT_LEFT|DT_SINGLELINE );
		}
		else 
		{
		}
	}
	SelectObject( hDC, hOld );
}

LRESULT WINAPI CUIMenu::WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CUIMenu* pPop = CUIMenu::FromHandle( hWnd );
	if ( pPop )
	{
		return pPop->WinProc( message, wParam, lParam );
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT CUIMenu::WinProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_ERASEBKGND:
		{
			return TRUE;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = NULL;
			hDC = ::BeginPaint( m_hWnd, &ps );
// 			if ( !m_bufferDC )
// 			{
// 				m_bufferDC.GetDC( hDC );
// 			}
// 			if ( hDC )
// 			{
// 				RECT rcClip;
// 				GetClipBox( hDC, &rcClip );
// 				if ( RECT_ISEMPTY( rcClip ) )
// 				{
// 					break;
// 				}
// 				if ( NULL == m_hBufferBmp )
// 				{
// 					RECT rcWnd;
// 					GetWindowRect( m_hWnd, &rcWnd );
// 					m_hBufferBmp = ::CreateCompatibleBitmap( m_bufferDC, RECT_W(rcWnd), RECT_H(rcWnd) );
// 					if ( m_hBufferBmp )
// 					{
// 						m_hOldBufferBmp = (HBITMAP)SelectObject( m_bufferDC, m_hBufferBmp );
// 					}
// 				}
				DrawMenu( hDC );
				//BitBlt( hDC, rcClip.left, rcClip.top, RECT_W(rcClip), RECT_H(rcClip),  m_bufferDC, 0, 0, SRCCOPY );
				::EndPaint( m_hWnd, &ps );
				return TRUE;
			//}
		}
	case WM_MOUSEMOVE:
		{
			POINT point = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
			for ( int _i = 0; _i < (int)m_itemsRect.size(); ++_i )
			{
				if ( ::PtInRect( &m_itemsRect[_i], point ) 
					&& m_nMouseIn != _i )
				{
					m_nMouseIn = _i;
					InvalidateRect( m_hWnd, NULL, FALSE );
					break;
				}
			}
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof( tme );
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 2;
			_TrackMouseEvent( &tme );
		}
		break;
	case WM_SIZE:
		{
// 			if ( m_hBufferBmp )
// 			{
// 				::SelectObject( m_bufferDC, m_hOldBufferBmp );
// 				DeleteObject( m_hBufferBmp );
// 			}
// 			m_hBufferBmp = ::CreateCompatibleBitmap( m_bufferDC, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
// 			if ( m_hBufferBmp )
// 			{
// 				m_hOldBufferBmp = (HBITMAP)SelectObject( m_bufferDC, m_hBufferBmp );
// 			}
		}
		break;
	case WM_KEYDOWN:
		{
			if ( VK_UP == wParam )
			{
				--m_nMouseIn;
				if( m_nMouseIn < 0 )
				{
					m_nMouseIn = m_nMenuCount-1;
				}
				InvalidateRect( m_hWnd, NULL, FALSE );
			}
			else if ( VK_DOWN == wParam )
			{
				++m_nMouseIn;
				if( m_nMouseIn > 0 )
				{
					m_nMouseIn %= m_nMenuCount; 
				}
				else
				{
					m_nMouseIn = 0;
				}
				InvalidateRect( m_hWnd, NULL, FALSE );
			}
			else if ( VK_RETURN == wParam )
			{
				if ( m_nMouseIn >= 0 )
				{
					m_nCmdRet = m_pMenuData[m_nMouseIn].nCmdId;
				}
				else
				{
					m_nCmdRet = 0;
				}
				EndMenu();
			}
		}
		break;
	case WM_KEYUP:
		{

		}
		break;
	case WM_SYSKEYDOWN:
		{
			if ( VK_MENU == wParam )
			{
				EndMenu(); 
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT point = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
			for ( int _i = 0; _i < (int)m_itemsRect.size(); ++_i )
			{
				if ( ::PtInRect( &m_itemsRect[_i], point ) )
				{
					m_nLbDown = _i;
					break;
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT point = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
			for ( int _i = 0; _i < (int)m_itemsRect.size(); ++_i )
			{
				if ( ::PtInRect( &m_itemsRect[_i], point ) )
				{
					if( m_nLbDown >= 0
						&& m_pMenuData[_i].nCmdId > 0 )
					{
						m_nCmdRet = m_pMenuData[_i].nCmdId;
						EndMenu();
					}
					m_nLbDown = -1;
					break;
				}
			}
			m_nLbDown = -1;
		}
		break;
	case WM_MOUSELEAVE:
		{
			m_nMouseIn = -1;
			InvalidateRect( m_hWnd, NULL, FALSE );
		}
		break;
	case WM_KILLFOCUS:
		{
			if( m_subMenu
				&& m_subMenu->IsSubMenuGetFocus( (HWND)wParam ))
			{
				return S_OK;
			}
			EndMenu();
		}
		break;
	default:
		break;
	}
	return DefWindowProc( m_hWnd, message, wParam, lParam );
}

void CUIMenu::EndMenu()
{
	m_bPoping = FALSE;
	if (m_hWnd != NULL)
	{
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

CUIMenu* CUIMenu::FromHandle( HWND hWnd )
{
	CUIMenu* pWin = NULL;
	if ( m_mapWindows.size() )
	{
		map<HWND, CUIMenu*>::iterator _itFound = m_mapWindows.find( hWnd );
		if( _itFound != m_mapWindows.end() )
		{
			return _itFound->second;
		}
	}
	return NULL;
}

bool CUIMenu::IsSubMenuGetFocus( HWND hFocus )
{
	if ( m_subMenu )
	{
		if ( m_subMenu->m_hWnd == hFocus )
		{
			return true;
		}
		return m_subMenu->IsSubMenuGetFocus( hFocus );
	}
	return false;
}