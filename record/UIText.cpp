#include "UIText.h"
#include "UILayPanel.h"
#include "UIView.h"
void CUIText::OnDraw(  IPainter* painter  )
{
	assert( XW_NULL != m_nClass );
	if ( !IsVisable() )
	{
		return;
	}
	if ( !m_strText.empty() )
	{
		RECT rcDraw;
		GetRect( rcDraw );
		if (  RECT_ISEMPTY(rcDraw)
			|| !painter->RectVisible( &rcDraw ))
		{
			return;
		}
		ARGB clrOld = painter->SetBrushColor( IsMouseIn()?GetHotTextColor():GetTextColor() );
		
		HFONT hFont = GetFont();
		if ( m_bEnableOutGlaw )
		{
			//DrawOuterGlow( _hDC, &rcDraw, m_strText.c_str(), m_hFont, 0, 0  );
		}
		painter->DrawString( m_strText.c_str(), -1, &rcDraw, hFont, m_nStyle );
		painter->SetBrushColor( clrOld );
	}
}

void CUIText::GetRect(RECT& rect) const
{
	assert( XW_NULL != m_nClass );
	if ( m_parent )
	{			
		//_CorrectSize();
		m_parent->GetClient(rect);
		rect.left = m_rcXCtrl.left + rect.left;
		rect.right= rect.left + RECT_W(m_rcXCtrl);
		rect.top  = m_rcXCtrl.top + rect.top;
		rect.bottom = rect.top + RECT_H(m_rcXCtrl);
	}
	else
	{
		rect = m_rcXCtrl;
	}
}

//void CUIText::_OnNotify( XWM_MESSAGE message )
//{
	
// 	if ( m_parent
// 		&&m_parent->GetParent()
// 		&&m_parent->GetParent()->GetSafeHwnd() )
// 	{
// 		CWnd* pDlg = m_parent->GetParent();
// 		if ( pDlg && pDlg->GetSafeHwnd() )
// 		{
// 			NM_PARAM nmRecord;
// 			nmRecord.hdr.hwndFrom = m_parent->GetParent()->GetSafeHwnd();
// 			nmRecord.hdr.idFrom = m_nCommandId;
// 			nmRecord.hdr.code = message;
// 			nmRecord.nIndex = m_parent->m_nId;
// 			nmRecord.lParam = 0;
// 			if ( pDlg->GetParent() )
// 			{
// 				pDlg->GetParent()->SendMessage( WM_XCTRL_MESSAGE, (WPARAM)nmRecord.hdr.idFrom, (LPARAM)(LONG_PTR)&nmRecord );
// 			}
// 		}
// 	}
//}

void CUIText::_FormatText( tstring& strText )
{
	if ( NULL == m_parent )
	{
		return;
	}
	RECT rcPanel;
	m_parent->GetClient( rcPanel );
	if ( m_strText.empty() || RECT_ISEMPTY(rcPanel) )
	{
		return;
	}
	if ( XWS_SINGLELINE & m_nStyle )
	{
		replace( strText.begin(), strText.end(), _T('\n'),_T('.') );
		replace( strText.begin(), strText.end(), _T('\r'),_T(' ') );
	}
}

void CUIText::_CorrectSize()
{
	if( (XWS_AUTOSIZE&m_nStyle) && m_parent )
	{
		RECT rcPanel;
		m_parent->GetClient( rcPanel );
		if ( m_strText.empty() || RECT_ISEMPTY(rcPanel) )
		{
			return;
		}
		if( !m_bChanged )
		{
			return;
		}
		m_bChanged = false;
		RECT rcPre;
		GetRect( rcPre );
		HDC hDC = GetDC( NULL );
		if ( hDC )
		{
			HFONT hOldFont = NULL;
			if ( NULL == m_hFont 
				&& m_parent )
			{
				m_hFont = m_parent->GetFont();
			}
			if( m_hFont )
			{
				hOldFont = (HFONT)SelectObject( hDC, m_hFont );
			}
			m_rcXCtrl.right = rcPanel.right - m_rcXCtrl.left + rcPanel.left;
			if ( XWS_SINGLELINE&m_nStyle )
			{
				m_strText = m_strTextBase;
				_FormatText( m_strText );
				int nFormat = DT_SINGLELINE | DT_NOPREFIX;
				if ( XWS_ENDELLIPSE & m_nStyle )
				{
					nFormat |= DT_END_ELLIPSIS;
				}
				if ( XWS_PATHELLIPSE & m_nStyle )
				{
					nFormat |= DT_PATH_ELLIPSIS;
				}
				DrawText( hDC, m_strText.c_str(), (int)m_strText.length(), &m_rcXCtrl, nFormat | DT_CALCRECT );
			}
			else
			{
				DrawText( hDC, m_strText.c_str(), (int)m_strText.length(), &m_rcXCtrl, DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT | DT_NOPREFIX );
				m_rcXCtrl.bottom += 5;
			}
			if ( m_hFont )
			{
				::SelectObject( hDC, (HGDIOBJ)hOldFont );
			}
			::DeleteDC( hDC );

			RECT rcThis = m_rcXCtrl;
			RECT rcPanel;
			m_parent->GetClient( rcPanel );
			RECT_TO(rcThis, rcPanel.left + rcThis.left, rcThis.top + rcPanel.top );
			if ( rcThis.right > rcPanel.right )
			{
				m_rcXCtrl.right = rcPanel.right - rcThis.left ;
			}

			if ( rcThis.bottom > rcPanel.bottom )
			{
				m_rcXCtrl.bottom = rcPanel.bottom - rcThis.top;
			}
		}
	}
}

