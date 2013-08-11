#include "UILayPanel.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILink.h"
#include "UIListCtrl.h"
#include "UIEdit.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIRichEdit.h"
IUICtrl* CUILayPanel::AddXCtrl( XW_CLS enStyle, const tstring& strText, const RECT& rcXCtrl, INT32 nCommandId /*= UNIT_TEXT_ID */ )
{
	assert( XW_NULL != enStyle );
	CUIText* pXCtrl = NULL;
	if( nCommandId != UNIT_TEXT_ID )
	{ 

		pXCtrl = (CUIText*)GetXCtrl( nCommandId );
		if ( pXCtrl )
		{
#if _DEBUG
			OutputDebugString( _T("\r\nWarning: there is one more unit int the Panel with the same ID, it will radom to remove one!\r\n") );
#endif
			return pXCtrl;
		}
	}
	try
	{
		try
		{
			switch ( enStyle )
			{
			case XW_LINK:
				{
					pXCtrl = new CUILink( );
				}
				break;
			case XW_BUTTON:
				{
					pXCtrl = new CUIButton();
				}
				break;
			case XW_IMAGE:
				{
					pXCtrl= new CUIImage();
				}
				break;
			case XW_CHECKBOX:
				{
					pXCtrl = new CUICheckBox();
				}
				break;
			case XW_EDIT:
				{
					pXCtrl= new CUIEdit( );
				}
				break;
			case XW_RICHEDIT:
				{
					pXCtrl= new CUIRichEdit( );
				}
				break;
			case XW_COMBO:
				{
					pXCtrl= new CUIComboBox( );
				}
				break;
			case XW_TEXT:
			default:
				{
					pXCtrl = new CUIText();
				}
				break;
			}
		}
		catch( ... )
		{
			assert( 0 );
			return NULL;
		}

		if ( NULL == pXCtrl )
		{
			return NULL;
		}
		pXCtrl->Create( strText, rcXCtrl, nCommandId );
		pXCtrl->SetParent(this);
		m_XCtrls.push_back( pXCtrl );
	}
	catch( ... )
	{
		if ( NULL != pXCtrl )
		{
			delete pXCtrl;
			pXCtrl = NULL;
		}
		return NULL;
	}
	return pXCtrl;
}


INT32 CUILayPanel::AddXCtrl( const IUICtrl* pXCtrl )
{
	if ( !pXCtrl )
	{
		return RL_FAIL;
	}
	CUIText* pXCtrlT = NULL;
	if( pXCtrl->GetID() != UNIT_TEXT_ID )
	{ 

		pXCtrlT = (CUIText*)GetXCtrl( pXCtrl->GetID() );
		if ( pXCtrlT )
		{
#if _DEBUG
			OutputDebugString( _T("\r\nWarning: there is one more unit int the Panel with the same ID, it will radom to remove one!\r\n") );
#endif
			return RL_OK;
		}
	}
	try
	{
		switch ( pXCtrl->GetClass() )
		{
		case XW_LINK:
			{
				pXCtrlT = new CUILink( *(CUILink*)pXCtrl );
			}
			break;
		case XW_BUTTON:
			{
				pXCtrlT = new CUIButton( *(CUIButton*)pXCtrl );
			}
			break;
		case XW_IMAGE:
			{
				pXCtrlT= new CUIImage( *(CUIImage*)pXCtrl );
			}
			break;
		case XW_EDIT:
			{
				pXCtrlT= new CUIEdit( *(CUIEdit*)pXCtrl );
			}
			break;
		case XW_RICHEDIT:
			{
				pXCtrlT= new CUIRichEdit( *(CUIRichEdit*)pXCtrl );
			}
			break;
		case XW_CHECKBOX:
			{
				pXCtrlT= new CUICheckBox( *(CUICheckBox*)pXCtrl );
			}
			break;
		case XW_COMBO:
			{
				pXCtrlT= new CUIComboBox( *(CUIComboBox*)pXCtrl);
			}
			break;
		case XW_TEXT:
		default:
			{
				pXCtrlT = new CUIText( *(CUIText*)pXCtrl );
			}
			break;
		}
		if ( NULL == pXCtrlT )
		{
			return RL_FAIL;	
		}
		pXCtrlT->SetParent(this);
		m_XCtrls.push_back( pXCtrlT );
	}
	catch( ... )
	{
		if ( NULL != pXCtrl )
		{
			delete pXCtrl;
			pXCtrl = NULL;
		}
		return RL_FAIL;
	}
	return RL_OK;
}

