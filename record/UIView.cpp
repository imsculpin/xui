// UIWnd.cpp : 实现文件
#include "UIView.h"
#include "UIText.h"
#include <WindowsX.h>
#include <algorithm>
#include <set>
#include "UIWindow.h"
#include <CommCtrl.h>
using std::set;
#define  DEFAULT_CLASS _T("X_DEFAULT")

// CUIView
CUIView::CUIView()
:m_hCursorNormal(LoadCursor( NULL, IDC_ARROW ))
,m_bVisable(false)
,m_dwStyle(0)
,m_nCommandId(-1)
,m_hFont(NULL)
,m_pUIWindow(NULL)
,m_parent(NULL)
,m_bScrollBarInClient(false)
{
	m_imageBkGnd.imageAvgClr = 0xFEFFFFFF;
	RtlZeroMemory( &m_margin, sizeof(Margin) );
}

BOOL CUIView::Create( IN const RECT& rect, IN IUIWindow* parent, IN UINT32 nID, IUIBase* pInsertBefore /*= NULL*/ )
{
	m_rect = rect;
	m_nCommandId = nID;
	m_VScroll.Create( VERT, this, 0 );
	m_HScroll.Create( HORZ, this, 1 );
	if ( parent )
	{	
		Attach( parent, pInsertBefore );
	}
	return TRUE;
}

BOOL CUIView::AddXCtrl( IUICtrl* pUnit, AREA area/*= CLIENT*/ )
{
	if ( pUnit )
	{
		HLAYPANEL* pPanel = NULL;
		try
		{	
			switch( area )
			{
			case TOP:
				{
					pPanel = &m_panelTop;
					if ( NULL == (*pPanel) )
					{
						(*pPanel)= new CUILayPanel(area);
						(*pPanel)->SetParent( this );
						(*pPanel)->SetRect( RECT_C(0, 0, RECT_W(m_rect), m_margin.top) );
					}
				}
				break;
			case RIGHT:
				{
					pPanel = &m_panelRight;
					if ( NULL == (*pPanel) )
					{
						(*pPanel)= new CUILayPanel(area);
						(*pPanel)->SetParent( this );
						(*pPanel)->SetRect( RECT_C(RECT_W(m_rect)-m_margin.right, m_margin.top, RECT_W(m_rect), RECT_H(m_rect)-m_margin.bottom) );
					}
				}
				break;
			case LEFT:
				{
					pPanel = &m_panelLeft;
					if ( NULL == (*pPanel) )
					{
						(*pPanel)= new CUILayPanel(area);
						(*pPanel)->SetParent( this );
						(*pPanel)->SetRect( RECT_C(0, m_margin.top, m_margin.left, RECT_H(m_rect)-m_margin.bottom) );
					}
				}
				break;
			case BOTTOM:
				{
					pPanel = &m_panelBottom;
					if ( NULL == (*pPanel) )
					{
						(*pPanel)= new CUILayPanel(area);
						(*pPanel)->SetParent( this );
						(*pPanel)->SetRect( RECT_C(0, RECT_H(m_rect)-m_margin.bottom, RECT_W(m_rect), RECT_H(m_rect)) );
					}
				}
				break;
			default:
				{
					pPanel = &m_panelClient;
					if ( NULL == (*pPanel) )
					{
						(*pPanel)= new CUILayPanel(area);
						(*pPanel)->SetParent( this );
						(*pPanel)->SetRect( RECT_C(m_margin.left, m_margin.top, \
							RECT_W(m_rect)-m_margin.right, RECT_H(m_rect)-m_margin.bottom) );
					}
				}
				break;
			}
		}
		catch( ... )
		{

		}
		if( *pPanel )
		{
			return RL_OK == (*pPanel)->AddXCtrl( pUnit );
		}
	}
	return FALSE;
}