bool CUIText::IsVisable() const
{
	if ( m_parent
		&& !m_parent->IsVisable() )
	{
		return false;
	}
	if ( m_showCondition
		&&!(*m_showCondition)() )
	{
		return false;
	}
	if ( (GetStyle() & XWS_HOTSHOW) && m_bShow )
	{
		return m_parent && m_parent->IsHot();
	}
	if ( (GetStyle() & XWS_HOTHIDE) && m_bShow  )
	{
		return m_parent && !m_parent->IsHot();
	}
	return m_bShow;
}

CUIText::CUIText( const CUIText& uObj ) : m_nClass  (uObj.m_nClass)
, m_crText   (uObj.m_crText)
, m_bShow    (uObj.m_bShow)
, m_bMouseIn (false)
, m_nStyle(uObj.m_nStyle)
, m_parent	 (uObj.m_parent)
, m_rcXCtrl	 (uObj.m_rcXCtrl)
, m_nCommandId( uObj.m_nCommandId )
, m_dwExtParam( uObj.m_dwExtParam )
, m_strTextBase(uObj.m_strTextBase)
, m_bLBDown(false)
, m_showCondition( uObj.m_showCondition )
, m_bHasNotify(uObj.m_bHasNotify)
, m_bEnableOutGlaw(uObj.m_bEnableOutGlaw)
{
	m_hFont = uObj.m_hFont;
	m_strText = m_strTextBase;
	_FormatText( m_strText );
	m_bChanged = true;
}

CUIText::CUIText() :m_nClass(XW_NULL)
,m_strText(_T(""))
,m_crText(CLR_DEFAULT_TEXT)
,m_parent(NULL)
,m_bShow(true)
,m_bMouseIn(false)
,m_bChanged(true)
,m_nStyle( XWS_ALIGNLEFT )
,m_nCommandId(UNIT_TEXT_ID)
,m_dwExtParam( 0 )
,m_bLBDown(false)
,m_showCondition( NULL )
,m_bHasNotify(true)
,m_bEnableOutGlaw(false)
{
	RECT_EMPTY( m_rcXCtrl );
}
const CUIText& CUIText::operator=( const CUIText& uObj )
{
	if ( this == &uObj )
	{
		return *this;
	}
	m_nClass   = uObj.m_nClass;
	m_crText	= uObj.m_crText;
	m_bShow     = uObj.m_bShow;
	m_nStyle = uObj.m_nStyle;
	m_parent		= uObj.m_parent;
	m_rcXCtrl	= uObj.m_rcXCtrl;
	m_nCommandId = uObj.m_nCommandId;
	m_dwExtParam = uObj.m_dwExtParam;
	m_strTextBase = uObj.m_strTextBase;
	m_showCondition = uObj.m_showCondition;
	m_bLBDown = false;
	m_bHasNotify	= uObj.m_bHasNotify;
	m_hFont = uObj.m_hFont;
	m_strText = m_strTextBase;
	m_bEnableOutGlaw = uObj.m_bEnableOutGlaw;
	_FormatText( m_strText );
	m_bChanged = true;
	return *this;
}

INT32 CUIText::Create( const tstring& strText, const RECT& rect, INT32 nCommandId /*= UNIT_TEXT_ID */ )
{
	assert( XW_NULL == m_nClass );
	m_nCommandId = nCommandId;
	m_strText = strText;
	m_strTextBase = strText;
	_FormatText( m_strText );
	m_rcXCtrl = rect;
	m_nClass = XW_TEXT;
	return RL_OK;
}

void CUIText::SetRect( const RECT& rcSet, bool bErase /*= false*/ )
{
	if ( RECT_EQUAL(m_rcXCtrl, rcSet ) )
	{
		return;
	}
	m_rcXCtrl = rcSet;
	m_bChanged = true;
	return;
}

inline void CUIText::MouseIn()
{
	m_bMouseIn = true;
	if ( HasNotify( ) )
	{
		_OnNotify( XWM_MOUSEIN, POINT_C(0,0) );
	}
}