INT32 CUILayPanel::RemoveXCtrl( INT32 nCtrlId )
{
	for ( XCTRLARRAY::iterator _lab = m_XCtrls.begin();
		_lab != m_XCtrls.end(); ++_lab )
	{
		if ( (*_lab)->GetID() == nCtrlId )
		{
			IUICtrl* pXCtrl = *_lab;
			*_lab = NULL;
			delete pXCtrl;
			pXCtrl = NULL;
			m_XCtrls.erase( _lab );
			return RL_OK;
		}
	}
	return RL_FAIL;
}

IUICtrl* CUILayPanel::GetXCtrl( UINT32 nCtrlId )const
{
	assert( UNIT_TEXT_ID != nCtrlId );
	for ( XCTRLARRAY::const_iterator _lab = m_XCtrls.begin();
		_lab != m_XCtrls.end(); ++_lab )
	{
		if ( (*_lab)->GetID() == nCtrlId )
		{
			return *_lab;
		}
	}
	return NULL;
}

void CUILayPanel::EnumXCtrls( XCTRLARRAY& vecXCtrls )const
{
	vecXCtrls.assign( m_XCtrls.begin(), m_XCtrls.end() );
	return;
}

void CUILayPanel::OnDraw(  IPainter* painter  )
{
	RECT rcDraw;
	GetRect( rcDraw );
	if( !painter->RectVisible( &rcDraw ) )
	{
		return;
	}
	IUICtrl* pXCtrl = NULL;
	for ( XCTRLARRAY::iterator itStart = m_XCtrls.begin();\
		itStart != m_XCtrls.end(); itStart++ )
	{
		pXCtrl = *itStart;
		if ( pXCtrl )
		{
			pXCtrl->OnDraw( painter );
		}
	}
}

IUICtrl* CUILayPanel::GetXCtrlFromPoint( POINT pt )
{
	IUICtrl* pXCtrl = NULL;
	for ( XCTRLARRAY::reverse_iterator itStart = m_XCtrls.rbegin();\
		itStart != m_XCtrls.rend(); itStart++ )
	{
		pXCtrl = *itStart;

		if( pXCtrl 
			&& pXCtrl->IsVisable() 
			&& pXCtrl->HasNotify() )
		{
			RECT rcCtrl;
			pXCtrl->GetRect( rcCtrl );
			if ( ::PtInRect( &rcCtrl, pt ) )
			{
				return pXCtrl;
			}
		}
	}
	return NULL;
}

void CUILayPanel::_Clear()
{
	m_nId = -1;
	m_nMargin = 0;
	m_dwData = 0;
	RECT_EMPTY( m_rect );
	m_dwSate = RRS_NORMAL;
	m_parent = NULL;
	m_pHot = NULL;
	m_bVisable = true;
	m_bLBDown = false;
	m_bRBDown = false;
	m_bMouseIn = false;
	
	for ( XCTRLARRAY::iterator itStart = m_XCtrls.begin();\
		itStart != m_XCtrls.end(); itStart++ )
	{
		IUICtrl* pXCtrl = *itStart;
		*itStart = NULL;
		delete pXCtrl;
		pXCtrl = NULL;
	}
	m_XCtrls.clear();
}

void CUILayPanel::_Copy( const CUILayPanel& row )
{
	_Clear();
	m_nId = row.m_nId;
	m_rect = row.m_rect;
	m_nMargin = row.m_nMargin;
	m_parent = row.m_parent;
	m_bVisable = row.m_bVisable;
	m_dwData = row.m_dwData;
	m_pBkGnd = row.m_pBkGnd;
	m_bLBDown = false;
	m_bRBDown = false;
	m_bMouseIn = false;
	m_pHot = NULL;

	for ( XCTRLARRAY::const_iterator itStart = row.m_XCtrls.begin();\
		itStart != row.m_XCtrls.end(); itStart++ )
	{
		AddXCtrl( *itStart );
	}
}

void  CUILayPanel::GetRect(RECT& rect)const
{
	rect = m_rect;
	if ( m_parent )
	{
		RECT rcParent;
		m_parent->GetRect( rcParent );
		::OffsetRect( &rect, rcParent.left, rcParent.top );
	}
}

void CUILayPanel::GetClient(  RECT& rect )
{
	GetRect( rect );
	::InflateRect( &rect, -m_nMargin, -m_nMargin );
}

SIZE CUILayPanel::GetSize() const
{
	SIZE size = {RECT_W(m_rect), RECT_H(m_rect)};
	return size;
}

bool CUILayPanel::IsValid()const
{
	return ( m_parent && m_nId>=0 );
}