BOOL CUIView::MouseMoving( POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseMoving( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseMoving( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty() )
	{
		return m_panelClient->MouseMoving( point );
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->MouseMoving( point );
	}
	return FALSE;
}


BOOL CUIView::CursorIn( POINT point )
{
	BOOL bRet = FALSE;
	if ( m_panelClient && IsEmpty() )
	{
		bRet = m_panelClient->CursorIn( point );
		if ( !bRet && m_hCursorNormal )
		{
			::SetCursor( m_hCursorNormal );
			bRet = TRUE;
		}
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->CursorIn( point );
	}
	return bRet;
}


BOOL CUIView::MouseLBUp  ( POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseLBUp( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseLBUp( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty() )
	{
		return m_panelClient->MouseLBUp( point );
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->MouseLBUp( point );
	}
	return FALSE;
}

BOOL CUIView::MouseRBUp  ( POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseRBUp( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseRBUp( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty() )
	{
		return m_panelClient->MouseRBUp( point );
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->MouseRBUp( point );
	}
	return FALSE;
}


BOOL CUIView::MouseLDBClick( POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseLDBClick( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseLDBClick( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty() )
	{
		return m_panelClient->MouseLDBClick( point );
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->MouseLDBClick( point );
	}
	return FALSE;
}

BOOL CUIView::MouseRBDown( POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseRBDown( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseRBDown( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty() )
	{
		return m_panelClient->MouseRBDown( point );
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->MouseRBDown( point );
	}
	return FALSE;
}


BOOL CUIView::MouseRDBClick(POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseRDBClick( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseRDBClick( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty() )
	{
		return m_panelClient->MouseRDBClick( point );
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel )
	{
		return (*pPanel)->MouseRDBClick( point );
	}
	return FALSE;
}


void CUIView::MouseLeave( )
{
	if ( m_VScroll.IsVisable() )
	{
		m_VScroll.MouseLeave(  );
	}
	if ( m_HScroll.IsVisable() )
	{
		m_HScroll.MouseLeave(  );
	}
	if ( m_panelClient && IsEmpty() )
	{
		m_panelClient->MouseLeave(  );
	}
	if ( NULL != m_panelLeft )
	{
		m_panelLeft->MouseLeave();
	}
	if ( NULL != m_panelRight )
	{
		m_panelRight->MouseLeave();
	}
	if ( NULL != m_panelTop )
	{
		m_panelTop->MouseLeave();
	}
	if ( NULL != m_panelBottom )
	{
		m_panelBottom->MouseLeave();
	}
}

BOOL CUIView::MouseLBDown( POINT point )
{
	if ( m_VScroll.IsVisable() )
	{
		if ( m_VScroll.MouseLBDown( point ) )
		{
			return TRUE;
		}
	}
	if ( m_HScroll.IsVisable() )
	{
		if ( m_HScroll.MouseLBDown( point ) )
		{
			return TRUE;
		}
	}
	if ( m_panelClient && IsEmpty()  )
	{
		if( m_panelClient->MouseLBDown( point ) )
		{
			return TRUE;
		}
	}
	HLAYPANEL* pPanel = IsPtInMargin( point );
	if ( pPanel && m_panelClient != *pPanel )
	{
		if( (*pPanel)->MouseLBDown( point ) )
		{
			return TRUE;
		}	
	}
	UIReleaseCapture();
	return FALSE;
}

void CUIView::OnDraw(  IPainter* painter )
{

	if ( IsEmpty()
		&& NULL != m_panelClient 
		&& !m_panelClient->IsEmpty() )
	{
		//显示条件较验
		m_panelClient->OnDraw( painter );
	}
	if ( NULL != m_panelLeft 
		&& !m_panelLeft->IsEmpty() )
	{
		//显示条件较验
		m_panelLeft->OnDraw( painter );
	}
	if ( NULL != m_panelRight 
		&& !m_panelRight->IsEmpty() )
	{
		//显示条件较验
		m_panelRight->OnDraw( painter );
	}
	if ( NULL != m_panelTop
		&& !m_panelTop->IsEmpty() )
	{
		//显示条件较验
		m_panelTop->OnDraw( painter );
	}
	if ( NULL != m_panelBottom 
		&& !m_panelBottom->IsEmpty() )
	{
		//显示条件较验
		m_panelBottom->OnDraw( painter );
	}
}

void CUIView::DrawScroll( IPainter* painter )
{
	//画纵向滚动条
	if ( m_VScroll.IsVisable() )
	{
		RECT rcVscroll;
		m_VScroll.GetRect( rcVscroll );
		if ( painter->RectVisible( &rcVscroll ) )
		{
			m_VScroll.OnDraw( painter );
		}
	}
	//画横向滚动条
	if ( m_HScroll.IsVisable() )
	{
		RECT rcHscroll;
		m_HScroll.GetRect( rcHscroll );
		if ( painter->RectVisible( &rcHscroll ) )
		{
			m_HScroll.OnDraw( painter );
		}
	}
}

void CUIView::_UpdateScroll( int nMaxVScroll /*= 1*/, int nMaxHScroll /*= 1 */ )
{
	int nPre = 0;
	RECT rcThis;
	GetRect( rcThis );
	if ( GetStyle() & XWS_HSCROLL )
	{
		if ( nMaxHScroll < RECT_W(rcThis) )
		{
			m_HScroll.SetPos(0);
		}
		m_HScroll.SetRange( 0, nMaxHScroll );
	}

	if( GetStyle() & XWS_VSCROLL )
	{
		if ( nMaxVScroll < RECT_H(rcThis) )
		{
			m_VScroll.SetPos(0);
		}
		m_VScroll.SetRange( 0, nMaxVScroll );
	}
}

BOOL CUIView::HorzScroll( int nSBCode, int nPos  )
{
	if( nSBCode == THUMB)
	{
		if ( GetUIWindow() )
		{
			::InvalidRect( GetUIWindow()->GetHandle(), &m_rect );
		}
	}
	return TRUE;
}


BOOL CUIView::VertScroll( int nSBCode, int nPos, int iDrect )
{
	nSBCode;
	nPos;
	RECT rcClient;
	GetClient( rcClient );	
	if( nSBCode == THUMB )
	{
		if ( GetUIWindow() )
		{
			::InvalidRect( GetUIWindow()->GetHandle(), &m_rect );
		}
	}
	return TRUE;
}


BOOL CUIView::MouseWheel   ( int zDelta )
{
	if( m_VScroll.IsVisable() )
	{
		if( m_VScroll.MouseWheel( zDelta ) )
		{
			return TRUE;
		}
	}
	if( m_HScroll.IsVisable() )
	{
		if( m_HScroll.MouseWheel( zDelta ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CUIView::Detach()
{
	LRESULT lRet = ::UnregisterUIWnd( this );
	m_pUIWindow = NULL;
	return S_OK == lRet;
}

BOOL CUIView::Attach( IUIWindow* window, IUIBase* pInsertBefore /*= NULL */ )
{
	m_pUIWindow = window;
	if( S_OK != ::RegisterUIWnd( this, pInsertBefore ) )
	{
		m_pUIWindow = NULL;
		return FALSE;
	}
	return TRUE;
}

void CUIView::Show( bool bShow )
{
	if ( bShow != m_bVisable )
	{
		m_bVisable = bShow; 
		if ( GetUIWindow()&&GetUIWindow()->GetHandle() )
		{
			UpdateBkGnd( GetUIWindow( )->GetHandle() );
		}
	}
}

void CUIView::SetMargin( IN Margin margin )
{
	m_margin = margin;
	if ( m_panelLeft )
	{
		m_panelLeft->SetRect( RECT_C(0, m_margin.top, m_margin.left, RECT_H(m_rect)-m_margin.bottom) );
	}
	if ( m_panelRight )
	{
		m_panelRight->SetRect( RECT_C(RECT_W(m_rect)-m_margin.right, m_margin.top, RECT_W(m_rect), RECT_H(m_rect)-m_margin.bottom) );
	}
	if ( m_panelTop )
	{
		m_panelTop->SetRect( RECT_C(0, 0, RECT_W(m_rect), m_margin.top) );
	}
	if ( m_panelBottom )
	{
		m_panelBottom->SetRect( RECT_C(0, RECT_H(m_rect)-m_margin.bottom, RECT_W(m_rect), RECT_H(m_rect)) );
	}
	if ( m_panelClient )
	{
		m_panelClient->SetRect( RECT_C(m_margin.left, m_margin.top, \
			RECT_W(m_rect)-m_margin.right, RECT_H(m_rect)-m_margin.bottom) );
	}
}

void CUIView::SetRect( const RECT& rcSet, bool bErase )
{
	if ( rcSet.right == m_rect.right
		&& rcSet.bottom == m_rect.bottom
		&& rcSet.left == m_rect.left
		&& rcSet.top == m_rect.top )
	{
		return;
	}
	m_rect = rcSet;
// 	if ( NULL == GetUIWindow() )
// 	{
// 		return;
// 	}
	m_VScroll.OnSize();
	m_HScroll.OnSize();
	if ( m_panelLeft )
	{
		m_panelLeft->SetRect( RECT_C(0, m_margin.top, m_margin.left, RECT_H(m_rect)-m_margin.bottom) );
	}
	if ( m_panelRight )
	{
		m_panelRight->SetRect( RECT_C(RECT_W(m_rect)-m_margin.right, m_margin.top, RECT_W(m_rect), RECT_H(m_rect)-m_margin.bottom) );
	}
	if ( m_panelTop )
	{
		m_panelTop->SetRect( RECT_C(0, 0, RECT_W(m_rect), m_margin.top) );
	}
	if ( m_panelBottom )
	{
		m_panelBottom->SetRect( RECT_C(0, RECT_H(m_rect)-m_margin.bottom, RECT_W(m_rect), RECT_H(m_rect)) );
	}
	if ( m_panelClient )
	{
		m_panelClient->SetRect( RECT_C(m_margin.left, m_margin.top, \
			RECT_W(m_rect)-m_margin.right, RECT_H(m_rect)-m_margin.bottom) );
	}
	if ( bErase )
	{
		if ( GetUIWindow()&&GetUIWindow()->GetHandle() )
		{
			UpdateBkGnd( GetUIWindow()->GetHandle() );
			InvalidRect( &m_rect );
		}
	}
}

INT32 CUIView::GetScrollPos( int nBar ) const
{
	if ( nBar == VERT )
	{
		if ( m_VScroll.IsVisable() )
		{
			return (int)m_VScroll.GetPos();
		}
	}
	else
	{
		if ( m_HScroll.IsVisable() )
		{
			return (int)m_HScroll.GetPos();
		}
	}
	return 0;
}

void CUIView::SetScrollPos( int nBar, int nPos, BOOL bRedraw /*= TRUE */ )
{
	if ( nBar == VERT )
	{
		m_VScroll.SetPos( nPos );
	}
	else
	{
		m_HScroll.SetPos( nPos );
	}
	return;
}

IUICtrl* CUIView::GetXCtrl( INT32 nCtrlId )
{
	assert( nCtrlId != UNIT_TEXT_ID );
	IUICtrl* pCtrl = NULL;
	if ( m_panelClient )
	{
		pCtrl = m_panelClient->GetXCtrl( nCtrlId );
		if ( pCtrl )
		{
			return pCtrl;
		}
	}
	if ( m_panelLeft )
	{
		pCtrl = m_panelLeft->GetXCtrl( nCtrlId );
		if ( pCtrl )
		{
			return pCtrl;
		}
	}
	if ( m_panelRight )
	{
		pCtrl = m_panelRight->GetXCtrl( nCtrlId );
		if ( pCtrl )
		{
			return pCtrl;
		}
	}
	if ( m_panelTop )
	{
		pCtrl = m_panelTop->GetXCtrl( nCtrlId );
		if ( pCtrl )
		{
			return pCtrl;
		}
	}
	if ( m_panelBottom )
	{
		pCtrl = m_panelBottom->GetXCtrl( nCtrlId );
		if ( pCtrl )
		{
			return pCtrl;
		}
	}
	return pCtrl;
}

void CUIView::EnableScroll( int nBar, bool bEnable )
{
	if ( nBar == HORZ )
	{
		if ( bEnable )
		{
			m_dwStyle |= XWS_HSCROLL;
			_UpdateScroll( m_HScroll.Min(), m_HScroll.Max() );
		}
		else
		{
			m_dwStyle &= ~XWS_HSCROLL;
			m_HScroll.Show( false );
		}
	}
	else if ( nBar == VERT )
	{
		if ( bEnable )
		{
			m_dwStyle |= XWS_VSCROLL;
			_UpdateScroll( m_VScroll.Min(), m_VScroll.Max() );
		}
		else
		{
			m_dwStyle &= ~XWS_VSCROLL;
			m_VScroll.Show( false );
		}
	}
}