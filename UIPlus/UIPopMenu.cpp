// UIWnd.cpp : 实现文件
#include <algorithm>
#include "UIPopMenu.h"
#include <CommCtrl.h>
#include <tchar.h>
#include <assert.h>
#include <WindowsX.h>
#include "UIdefine.h"
#include "UIPlus.h"
map<HWND, CUIPopMenu*> CUIPopMenu::m_mapWindows;
IMG*    CUIPopMenu::m_pBkGnd = NULL;
IMG*    CUIPopMenu::m_pSubBkGnd = NULL;
IMG*    CUIPopMenu::m_pBkItem = NULL;
IMG*    CUIPopMenu::m_pSeparator = NULL;
Margin  CUIPopMenu::m_margin = {0};
Margin  CUIPopMenu::m_subMargin = {0};
bool    CUIPopMenu::m_bLayered = false;
IImage* CUIPopMenu::m_pSubArrow = NULL;//子菜单标识图标
UINT	CUIPopMenu::m_uItemHeight = 20;//菜单高度
UINT    CUIPopMenu::m_uIconWidth = 20; //菜单项左侧图标宽度
UINT    CUIPopMenu::m_itemOffset = 0;
UINT    CUIPopMenu::m_uDrawMask  = MDM_ICON_MID;  //菜单项绘制掩码 见MENUDRAWMASK
UINT    CUIPopMenu::m_uAlpha = 255;     //菜单透明度
HFONT   CUIPopMenu::m_font = NULL;
ARGB    CUIPopMenu::m_clrFont = 0xFE000000;
ARGB    CUIPopMenu::m_clrGray = 0xFE808080;
bool    CUIPopMenu::m_bLoopRuning=false;
MENUMSGFILTER CUIPopMenu::m_msgFilter = NULL;
#define  ARROW_BORDER 5

CUIPopMenu* pTop = NULL;
static CUIPopMenu* pCurMenu = NULL;
extern CUIPopMenu* pSingleMenu;

// CUIPopMenu
CUIPopMenu::CUIPopMenu()
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
,m_hbkBitmap(NULL)
,m_hbkOldBitmap(NULL)
,m_parentMenu(NULL)
,m_hAccel(NULL)
,m_parentIndex(0)
{
	pCurMenu = this;
}

CUIPopMenu::~CUIPopMenu()
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
	if ( pTop == this )
	{
		pTop = NULL;
		pSingleMenu = NULL;
	}
}

bool PtInMenu( POINT point )
{
	RECT rect;
	CUIPopMenu* pMenu = pTop;
	while( pMenu )
	{
		if ( pMenu->m_hWnd )
		{
			GetWindowRect( pMenu->m_hWnd, &rect );
			if ( PtInRect( &rect, point ) )
			{
				return true;
			}
		}
		pMenu = pMenu->m_subMenu;
	}
	return false;
}

