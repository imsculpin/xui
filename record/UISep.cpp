// UIWnd.cpp : 实现文件
#include "UIWnd.h"
#include "UIText.h"
#include ".\xwnd.h"
#include ".\xsep.h"
// CXSep
// BOOL CXSep::Create( IN DWORD dwStyle, IN const RECT& rect, IN CWnd* pParentWnd, IN UINT32 nID )
// {
// 	if ( ((dwStyle & SEP_VERT)
// 		&& (dwStyle & SEP_HORZ))
// 		|| !( (dwStyle & SEP_VERT)
// 		|| (dwStyle & SEP_HORZ) ))
// 	{
// 		OutputDebugString( _T("必须指定Sep控件样式的类型,但不能同时指定 WS_SEP_VERT和SEP_HORZ两种样式") );
// 		return FALSE;
// 	}
// 	if ( NULL == pParentWnd )
// 	{
// 		OutputDebugString( _T("必须指定Sep控件的父窗口") );
// 		return FALSE;
// 	}
// 	m_dwStyle = dwStyle;
// 	dwStyle &= ~(SEP_HORZ|WS_SEP_VERT);
// 	if ( m_dwStyle & WS_SEP_VERT )
// 	{
// 		m_hCursor = LoadCursor( NULL, IDC_SIZEWE );
// 	}
// 	else
// 	{
// 		m_hCursor = LoadCursor( NULL, IDC_SIZENS );
// 	}
// 	return __super::Create( _T("X_SEP"), _T(""), dwStyle, rect, pParentWnd, nID );
// }

BOOL CXSep::OnLButtonDown( CPoint point )
{
	SetCapture();
	m_bLButtonDown = true;
	m_clickPoint = point;
	ClientToScreen( &m_clickPoint );
	__super::OnLButtonDown(point);
}

BOOL CXSep::OnLClick( CPoint point )
{
	m_bLButtonDown = false;
	ReleaseCapture();
	m_clickPoint.SetPoint( 0, 0 );
	__super::OnLClick(point);
}

BOOL CXSep::OnMouseMove( CPoint point)
{
	if ( m_bLButtonDown 
		&& point != m_clickPoint )
	{
		CRect rcSep;
		GetWindowRect( rcSep );
		ClientToScreen( &point );
		CRect rcParent;
		GetParent()->GetClientRect( &rcParent );
		GetParent()->ClientToScreen( &rcParent );
		if ( rcParent.PtInRect( point ) )
		{
			if ( m_dwStyle & SEP_HORZ )
			{
				if( abs(point.y - m_clickPoint.y) > 8 )
				{
					if ( m_pOwnerPanel
						&&m_pOwnerPanel->GetOwner()
						&&m_pOwnerPanel->GetOwner()->GetSafeHwnd() )
					{
						rcSep.MoveToY( rcSep.top+point.y - m_clickPoint.y );
						GetParent()->ScreenToClient( &rcSep );
						_OnNotifyDlg( UNM_SEP_DRAGING, (LPARAM)(LONG_PTR)&rcSep );
					}
					GetParent()->SendMessage( WM_XCTRL_MESSAGE, GetDlgCtrlID(), (LONG)(LONG_PTR)&rcSep );
					::SetWindowPos( GetSafeHwnd(), NULL, rcSep.left, rcSep.top, rcSep.Width(), rcSep.Height(), SWP_NOZORDER );
					m_clickPoint = point;
				}
			}
			else
			{
				if( abs(point.x - m_clickPoint.y) > 8 )
				{
					rcSep.MoveToX( rcSep.left+point.x - m_clickPoint.x );
					GetParent()->ScreenToClient( &rcSep );
					GetParent()->SendMessage( WM_XCTRL_MESSAGE, GetDlgCtrlID(), (LONG)(LONG_PTR)&rcSep );
					::SetWindowPos( GetSafeHwnd(), NULL, rcSep.left, rcSep.top, rcSep.Width(), rcSep.Height(), SWP_NOZORDER );
					m_clickPoint = point;
				}
			}
		}
		
	}
	//CUIWnd::OnMouseMove(nFlags, point);
}

BOOL CXSep::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( m_hCursor )
	{
		::SetCursor( m_hCursor );
		return TRUE;
	}
	return CUIWnd::OnSetCursor(pWnd, nHitTest, message);
}
