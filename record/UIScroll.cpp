
#include "UIScroll.h"
#include <WindowsX.h>
#include "UIView.h"
static INT_PTR  g_buttonRepeatTimer = -1;
static INT_PTR  g_flashScroll = -1;
static int		 g_flashingOffset = 0;
static CUIScroll* g_flashingScroll = NULL;
static int g_flashScrollStep = 10;
void CALLBACK ButtonRepeat( HWND, UINT, UINT_PTR, DWORD )
{
	IUIBase* pCtrl = UIGetCapture(  );
	if ( pCtrl )
	{
		POINT point;
		GetCursorPos( &point );
		if ( pCtrl->GetUIWindow()
			&& pCtrl->GetUIWindow()->GetHandle())
		{
			ScreenToClient( pCtrl->GetUIWindow()->GetHandle(), &point );
		}
		RECT rect;
		pCtrl->GetRect( rect );
		if ( PtInRect( &rect, point ) )
		{
			pCtrl->MouseLClick( point );
		}
	}
}

void CALLBACK FlashScroll( HWND, UINT, UINT_PTR, DWORD )
{
	if ( g_flashingScroll )
	{
		g_flashingScroll->SetPos( g_flashingScroll->GetPos() - g_flashingOffset );
		if( g_flashingOffset > 0 )
		{
			g_flashingOffset -= g_flashScrollStep;
			if ( g_flashingOffset < 0 )
			{
				g_flashingOffset = 0;
				KillTimer( NULL, g_flashScroll );
			}
		}
		else
		{
			g_flashingOffset += g_flashScrollStep;
			if ( g_flashingOffset > 0 )
			{
				g_flashingOffset = 0;
				KillTimer( NULL, g_flashScroll );
			}
		}
	}
}

void CUIScroll::OnDraw(  IPainter* painter  )
{
	if ( IsVisable() )
	{
		m_bar.OnDraw( painter );
		m_arrow1.OnDraw( painter );
		m_arrow2.OnDraw( painter );
		m_thumb.OnDraw( painter );
	}
}

void CUIScroll::SetRange( long min, long max )
{
	if( !m_parent 
		|| max < 0 )
	{
		return ;
	}
	m_scrollRange.m_min = min;
	m_scrollRange.m_max   = max;
	RECT rcParent;
	m_parent->GetRect( rcParent );
	if( RECT_ISEMPTY(rcParent) )
	{
		return;
	}
	if ( m_nbar == VERT )
	{
		int height = RECT_H(rcParent) - m_arrow1.GetSize().cy-m_arrow2.GetSize().cy;
		int nPageCount = m_scrollRange.Distance()/RECT_H(rcParent);
		if( nPageCount > 0 )
		{
			int thumbHeight = height/nPageCount;
			if ( m_scrollRange.Distance()%RECT_H(rcParent) )
			{
				++nPageCount;
				int thumbHTmp = height/nPageCount;
				thumbHeight -= (thumbHeight - thumbHTmp)*(m_scrollRange.Distance()%RECT_H(rcParent))/RECT_H(rcParent);
			}
			if( thumbHeight < m_miniThumb )
			{
				thumbHeight = m_miniThumb;
			}
			RECT rcthumb;
			m_thumb.GetRect( rcthumb );
			RECT_TO( rcthumb, rcthumb.left-m_rect.left, rcthumb.top-m_rect.top );
			rcthumb.bottom = rcthumb.top+thumbHeight;
			if ( m_nPos < m_scrollRange.m_min )
			{
				m_nPos = m_scrollRange.m_min;
			}
			if( m_scrollRange.Distance()-m_bar.GetSize().cy>0 )
			{
				RECT_TO( rcthumb, rcthumb.left, m_arrow1.GetSize().cy+m_nPos*(m_bar.GetSize().cy-m_arrow2.GetSize().cy- m_arrow1.GetSize().cy-\
					RECT_H(rcthumb))/(m_scrollRange.Distance()-m_bar.GetSize().cy));
				
			}
			else
			{
				RECT_TO( rcthumb, rcthumb.left, m_arrow1.GetSize().cy );
			}
			m_thumb.SetRect( rcthumb );
		}
	}
	SetPos( m_nPos );
}