inline BOOL CUIText::MouseLClick( POINT pt )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	_OnNotify( XWM_LCLICK, pt );
	return TRUE;
}

inline BOOL CUIText::MouseRBDown( POINT pt )
{
	m_bRbDown = true;
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	_OnNotify( XWM_RBDOWN, pt );
	return TRUE;
}

inline BOOL CUIText::MouseRClick( POINT pt )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	_OnNotify( XWM_RCLICK, pt );
	return TRUE;
}

inline BOOL CUIText::MouseLBDown( POINT pt )
{
	if ( !IsVisable() )
	{
		return FALSE;
	}
	m_bLBDown = true;
	if ( !HasNotify() )
	{
		return FALSE;
	}
	UISetCapture(this);
	_OnNotify( XWM_LBDOWN, pt );
	if( UIGetFocus() != this )
	{
		UISetFocus( this );
		FocusIn(pt);
	}
	return TRUE;
}

inline void CUIText::MouseLeave()
{
	m_bMouseIn = false;
	m_bLBDown = false;
	m_bRbDown = false;
	if ( XW_TEXT != m_nClass
		&& XW_IMAGE != m_nClass )
	{
		_Update();
	}
	if ( !HasNotify() )
	{
		return;
	}
	_OnNotify( XWM_MOUSELEAVE, POINT_C(0,0) );

}

inline BOOL CUIText::MouseMoving( POINT pt )
{
	if ( !IsVisable() )
	{
		return FALSE;
	}
	RECT rcThis;
	GetRect( rcThis );
	if ( ::PtInRect( &rcThis, pt ) )
	{
		if ( !m_bMouseIn )
		{
			MouseIn();
			if ( XW_TEXT != m_nClass
				&& XW_IMAGE != m_nClass )
			{
				_Update();
			}
		}
	}
	else
	{
		m_bMouseIn = false;
	}

	if ( HasNotify( ) 
		&& m_parent )
	{
		_OnNotify( XWM_MOUSEMOVE, pt );
	}
	return TRUE;
}

inline BOOL CUIText::MouseLBUp( POINT pt )
{
	if ( !IsVisable() )
	{
		return FALSE;
	}	
	if( m_bLBDown && m_bMouseIn )
	{
		m_bLBDown = false;
		MouseLClick( pt );
	}
	if ( !HasNotify() )
	{
		return FALSE;
	}
	UIReleaseCapture();
	_OnNotify( XWM_LBUP, pt );
	RECT rcThis;
	GetRect( rcThis );
	if ( !::PtInRect( &rcThis, pt ) )
	{
		MouseLeave();
	}
	return TRUE;
}

inline BOOL CUIText::MouseLDBClick( POINT pt )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	_OnNotify( XWM_LDBCLICK, pt );
	return TRUE;
}

inline BOOL CUIText::MouseRBUp( POINT pt )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	if ( m_bRbDown )
	{	
		_OnNotify( XWM_RCLICK, pt );
	}
	m_bRbDown = false;
	_OnNotify( XWM_RBUP, pt);
	return TRUE;
}

BOOL CUIText::MouseRDBClick( POINT pt )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	_OnNotify( XWM_RDBCLICK, pt );
	return TRUE;
}

BOOL CUIText::FocusIn( POINT point )
{
	if ( HasNotify() )
	{
		_OnNotify( XWM_FOCUSIN, point );
		return TRUE;
	}
	return FALSE;
}

BOOL CUIText::FocusLeave(  )
{
	if ( HasNotify() )
	{
		POINT point = {0};
		_OnNotify( XWM_FOCUSLEAVE, point );
		return TRUE;
	}
	return FALSE;
}

CUIText::~CUIText()
{
	if ( UIGetFocus() == this )
	{
		UISetFocus( NULL );
	}
	m_hFont    = NULL;
	m_parent   = NULL;
}

BOOL CUIText::KeyUp( WPARAM wParam, LPARAM lParam )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	NM_PARAM param = {0};
	param.lParam = lParam;
	param.nCtrlId = m_nCommandId;
	param.wVirtualKey = wParam;
	if ( m_parent )
	{
		param.nParentId = m_parent->GetID();
	}
	Notify( XWM_KEYUP, 0, (LPARAM)(LONG_PTR)&param );
	return TRUE;
}

BOOL CUIText::KeyDown( WPARAM wParam, LPARAM lParam )
{
	if ( !IsVisable()
		|| !HasNotify() )
	{
		return FALSE;
	}
	NM_PARAM param = {0};
	param.lParam = lParam;
	param.nCtrlId = m_nCommandId;
	param.wVirtualKey = wParam;
	if ( m_parent )
	{
		param.nParentId = m_parent->GetID();
	}
	Notify( XWM_KEYDOWN, 0, (LPARAM)(LONG_PTR)&param );
	return TRUE;
}