#define  SCREEN_BORDER 10
UINT CUIPopMenu::TrackPopupMenu( MENUDATA* pMenuData, int nCount, int x, int y, int fuFlags, ACCEL* pAccels /*= NULL*/, int nAccelCount /*= 0*/ )
{
	if ( pTop )
	{
		pTop->m_nCmdRet = 0;
		pTop->EndMenu( true );
	}
	pTop = this;
	pSingleMenu = pTop;
	pCurMenu = this;
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
	m_orgX = x;
	m_orgY = y;

	if( !CreateMenu(  ) )
	{
		return m_nCmdRet;
	}
	GetTopLeft( m_orgX, m_orgY, fuFlags );
	//设置顶层窗口
	SetWindowPos(m_hWnd, HWND_TOPMOST, m_orgX, m_orgY, 0, 0, SWP_HIDEWINDOW|
		SWP_NOSIZE|SWP_NOACTIVATE);
	if ( m_pAccel )
	{
		m_hAccel = CreateAcceleratorTable( m_pAccel, m_nAccelCount );
	}
	SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|
		SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	MSG msg = {0};
	MSG msgLast = {0};
	MSG msgCur = {0};
	//for tracking the idle time state
	// acquire and dispatch messages until a WM_QUIT message is received.
	//SetActiveWindow( m_hParentWnd );
	m_bLoopRuning = true;
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
			HACCEL hAccel = m_hAccel;
			if ( pCurMenu )
			{
				hAccel = pCurMenu->m_hAccel;
			}
			// process this message
			if ( msgCur.message != 0x036A
				&&( NULL == hAccel
				|| ( NULL != hAccel
				&& !::TranslateAccelerator( msgCur.hwnd, hAccel, &msgCur)))
				)
			{
				POINT point;
				GetCursorPos( &point );
				if ( pCurMenu )
				{
					if ( WM_KEYDOWN == msgCur.message
						|| WM_KEYUP == msgCur.message
						|| WM_SYSKEYDOWN == msgCur.message
						|| WM_SYSKEYUP == msgCur.message 
						)
					{
						pCurMenu->WinProc( msgCur.message, msgCur.wParam, msgCur.lParam );
						continue;
					}
					else if ( WM_LBUTTONDOWN == msgCur.message
						|| WM_RBUTTONDOWN == msgCur.message 
						|| WM_LBUTTONDBLCLK == msgCur.message 
						|| WM_RBUTTONDOWN == msgCur.message 
						)
					{
						if ( !PtInMenu( point ) )
						{
							pCurMenu->WinProc( WM_KILLFOCUS, 0, 0 );
						}
					}
					else if ( WM_NCLBUTTONDOWN == msgCur.message 
						|| WM_NCRBUTTONUP == msgCur.message )
					{
						if ( !PtInMenu( point ) )
						{
							pCurMenu->WinProc( WM_KILLFOCUS, 0, 0 );
						}
					}
				}
				if (( !m_msgFilter
					||(m_msgFilter 
					&& !(*m_msgFilter)( &msgCur )))
					)
				{
					::TranslateMessage(&msgCur);
					::DispatchMessage(&msgCur);
				}
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

		} while ( m_bPoping&&::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
	}
	m_bLoopRuning = false;
	if ( m_hAccel )
	{
		DestroyAcceleratorTable( m_hAccel );
		m_hAccel = NULL;
	}
	if ( m_hWnd )
	{
		if ( m_mapWindows.size() )
		{
			m_mapWindows.erase( m_hWnd );
		}
		m_itemsRect.clear();
		if (m_hWnd != NULL)
		{
			SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
		DestroyWindow( m_hWnd );
	}
	return m_nCmdRet;
}

BOOL CUIPopMenu::CreateMenu( HWND hParent )
{
	WNDCLASS wndcls;
	if (!(::GetClassInfo((HINSTANCE)::GetModuleHandle(NULL), _T("UI_MENU"), &wndcls)))//| CS_HREDRAW | CS_DROPSHADOW |CS_VREDRAW
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS  |  CS_SAVEBITS;
		wndcls.lpfnWndProc      = CUIPopMenu::WinProcess;
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
		WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, 0, 0, hParent, NULL,\
		(HINSTANCE)::GetModuleHandle(NULL), NULL );
	m_itemsRect.clear();
	if ( m_hWnd )
	{
		IPainter* painter = UICreateWndPainter( m_hWnd );
		HDC hHelpDC = GetDC( m_hWnd );
		HFONT hOld = (HFONT)SelectObject( hHelpDC, m_font );
		RECT rectItem = { 0,0,1,1 };
		int nMaxWidth = 0;
		RectF rcItemF;
		Margin* pMargin = &m_margin;
		if ( m_parentMenu
			&& m_pSubBkGnd )
		{
			pMargin = &m_subMargin;
		}
		//计算每个菜单项的矩形区域
		for ( int _i = 0; _i < m_nMenuCount; ++_i )
		{
			rectItem.top = 0;
			rectItem.bottom = 1000;
			rectItem.left = 0;
			rectItem.right = 1000;
			if( m_pMenuData[_i].nCmdId > 0 )
			{ 
				if ( m_pMenuData[_i].pszText[0] != 0 )
				{
					if ( m_bLayered )
					{
						painter->MeasureString( m_pMenuData[_i].pszText, -1, &rectItem, m_font, XWS_ALIGNLEFT );
					}
					else
					{
						DrawText( hHelpDC, m_pMenuData[_i].pszText, (int)_tcslen(m_pMenuData[_i].pszText), &rectItem,\
							DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_CALCRECT );
					}
				}
				else
				{
					rectItem.right = 1;
				}
				rectItem.bottom = max((int)m_uItemHeight, RECT_H(rectItem));
				m_uItemHeight = rectItem.bottom;
			}
			else
			{
				if ( m_pSeparator )
				{
					if ( m_pSeparator->hImage )
					{
						ImageInfo info = {0};
						m_pSeparator->hImage->GetImageInfo( &info );
						rectItem.bottom = info.Height;
					}
					else
					{
						rectItem.bottom = 1;
					}
				}
				rectItem.right = 1;
			}
			if ( RECT_W(rectItem) > nMaxWidth )
			{
				nMaxWidth = RECT_W(rectItem);
			}
			if ( m_itemsRect.size() )
			{
				RECT_TO( rectItem, m_itemsRect.back().left, m_itemsRect.back().bottom );
			}
			else
			{
				RECT_TO( rectItem, pMargin->left, pMargin->top );
			}
			m_itemsRect.push_back( rectItem );
		}
		int nSubArrow = 0;
		if ( m_pSubArrow )
		{
			ImageInfo info = {0};
			m_pSubArrow->GetImageInfo( &info );
			nSubArrow = info.Width+2*ARROW_BORDER;
		}
		for ( int _j = 0; _j < (int)m_itemsRect.size(); ++_j )
		{
			m_itemsRect.at( _j ).right = m_itemsRect.at( _j ).left+nMaxWidth+m_uIconWidth+nSubArrow+m_itemOffset;
		}
		::SelectObject( hHelpDC, hOld );
		ReleaseDC( m_hWnd, hHelpDC );
		UIDestroyPainter( painter );
		rectItem = m_itemsRect.back();
		SetWindowPos( m_hWnd, NULL, 0, 0, m_itemsRect.back().right+pMargin->right, m_itemsRect.back().bottom+pMargin->bottom, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_HIDEWINDOW );
		if ( m_bLayered )
		{
			InvalidRect( NULL );
		}
		m_mapWindows[m_hWnd] = this;
	}
	return m_hWnd != NULL;
}