void CUIScroll::SetPos( long nPos )
{
	if( !IsVisable())
	{
		return;
	}
	if ( nPos < m_scrollRange.m_min )
	{
		nPos = m_scrollRange.m_min;
	}

	long nPrePos = m_nPos;
	RECT rcThumb;
	RECT_S(rcThumb, 0, 0, m_thumb.GetSize().cx, m_thumb.GetSize().cy );
	if ( m_nbar == VERT )
	{
		if ( nPos > m_scrollRange.m_max-m_bar.GetSize().cy )
		{
			nPos = m_scrollRange.m_max-m_bar.GetSize().cy;
			nPos = max( 0, nPos );
		}
		if ( m_nPos == nPos )
		{
			if ( g_buttonRepeatTimer > 0 
				&& ( UIGetCapture() == &m_arrow1
				|| UIGetCapture() == &m_arrow2) )
			{
				KillTimer( NULL, g_buttonRepeatTimer );
				g_buttonRepeatTimer = -1;
			}
			return;
		}

		m_nPos = nPos;
		if ( m_scrollRange.Distance()-m_bar.GetSize().cy > 0 )
		{
			RECT_TO( rcThumb, rcThumb.left, m_arrow1.GetSize().cy+m_nPos*(m_bar.GetSize().cy-m_arrow2.GetSize().cy- m_arrow1.GetSize().cy-\
				RECT_H(rcThumb))/(m_scrollRange.Distance()-m_bar.GetSize().cy));
		}
		else
		{
			RECT_TO( rcThumb, rcThumb.left, m_arrow1.GetSize().cy );
		}
		m_thumb.SetRect( rcThumb );
		if ( g_buttonRepeatTimer > 0
			&& UIGetCapture() == &m_bar )
		{
			m_thumb.GetRect( rcThumb );
			POINT _point;
			GetCursorPos( &_point );
			ScreenToClient( GetUIWindow()->GetHandle(), &_point );
			if ( PtInRect( &rcThumb, _point ) )
			{
				KillTimer( NULL, g_buttonRepeatTimer );
				g_buttonRepeatTimer = -1;
			}
		}
		if ( nPrePos >= 0 )
		{		
			VertScroll( THUMB, m_nPos, nPrePos>m_nPos?-1:1 );
		}
	}
}