bool  CUILayPanel::IsHot      (  )const     
{
	if(m_parent)
	{
		return RRS_NORMAL!=(m_dwSate&RRS_HOT);
	}
	return false; 
}

 bool  CUILayPanel::IsFocused	 (  )const	   
{ 
	if(m_parent)
	{ 
		return RRS_NORMAL != (m_dwSate&RRS_FOCUSED);
	}
	return false; 
}

BOOL CUILayPanel::MouseMoving ( POINT pt )
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
	if ( !m_bMouseIn )
	{
		m_bMouseIn = true;
		MouseIn();
	}
	_OnNotify( XLAY_MOUSEMOVE, pt );
	return FALSE;
}

BOOL CUILayPanel::MouseLBUp( POINT pt )
{
	bool bLb = m_bLBDown;
	m_bLBDown = false;
	UIReleaseCapture();
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if( pXCtrl->MouseLBUp( pt ) )
		{
			return TRUE;
		}
	}
	if ( bLb )
	{
		MouseLClick( pt );
	}
	_OnNotify( XLAY_LBUP, pt );
	return FALSE;
}

BOOL CUILayPanel::MouseLBDown( POINT pt )
{
	m_bLBDown = true;
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if( pXCtrl->MouseLBDown( pt ) )
		{
			return TRUE;
		}
	}
	UISetCapture( this );
	_OnNotify( XLAY_LBDOWN, pt );
	return FALSE;
}

BOOL CUILayPanel::MouseLDBClick( POINT pt )
{
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if( pXCtrl->MouseLDBClick( pt ) )
		{
			return TRUE;
		}
	}
	_OnNotify( XLAY_LDBCLICK, pt );
	return FALSE;
}

BOOL CUILayPanel::MouseRBUp( POINT pt )
{
	bool bRb = m_bRBDown;
	m_bRBDown = false;
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if( pXCtrl->MouseRBUp( pt ) )
		{
			return TRUE;
		}
	}
	if ( bRb )
	{
		MouseRClick( pt );
	}
	_OnNotify( XLAY_RBUP, pt);
	return TRUE;
}

BOOL CUILayPanel::MouseRDBClick( POINT pt )
{
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->MouseRDBClick( pt );
	}
	_OnNotify( XLAY_RDBCLICK,pt );
	return FALSE;
}

BOOL CUILayPanel::CursorIn( POINT pt )
{
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->CursorIn( pt );
	}
	return FALSE;
}

void CUILayPanel::MouseLeave(  )
{
	m_bMouseIn = false;
	m_bRBDown  =  false;
	m_bLBDown  =  false;
	try
	{
		if ( m_pHot )
		{
			m_pHot->MouseLeave();
			m_pHot = NULL;
		}
	}
	catch( ... )
	{

	}
	_OnNotify( XLAY_MOUSELEAVE, POINT_C(0,0));
}

void CUILayPanel::MouseIn()
{
	_OnNotify( XLAY_MOUSEIN, POINT_C(0,0) );
}

BOOL CUILayPanel::MouseLClick( POINT pt )
{
	_OnNotify( XLAY_LCLICK, pt);
	return TRUE;
}

BOOL CUILayPanel::MouseRBDown( POINT pt )
{
	m_bRBDown = true;
	IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if( pXCtrl->MouseRBDown( pt ) )
		{
			return TRUE;
		}
	}
	_OnNotify( XLAY_RBDOWN, pt );
	return TRUE;
}

BOOL CUILayPanel::MouseRClick( POINT pt )
{
	_OnNotify( XLAY_RCLICK, pt );
	return TRUE;
}

void CUILayPanel::CalcSize()
{
	if ( !m_parent )
	{
		return;
	}
	RECT rcClient;
	m_parent->GetClient(rcClient);
	if ( IsEmpty() )
	{
		return ;
	}
	INT32 nTop = 0;
	INT32 nBottom = 0;
	for ( XCTRLARRAY::const_iterator itStart = m_XCtrls.begin();\
		itStart != m_XCtrls.end(); itStart++)
	{
		CUIText* pTmp = (CUIText*)(*itStart);
		if ( pTmp )
		{
			if ( pTmp->m_rcXCtrl.top < nTop )
			{
				nTop = pTmp->m_rcXCtrl.top;
			}

			if ( pTmp->m_rcXCtrl.bottom > nBottom )
			{
				nBottom = pTmp->m_rcXCtrl.bottom;
			}
		}
	}
	int nHeight = nBottom-nTop+2*m_nMargin;
	m_rect.right = m_rect.left + RECT_W(rcClient)-2*m_nMargin;
	m_rect.bottom = m_rect.top+nHeight;
	if( m_nId >= 0 )
	{
		Notify( XLAY_SIZECHANGED, 0, (LPARAM)(LONG_PTR)this );
	}
}