void CUIPopMenu::DrawMenu( IPainter* pPainter, LPCRECT lpRectClip /*= NULL*/ )
{
	if ( m_itemsRect.empty() )
	{
		return;
	}
	RECT rcIcon = {0};
	int nSubArrow = 0;
	if ( m_pSubArrow )
	{
		ImageInfo info = {0};
		m_pSubArrow->GetImageInfo( &info );
		nSubArrow = info.Width+2*ARROW_BORDER;
	}
	if ( m_bLayered )
	{
		StringFormat strformat;
		ARGB argbOld = pPainter->SetBrushColor( Color(255, 55, 140, 210).GetValue() );
		RECT rect;
		DrawBkGnd( pPainter );
		if ( m_nMouseIn >= 0
			&& m_pMenuData[m_nMouseIn].nCmdId > 0
			&& m_pMenuData[m_nMouseIn].bEnable
			&& ::IntersectRect( &rect, lpRectClip, &m_itemsRect[m_nMouseIn] )
			&& !RECT_ISEMPTY(rect) )
		{
			if ( m_pBkItem )
			{
				if ( m_pBkItem->hImage )
				{
					pPainter->DrawImage( m_pBkItem, &m_itemsRect.at(m_nMouseIn) );
				}
				else
				{
					ARGB argbOld1 = pPainter->SetBrushColor( m_pBkItem->imageAvgClr );
					pPainter->FillRectangle( m_itemsRect.at(m_nMouseIn).left, m_itemsRect.at(m_nMouseIn).top, RECT_W(m_itemsRect.at(m_nMouseIn)), RECT_H(m_itemsRect.at(m_nMouseIn)) );
					pPainter->SetBrushColor( argbOld1 );
				}
			}
			else
			{
				ARGB argbOld1 = pPainter->SetBrushColor( Color(254, 55, 140, 210).GetValue() );
				pPainter->FillRectangle( m_itemsRect.at(m_nMouseIn).left, m_itemsRect.at(m_nMouseIn).top, \
					RECT_W(m_itemsRect.at(m_nMouseIn)), RECT_H(m_itemsRect.at(m_nMouseIn)) );
				pPainter->SetBrushColor( argbOld1 );
			}
		}
		for ( int _i = 0; _i < m_nMenuCount; ++_i )
		{
			::IntersectRect( &rect, lpRectClip, &m_itemsRect[_i] );
			if( !RECT_ISEMPTY( rect ) )
			{ 
				if ( m_pMenuData[_i].nCmdId > 0 )
				{
					if ( m_pMenuData[_i].pImage )
					{
						ImageInfo info = {0};
						m_pMenuData[_i].pImage->GetImageInfo( &info );
						rcIcon.top = m_itemsRect[_i].top+((m_uItemHeight-info.Height)>>1);
						if ( (m_uItemHeight-info.Height)%2 )
						{
							++rcIcon.top;
						}
						if ( m_uDrawMask & MDM_ICON_LEFT )
						{
							rcIcon.left = m_itemsRect[_i].left;
						}
						else if ( m_uDrawMask & MDM_ICON_LEFT )
						{
							rcIcon.left = m_itemsRect[_i].right - info.Width;
						}
						else
						{
							rcIcon.left= m_itemsRect[_i].left+((m_uIconWidth-info.Width)>>1);
							if ( (m_uIconWidth-info.Width)%2 )
							{
								++rcIcon.left;
							}
						}
						rcIcon.right = rcIcon.left + info.Width;
						rcIcon.bottom = rcIcon.top + info.Height;
						IMG img;
						if ( m_pMenuData[_i].bEnable )
						{
							img.hImage.Attach(m_pMenuData[_i].pImage);
							pPainter->DrawImage( &img, &rcIcon );
							img.hImage.Detach();
						}
						else
						{
							img.hImage = GetGrayImage( m_pMenuData[_i].pImage );
							pPainter->DrawImage( &img, &rcIcon );
						}
					}
					if ( m_pMenuData[_i].pszText[0] != 0 )
					{
 						ARGB clrOld = X_TRANSPARENT;
  						if ( m_pMenuData[_i].bEnable )
  						{
  							clrOld = pPainter->SetBrushColor( Color( m_clrFont ).GetValue() );
  						}
  						else
  						{
  							clrOld = pPainter->SetBrushColor( Color( m_clrGray ).GetValue() );
  						}
 						UINT nFormat = XWS_ALIGNVCENTER|(m_uDrawMask&MDM_PREFIX_NONE?XWS_NOPREFIX:\
 							m_uDrawMask&MDM_PREFIX_HIDE?XWS_PREFIXHIDE:XWS_PREFIXSHOW);
 						RECT rcDraw = m_itemsRect.at(_i);
 						rcDraw.left += m_uIconWidth+m_itemOffset;
 						pPainter->DrawString( m_pMenuData[_i].pszText, -1,\
 							&rcDraw, m_font, nFormat );
 						pPainter->SetBrushColor( clrOld );
					}
					if( m_pMenuData[_i].pSubMenu
						&& NULL != m_pSubArrow )
					{
						ImageInfo info = {0};
						m_pSubArrow->GetImageInfo( &info );
						rcIcon.left = m_itemsRect[_i].right-nSubArrow+ARROW_BORDER;
						rcIcon.top = m_itemsRect[_i].top+((m_uItemHeight-info.Height)>>1);
						rcIcon.right = rcIcon.left+info.Width;
						rcIcon.bottom  = rcIcon.top+info.Height;
						IMG img;
						if ( m_pMenuData[_i].bEnable )
						{
							img.hImage.Attach(m_pSubArrow);
							pPainter->DrawImage( &img, &rcIcon );
							img.hImage.Detach();
						}
						else
						{
							img.hImage = GetGrayImage( m_pSubArrow );
							pPainter->DrawImage( &img, &rcIcon );
						}

					}
				}
				else
				{
					if ( m_pSeparator )
					{
						rect = m_itemsRect.at(_i);
						if ( !(m_uDrawMask & MDM_SEPARATOR_FULLITEM) )
						{
							rect.left += m_uIconWidth;
						}
						if ( m_pSeparator->hImage )
						{
							pPainter->DrawImage( m_pSeparator, &rect );
						}
						else if ( m_pSeparator->imageAvgClr != X_TRANSPARENT )
						{
							HPen pen = UICreateSolidPen( m_pSeparator->imageAvgClr, RECT_H(rect) );
							HPen hOldPen = pPainter->SetPen( pen );
							pPainter->DrawLine( rect.left, rect.top, rect.right, rect.top );
							pPainter->SetPen( hOldPen );
							UIDestroyPen( pen );
						}
					}
				}
			}
		}
	}
	else
	{
		RECT rect;
		GetClientRect( m_hWnd, &rect );
		DrawBkGnd( pPainter );
		if ( m_nMouseIn >= 0
			&& m_pMenuData[m_nMouseIn].nCmdId > 0
			&& ::IntersectRect( &rect, lpRectClip, &m_itemsRect[m_nMouseIn] )
			&& !RECT_ISEMPTY(rect) )
		{
			if ( m_pBkItem )
			{
				if ( m_pBkItem->hImage )
				{
					pPainter->DrawImage( m_pBkItem, &m_itemsRect.at(m_nMouseIn) );
				}
				else
				{
					HBrush hBrush = UICreateSolidBrush( m_pBkItem->imageAvgClr );
					HBrush hOld = pPainter->SetBrush( hBrush );
					pPainter->FillRectangle( m_itemsRect.at(m_nMouseIn).left, m_itemsRect.at(m_nMouseIn).top, RECT_W(m_itemsRect.at(m_nMouseIn)), RECT_H(m_itemsRect.at(m_nMouseIn)) );
					pPainter->SetBrush( hOld );
					UIDestroyBrush( hBrush );
				}
			}
			else
			{
				SolidBrush brushBk( Color(255, 55, 140, 210) );
				HBrush hBrush = UICreateSolidBrush( Color(255, 55, 140, 210).GetValue() );
				HBrush hOld = pPainter->SetBrush( hBrush );
				pPainter->FillRectangle( m_itemsRect.at(m_nMouseIn).left, m_itemsRect.at(m_nMouseIn).top, \
					RECT_W(m_itemsRect.at(m_nMouseIn)), RECT_H(m_itemsRect.at(m_nMouseIn)) );
				pPainter->SetBrush( hOld );
				UIDestroyBrush( hBrush );
			}
		}

		assert( m_font );
		for ( int _i = 0; _i < m_nMenuCount; ++_i )
		{
			if( m_pMenuData[_i].nCmdId > 0 )
			{ 
				if ( m_pMenuData[_i].pImage )
				{
					ImageInfo info = {0};
					m_pMenuData[_i].pImage->GetImageInfo( &info );
					rcIcon.top = m_itemsRect[_i].top+((m_uItemHeight-info.Height)>>1);
					if ( (m_uItemHeight-info.Height)%2 )
					{
						++rcIcon.top;
					}
					if ( m_uDrawMask & MDM_ICON_LEFT )
					{
						rcIcon.left = m_itemsRect[_i].left;
					}
					else if ( m_uDrawMask & MDM_ICON_LEFT )
					{
						rcIcon.left = m_itemsRect[_i].right - info.Width;
					}
					else
					{
						rcIcon.left= m_itemsRect[_i].left+((m_uIconWidth-info.Width)>>1);
						if ( (m_uIconWidth-info.Width)%2 )
						{
							++rcIcon.left;
						}
					}
					rcIcon.right = rcIcon.left + info.Width;
					rcIcon.bottom = rcIcon.top + info.Height;
					IMG img;
					img.hImage.Attach( m_pMenuData[_i].pImage );
					pPainter->DrawImage( &img, &rcIcon );
					img.hImage.Detach();
				}
				if ( m_pMenuData[_i].pszText[0] != 0 )
				{
					UINT nFormat = XWS_ALIGNVCENTER|XWS_ALIGNLEFT|XWS_SINGLELINE|\
						(m_uDrawMask&MDM_PREFIX_NONE?XWS_NOPREFIX:\
						m_uDrawMask&MDM_PREFIX_HIDE?XWS_PREFIXHIDE:XWS_PREFIXSHOW);
					ARGB clrOld = X_TRANSPARENT;
					if ( m_pMenuData[_i].bEnable )
					{
						clrOld = pPainter->SetBrushColor( Color( m_clrFont ).GetValue() );
					}
					else
					{
						clrOld = pPainter->SetBrushColor( Color( m_clrGray ).GetValue() );
					}
					RECT rcDraw = m_itemsRect.at(_i);
					rcDraw.left += m_uIconWidth+m_itemOffset;
					pPainter->DrawString( m_pMenuData[_i].pszText, -1,\
						&rcDraw, m_font, nFormat );
					pPainter->SetBrushColor( clrOld );
				}
				if( m_pMenuData[_i].pSubMenu
					&& NULL != m_pSubArrow )
				{
					ImageInfo info = {0};
					m_pSubArrow->GetImageInfo( &info );
					rcIcon.left = m_itemsRect[_i].right -nSubArrow+ ARROW_BORDER;
					rcIcon.top = m_itemsRect[_i].top+((m_uItemHeight-info.Height)>>1);
					rcIcon.right = rcIcon.left+info.Width;
					rcIcon.bottom  = rcIcon.top+info.Height;
					IMG img;
					img.hImage.Attach( m_pSubArrow );
					pPainter->DrawImage( &img, &rcIcon );
					img.hImage.Detach();
				}
			}
			else 
			{
				if ( m_pSeparator )
				{
					rect = m_itemsRect.at(_i);
					if ( !(m_uDrawMask & MDM_SEPARATOR_FULLITEM) )
					{
						rect.left += m_uIconWidth;
					}
					if ( m_pSeparator->hImage )
					{
						pPainter->DrawImage( m_pSeparator, &rect );
					}
					else if( m_pSeparator->imageAvgClr != X_TRANSPARENT )
					{
						HPen pen = UICreateSolidPen( m_pSeparator->imageAvgClr, RECT_H(rect) );
						HPen hOldPen = pPainter->SetPen( pen );
						pPainter->DrawLine( rect.left, rect.top, rect.right, rect.top );
						pPainter->SetPen( hOldPen );
						UIDestroyPen( pen );
					}
				}
			}
		}
	}
}