BOOL CUIScroll::MouseMoving( POINT pt )
{
	RECT rcArrow1;
	RECT rcArrow2;
	m_arrow1.GetRect( rcArrow1 );
	m_arrow2.GetRect( rcArrow2 );
	if ( UIGetCapture() == &m_thumb
		&& pt.y >= rcArrow1.bottom
		&& pt.y <= rcArrow2.top )
	{
		if ( VERT == m_nbar )
		{
			if ( m_bar.GetSize().cy-m_thumb.GetSize().cy-\
				m_arrow1.GetSize().cy-m_arrow2.GetSize().cy > 0 )
			{
				int nPos = (int)((m_scrollRange.Distance()-m_bar.GetSize().cy)*(float)(pt.y-m_pointLbDown.y)/(float)(m_bar.GetSize().cy-m_thumb.GetSize().cy-\
					m_arrow1.GetSize().cy-m_arrow2.GetSize().cy));
				SetPos( m_nPos+nPos );
			}
		}
		m_pointLbDown = pt;
	}
	if ( UIGetCapture() != &m_thumb )
	{
		IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
		if ( pXCtrl )
		{
			if ( m_pHot && m_pHot != pXCtrl )
			{
				m_pHot->MouseLeave();
			}
			m_pHot = pXCtrl;
			if( pXCtrl->MouseMoving( pt ) )
			{
				return TRUE;
			}
		}
		else
		{
			if ( m_pHot )
			{
				m_pHot->MouseLeave();
				m_pHot = NULL;
			}
		}
	}

	if ( m_pointLbDown.x >= 0 )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CUIScroll::Notify( UINT message, WPARAM wParam, LPARAM lParam )
{
	NM_PARAM* param = (NM_PARAM*)lParam;
	if ( !param )
	{
		return FALSE;
	}
	if ( XWM_LBDOWN == message )
	{
		g_buttonRepeatTimer = SetTimer( NULL, 0, 100, ButtonRepeat );
	}
	else if ( XWM_LBUP == message )
	{
		if ( g_buttonRepeatTimer > 0 )
		{
			KillTimer( NULL, g_buttonRepeatTimer );
		}
		g_buttonRepeatTimer = -1;
		UIReleaseCapture();
		m_pointLbDown.x = -1;
	}
	else if( message == XWM_LCLICK 
		|| message == XWM_LDBCLICK )
	{
		switch( param->nCtrlId )
		{
		case ID_ARROW1:
			{
				if ( VERT == m_nbar )
				{
					SetPos( m_nPos- m_scrollStep );
				}
			}
			break;
		case ID_ARROW2:
			{
				if ( VERT == m_nbar )
				{
					SetPos( m_nPos+m_scrollStep );
				}
			}
			break;
		case ID_BAR:
			{
				if ( VERT == m_nbar )
				{
					RECT rcThumb;
					m_thumb.GetRect( rcThumb );
					POINT _point;
					GetCursorPos( &_point );
					ScreenToClient( GetUIWindow()->GetHandle(), &_point );
					if ( PtInRect( &rcThumb, _point ) )
					{
						return TRUE;
					}
					if ( param->point.y > rcThumb.bottom )
					{
						SetPos( m_nPos+m_scrollStep );
					}
					else
					{
						SetPos( m_nPos-m_scrollStep );
					}
				}
			}
			break;
		default:
			break;
		}
	}
	else if ( param->nCtrlId == ID_THUMB )
	{
		if ( XWM_MOUSEMOVE == message 
			&& UIGetCapture() == &m_thumb )
		{
			MouseMoving( param->point );
		}
		else if ( XWM_LBUP == message )
		{
			m_pointLbDown.x = -1;
		}
	}
	return FALSE;
}

BOOL CUIScroll::Create( int bar , IUIBase* parent, UINT nCmdId )
{
	Show( false );
	m_nbar = bar;
	m_parent = parent;
	m_nCommandId = nCmdId;
	m_bar.Create( _T(""), RECT_C(0,0,20,20), ID_BAR );
	m_bar.SetParent( this );
	m_thumb.Create( _T(""), RECT_C(0,0,20,20), ID_THUMB );
	m_thumb.SetParent( this );
	m_arrow1.Create( _T(""), RECT_C(0,0,20,20), ID_ARROW1 );
	m_arrow1.SetParent( this );
	m_arrow2.Create( _T(""), RECT_C(0,0,20,20), ID_ARROW2 );
	m_arrow2.SetParent( this );
	m_bar.SetBkColor( RGB( 255,255,255 ) );
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
	OnSize();
	return TRUE;
}

void CUIScroll::OnSize()
{
	if ( m_parent )
	{
		RECT rcParent;
		m_parent->GetRect( rcParent );
		RECT rcTmp;
		if ( VERT == m_nbar )
		{	
			int nWidth = m_bar.GetSize().cx;
			if ( nWidth == 0 )
			{
				nWidth = m_thumb.GetSize().cx;
			}
			RECT_S(m_rect, rcParent.right-nWidth, rcParent.top, rcParent.right, rcParent.bottom );
			int X = 0;
			RECT_S(rcTmp, X, 0, X+nWidth, RECT_H(m_rect) );
			m_bar.SetRect( rcTmp );
			X = (m_bar.GetSize().cx-m_arrow1.GetSize().cx)>>1;
			RECT_S(rcTmp, X, 0, X+m_arrow1.GetSize().cx, m_arrow1.GetSize().cy);
			m_arrow1.SetRect( rcTmp );
			X = (m_bar.GetSize().cx-m_arrow2.GetSize().cx)>>1;
			RECT_S( rcTmp, X, m_bar.GetSize().cy-m_arrow2.GetSize().cy, X+m_arrow2.GetSize().cx, m_bar.GetSize().cy );
			m_arrow2.SetRect( rcTmp );
			X = (m_bar.GetSize().cx-m_thumb.GetSize().cx)>>1;
			RECT_S( rcTmp, X, m_arrow1.GetSize().cy, X+m_thumb.GetSize().cx, m_bar.GetSize().cy - m_arrow2.GetSize().cy );
			if( m_nPos >= 0
				&& m_scrollRange.Distance()-m_bar.GetSize().cy > 0 )
			{
				RECT_TO( rcTmp, rcTmp.left,m_arrow1.GetSize().cy+m_nPos*(m_bar.GetSize().cy-m_arrow2.GetSize().cy- m_arrow1.GetSize().cy-\
					m_thumb.GetSize().cy)/(m_scrollRange.Distance()-m_bar.GetSize().cy));
			}
			m_thumb.SetRect( rcTmp );
		}
		else
		{

		}
	}
}

BOOL CUIScroll::MouseLBDown( POINT pt )
{
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if ( pXCtrl->GetID() == ID_THUMB )
		{
			m_pointLbDown = pt;
			::UISetCapture(this);
		}
		if( pXCtrl->MouseLBDown( pt ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

IUICtrl* CUIScroll::GetXCtrlFromPoint( POINT pt )
{
	RECT rcTmp;
	m_arrow1.GetRect( rcTmp ); 
	if ( ::PtInRect( &rcTmp, pt ) )
	{
		return &m_arrow1;
	}
	m_arrow2.GetRect( rcTmp );
	if ( ::PtInRect( &rcTmp, pt ) )
	{
		return &m_arrow2;
	}
	m_thumb.GetRect( rcTmp );
	if ( ::PtInRect( &rcTmp, pt ) )
	{
		return &m_thumb;
	}
	m_bar.GetRect( rcTmp );
	if ( ::PtInRect( &rcTmp, pt ) )
	{
		return &m_bar;
	}
	MouseLeave();
	return NULL;
}

BOOL CUIScroll::MouseLBUp( POINT pt )
{
	m_pointLbDown.x = -1;
	UIReleaseCapture();
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if( pXCtrl->MouseLBUp( pt ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CUIScroll::SetScroll( const CUIButton& btnArrow1, const CUIButton& btnArrow2, const CUIImage& imgBar, const CUIButton& btnThumb )
{
	m_arrow1 = btnArrow1;
	m_arrow1.SetID( ID_ARROW1 );
	m_arrow1.SetParent( this );
	m_arrow2 = btnArrow2;
	m_arrow2.SetID( ID_ARROW2 );
	m_arrow2.SetParent( this );
	m_bar = imgBar;
	if ( m_parent )
	{
		RECT rcBar;
		m_bar.GetRect( rcBar );
		RECT rcParent;
		m_parent->GetRect( rcParent );
		if ( VERT == m_nbar )
		{
			rcBar.top = 0;
			rcBar.bottom = RECT_H(rcParent);
		}
		else
		{
			rcBar.left = 0;
			rcBar.right = RECT_W(rcParent);
		}
		m_bar.SetRect( rcBar );
	}
	m_bar.SetID( ID_BAR );
	m_bar.SetParent( this );
	m_thumb = btnThumb;
	m_thumb.SetID( ID_THUMB );
	m_thumb.SetParent( this );
	OnSize();
}

BOOL CUIScroll::MouseWheel( int zDelta )
{
	if ( (g_flashingOffset > 0
		&& zDelta < 0 )
		||( g_flashingOffset < 0
		&& zDelta > 0 ) )
	{
		g_flashingOffset = 0;
	}
	KillTimer( NULL, g_flashScroll );
	g_flashScrollStep = 1;
	g_flashingScroll = this;
	g_flashingOffset += ((((long)sqrt((double)8*m_scrollStep+1)-1)>>1)*(zDelta>0?1:-1))>>(g_flashingOffset>0?1:0);
	g_flashScroll = SetTimer( NULL, 0, 10, FlashScroll );
	return TRUE;
}