LRESULT WINAPI CUIPopMenu::WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CUIPopMenu* pPop = CUIPopMenu::FromHandle( hWnd );
	if ( pPop )
	{
		return pPop->WinProc( message, wParam, lParam );
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT CUIPopMenu::WinProc( UINT message, WPARAM wParam, LPARAM lParam )
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
			HDC hDC = ::BeginPaint( m_hWnd, &ps );
			if ( !m_bufferDC )
			{
				m_bufferDC.GetDC( hDC );
			}
			if ( hDC )
			{
				RECT rcClip;
				GetClipBox( hDC, &rcClip );
				if ( RECT_ISEMPTY( rcClip ) )
				{
					break;
				}
				if ( NULL == m_hBufferBmp )
				{
					RECT rcWnd;
					GetWindowRect( m_hWnd, &rcWnd );
					ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );
					bmih.biSize = sizeof (BITMAPINFOHEADER) ;
					bmih.biWidth = RECT_W(rcWnd);
					bmih.biHeight = RECT_H(rcWnd) ;
					bmih.biPlanes = 1 ;
					bmih.biBitCount = 32; //这里一定要是32
					bmih.biCompression = BI_RGB ;
					bmih.biSizeImage = 0 ;
					bmih.biXPelsPerMeter = 0 ;
					bmih.biYPelsPerMeter = 0 ;
					bmih.biClrUsed = 0 ;
					bmih.biClrImportant = 0 ;
					BYTE* pBits = NULL;
					m_hBufferBmp = CreateDIBSection (NULL, (BITMAPINFO *) &bmih, DIB_RGB_COLORS, (void**)&pBits, NULL, 0) ;
					if ( m_hBufferBmp )
					{
						m_hOldBufferBmp = (HBITMAP)SelectObject( m_bufferDC, m_hBufferBmp );
					}
				}
				//画菜单
				IPainter* painter = UICreateDCPainter( m_bufferDC, true );
				if ( painter )
				{
					DrawMenu( painter, &rcClip );
					UIDestroyPainter( painter );
					painter = NULL;
				}
				BitBlt( hDC, rcClip.left, rcClip.top, RECT_W(rcClip), RECT_H(rcClip),  m_bufferDC, rcClip.left, rcClip.top, SRCCOPY );
				::EndPaint( m_hWnd, &ps );
				return TRUE;
			}
		}
	case WM_MOUSEMOVE:
		{

			POINT point = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
			for ( int _i = 0; _i < (int)m_itemsRect.size(); ++_i )
			{
				if ( ::PtInRect( &m_itemsRect[_i], point ) 
					&& m_nMouseIn != _i )
				{
					int nPre = m_nMouseIn;
					m_nMouseIn = _i;
					if ( !m_bLayered )
					{
						RECT rcInvalidate = m_itemsRect[_i];
						if ( nPre >= 0 )
						{
							UnionRect( &rcInvalidate, &m_itemsRect[_i], &m_itemsRect[nPre] );
						}
						InvalidRect( &rcInvalidate );
						ShowSubMenu();
						break;
					}
					ShowSubMenu();
					InvalidRect( NULL );
					break;
				}
			}
			if ( m_bLayered )
			{
				RECT rcClient;
				GetClientRect( m_hWnd, &rcClient );
				if ( ::PtInRect( &rcClient, point ) )
				{
					if ( GetCapture() != m_hWnd )
					{
						SetCapture( m_hWnd );
					}
				}
				else
				{
					if ( GetCapture() == m_hWnd )
					{
						ReleaseCapture();
					}
					int nPre = m_nMouseIn;
					m_nMouseIn = -1;
					if( nPre >= 0 )
					{
						InvalidRect( NULL );
					}
				}
				break;
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
			if ( m_bufferDC )
			{
				if ( m_hBufferBmp )
				{
					::SelectObject( m_bufferDC, m_hOldBufferBmp );
					HDC hdc = GetDC( m_hWnd );
					if ( hdc )
					{
						RECT rcWnd;
						GetWindowRect( m_hWnd, &rcWnd );
						m_hBufferBmp = ::CreateCompatibleBitmap( hdc, RECT_W(rcWnd), RECT_H(rcWnd) );
						m_hOldBufferBmp = (HBITMAP)::SelectObject( m_bufferDC, m_hBufferBmp );
					}
					ReleaseDC( m_hWnd, hdc );
				}
			}
		}
		break;
	case WM_KEYDOWN:
		{
 			UINT nCmd = HotkeyTest( (TCHAR)wParam );
 			if ( nCmd > 0 )
 			{
				for ( int _i = 0; _i < m_nMenuCount; ++_i )
				{
					if ( m_pMenuData[_i].nCmdId == nCmd )
					{
						if ( m_pMenuData[_i].bEnable )
						{
							if ( m_pMenuData[_i].pSubMenu == NULL  )
							{
								m_nCmdRet = nCmd; 
								EndMenu( false );
								break;
							}
							m_nMouseIn = _i;
							ShowSubMenu();
						}
						break;
					}
				}
 				break;
 			}
			if ( VK_UP == wParam )
			{
				do
				{
					--m_nMouseIn;
					if( m_nMouseIn < 0 )
					{
						m_nMouseIn = m_nMenuCount-1;
						break;
					}
					if ( m_pMenuData[m_nMouseIn].nCmdId > 0 )
					{
						break;
					}
				}while( 1 );
				InvalidRect( NULL );
			}
			else if ( VK_DOWN == wParam
				|| VK_TAB == wParam )
			{
				do 
				{
					++m_nMouseIn;
					if( m_nMouseIn > 0 )
					{
						m_nMouseIn %= m_nMenuCount; 
					}
					else
					{
						m_nMouseIn = 0;
						break;
					}
					if ( m_nMouseIn == 0
						|| m_pMenuData[m_nMouseIn].nCmdId > 0 )
					{
						break;
					}
				} while ( 1 );
				
				InvalidRect( NULL );
			}
			else if ( VK_RIGHT == wParam )
			{
				ShowSubMenu(  );
			}
			else if ( VK_LEFT == wParam )
			{
				if ( m_parentMenu )
				{
					EndMenu( true );
				}
			}
			else if ( VK_RETURN == wParam )
			{
				if ( m_nMouseIn >= 0 
					&& m_pMenuData[m_nMouseIn].bEnable )
				{
					if ( m_pMenuData[m_nMouseIn].nCmdId > 0
						&& m_pMenuData[m_nMouseIn].pSubMenu )
					{
						ShowSubMenu();
						break;
					}
					m_nCmdRet = m_pMenuData[m_nMouseIn].nCmdId;
				}
				else
				{
					m_nCmdRet = 0;
				}
				EndMenu();
			}
			else if ( VK_ESCAPE == wParam )
			{
				m_nCmdRet = 0;
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
				EndMenu( false ); 
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
					if(  m_pMenuData[_i].nCmdId > 0
						&& m_pMenuData[_i].pSubMenu == NULL 
						&& m_pMenuData[m_nMouseIn].bEnable )
					{
						m_nCmdRet = m_pMenuData[_i].nCmdId;
						EndMenu( false );
						break;
					}
					break;
				}
			}
		}
		break;
	case WM_MOUSELEAVE:
		{
			int nPre = m_nMouseIn;
			m_nMouseIn = -1;
			if ( nPre >= 0 )
			{
				if ( !m_bLayered )
				{
					InvalidRect( &m_itemsRect[nPre] );
					break;
				}
				InvalidRect( NULL );
			}
		}
		break;
	case WM_COMMAND:
		{
			m_nCmdRet = LOWORD(wParam); 
			EndMenu( false );
		}
		break;
	case WM_KILLFOCUS:
		{
 			if ( pTop )
 			{
 				EndMenu( false );
 			}
		}
		break;
	default:
		break;
	}
	return DefWindowProc( m_hWnd, message, wParam, lParam );
}

void CUIPopMenu::InvalidRect( LPRECT lpRect )
{
	if ( !m_bLayered )
	{
		::InvalidateRect( m_hWnd, lpRect, FALSE );
	}
	else
	{
		BLENDFUNCTION blend;
		blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
		blend.BlendFlags=0; //nothingelseisspecial...
		blend.AlphaFormat=1; //...
		blend.SourceConstantAlpha=m_uAlpha%256;//AC_SRC_ALPHA
		RECT rcThis;
		GetWindowRect( m_hWnd, &rcThis );
		POINT ptWinPos={rcThis.left,rcThis.top};
		::ScreenToClient( m_hWnd, (LPPOINT)&rcThis );
		::ScreenToClient( m_hWnd, ((LPPOINT)&rcThis)+1 );
		LPRECT lpClip = lpRect;
		if ( NULL == lpClip )
		{ 
			lpClip = &rcThis;
		}
		if ( !m_bufferDC )
		{
			HDC hdcThis = GetDC( m_hWnd );
			m_bufferDC.GetDC( hdcThis );
			RECT rcWnd;
			GetWindowRect( m_hWnd, &rcWnd );
			BYTE * pBits = NULL ;
			ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );
			bmih.biSize = sizeof (BITMAPINFOHEADER) ;
			bmih.biWidth = RECT_W(rcWnd);
			bmih.biHeight = RECT_H(rcWnd) ;
			bmih.biPlanes = 1 ;
			bmih.biBitCount = 32; //这里一定要是32
			bmih.biCompression = BI_RGB ;
			bmih.biSizeImage = 0 ;
			bmih.biXPelsPerMeter = 0 ;
			bmih.biYPelsPerMeter = 0 ;
			bmih.biClrUsed = 0 ;
			bmih.biClrImportant = 0 ;
			m_hBufferBmp = CreateDIBSection (NULL, (BITMAPINFO *) &bmih, DIB_RGB_COLORS, (void**)&pBits, NULL, 0) ;
			if ( m_hBufferBmp )
			{
				m_hOldBufferBmp = (HBITMAP)SelectObject( m_bufferDC, m_hBufferBmp );
			}
			ReleaseDC( m_hWnd, hdcThis );
		}
		IPainter* pPanter = UICreateDCPainter( m_bufferDC, true );
		DrawMenu( pPanter, lpClip );
		HDC hdcScreen=::GetDC (m_hWnd);
		SIZE sizeWindow={RECT_W(rcThis),RECT_H(rcThis)};
		POINT ptSrc={0,0};
		DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
		if((dwExStyle&0x80000)!=0x80000)
			SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);
 		UpdateLayeredWindow( m_hWnd,hdcScreen,&ptWinPos,
 			&sizeWindow,m_bufferDC,&ptSrc,0,&blend,ULW_ALPHA);
		::ReleaseDC(m_hWnd,hdcScreen);
		hdcScreen=NULL;
	}
}

void CUIPopMenu::DrawBkGnd( IPainter* painter )
{
	IMG* pBk = m_pBkGnd;
	if ( m_parentMenu
		&& m_pSubBkGnd )
	{
		pBk = m_pSubBkGnd;
	}
	//画背景
	RECT rcThis;
	GetClientRect( m_hWnd, &rcThis );
	if ( m_pBkGnd )
	{
		if ( pBk->hImage )
		{
			painter->DrawImage( pBk, &rcThis );
		}
		else
		{
			HBrush hBrush = UICreateSolidBrush( pBk->imageAvgClr );
			HBrush hOld = painter->SetBrush( hBrush );
			painter->FillRectangle( rcThis.left, rcThis.top, RECT_W(rcThis), RECT_H(rcThis) );
			painter->SetBrush( hOld );
			UIDestroyBrush( hBrush );
		}
	}
	else
	{
		HBrush hBrush = UICreateSolidBrush( 0xFEFFFFFF );
		HBrush hOld = painter->SetBrush( hBrush );
		painter->FillRectangle(rcThis.left, rcThis.top, RECT_W(rcThis), RECT_H(rcThis) );
		painter->SetBrush( hOld );
	}
}

void CUIPopMenu::EndMenu( bool bOnlyFree )
{
	if ( m_subMenu )
	{
		m_subMenu->EndMenu( true );
	}
	if ( !pTop )
	{
		pSingleMenu = NULL;
		return;
	}
	pCurMenu = m_parentMenu;
	UINT nRet = m_nCmdRet;
	if ( pTop != this )
	{	
		m_itemsRect.clear();
		if ( m_parentMenu )
		{
			m_parentMenu->m_subMenu = NULL;
		}
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
		m_hWnd = NULL;
		delete this;
	}
	else
	{
		m_bPoping = FALSE;
		if (m_hWnd != NULL)
		{
			SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
		return;
	}
	if ( !bOnlyFree )
	{
		if ( pTop )
		{
			pTop->m_nCmdRet = nRet;
			pTop->EndMenu( true );
			
		}
	}
}

void CUIPopMenu::ShowSubMenu(  )
{
	if ( m_nMouseIn >= 0 )
	{
		if ( m_pMenuData[m_nMouseIn].nCmdId > 0
			&& m_pMenuData[m_nMouseIn].pSubMenu
			&& m_pMenuData[m_nMouseIn].bEnable )
		{
			if ( m_subMenu )
			{
				if ( m_subMenu->m_parentIndex == m_nMouseIn )
				{
					return;
				}
				m_subMenu->EndMenu( true );
			}
			try
			{
				CUIPopMenu* pMenu = new CUIPopMenu();
				pMenu->m_pMenuData = m_pMenuData[m_nMouseIn].pSubMenu;
				pMenu->m_pAccel = m_pMenuData[m_nMouseIn].pSubAccel;
				pMenu->m_nMenuCount = m_pMenuData[m_nMouseIn].nSubCount;
				pMenu->m_nAccelCount = m_pMenuData[m_nMouseIn].nSubAccelcount;
				pMenu->m_fuFlags = m_fuFlags;
				pMenu->m_parentMenu = this;
				m_subMenu = pMenu;
				pMenu->m_parentIndex = m_nMouseIn;
				if( !pMenu->CreateMenu( m_hWnd ) )
				{
					m_subMenu = NULL;
					delete pMenu;
					pMenu = NULL;
					return;
				}

				pMenu->GetTopLeft( pMenu->m_orgX, pMenu->m_orgY, m_fuFlags );
				if ( m_pAccel )
				{
					m_hAccel = CreateAcceleratorTable( pMenu->m_pAccel, pMenu->m_nAccelCount );
				}
				//设置顶层窗口
				SetWindowPos(pMenu->m_hWnd, HWND_TOPMOST, pMenu->m_orgX, pMenu->m_orgY, 0, 0, SWP_SHOWWINDOW|
					SWP_NOSIZE|SWP_NOACTIVATE);
			}
			catch( ... )
			{

			}
		}
		else
		{
			if ( m_subMenu )
			{
				m_subMenu->EndMenu( true );
			}
		}
	}
}

void CUIPopMenu::GetTopLeft( int &x, int& y, UINT fuFlags )
{
	RECT rcThis = {0};
	RECT rcParentItem = {0};
	GetWindowRect( m_hWnd, &rcThis );
	if ( m_parentMenu )
	{	
		rcParentItem = m_parentMenu->m_itemsRect[m_parentMenu->m_nMouseIn];
		RECT_TO( rcParentItem, m_parentMenu->m_orgX+rcParentItem.left, m_parentMenu->m_orgY+rcParentItem.top );
		x = rcParentItem.right;	
		y = rcParentItem.top;
		RECT_TO( rcThis, x, y );
	}
	else
	{
		if ( fuFlags & MAL_LEFT )
		{
			RECT_TOX( rcThis, x-RECT_W(rcThis));
		}
		else 
		{
			RECT_TOX( rcThis, x);	
		}

		if ( fuFlags & MAL_TOP )
		{
			RECT_TOY( rcThis, y-RECT_H(rcThis) );
		}
		else
		{
			RECT_TOY( rcThis, y );
		}
	}


	int nXScreen = GetSystemMetrics( SM_CXSCREEN );
	int nYScreen = GetSystemMetrics( SM_CYSCREEN );
	if ( rcThis.right > nXScreen - SCREEN_BORDER  )
	{
		RECT_TOX( rcThis, rcThis.left-RECT_W(rcThis)-RECT_W(rcParentItem) );
		if ( rcThis.left < SCREEN_BORDER )
		{
			RECT_TOX( rcThis, SCREEN_BORDER );
		}
	}
	else if ( rcThis.left < SCREEN_BORDER )
	{
		RECT_TOX( rcThis, x );
		if ( rcThis.right > nXScreen-SCREEN_BORDER )
		{
			RECT_TOX( rcThis, SCREEN_BORDER );
		}
	}
	if ( rcThis.bottom > nYScreen - SCREEN_BORDER )
	{
		RECT_TOY( rcThis, rcThis.top - RECT_H(rcThis)+RECT_H(rcParentItem) );
		if ( rcThis.top < SCREEN_BORDER )
		{
			RECT_TOY( rcThis, SCREEN_BORDER);
		}
	}
	else if ( rcThis.top < SCREEN_BORDER )
	{
		RECT_TOY( rcThis, y );
		if ( rcThis.bottom > nYScreen-SCREEN_BORDER )
		{
			RECT_TOY( rcThis, SCREEN_BORDER);
		}
	}
	x = rcThis.left;
	y = rcThis.top;
}