#include "XGroup.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILink.h"
#include "UIListCtrl.h"
CXCtrlBase* CGroup::AddXCtrl( UNIT_STYLE enStyle, const tstring& strText, const CRect& rcXCtrl, INT32 nCommandId /*= UNIT_TEXT_ID */ )
{
	ASSERT( UNS_NULL != enStyle );
	CXText* pXCtrl = NULL;
	if( nCommandId != UNIT_TEXT_ID )
	{ 

		pXCtrl = (CXText*)GetXCtrl( nCommandId );
		if ( pXCtrl )
		{
#if _DEBUG
			OutputDebugString( _T("\r\nWarning: there is one more unit int the Row with the same ID, it will radom to remove one!\r\n") );
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
			case UNS_LINK:
				{
					pXCtrl = new CUILink( );
				}
				break;
			case UNS_BUTTON:
				{
					pXCtrl = new CUIButton();
				}
				break;
			case UNS_IMAGE:
				{
					pXCtrl= new CUIImage();
				}
				break;
			case UNS_TEXT:
			default:
				{
					pXCtrl = new CXText();
				}
				break;
			}
		}
		catch( ... )
		{
			ASSERT( 0 );
			return NULL;
		}

		if ( NULL == pXCtrl )
		{
			return NULL;
		}
		pXCtrl->CreateXCtrl( strText, rcXCtrl, nCommandId );
		pXCtrl->SetOwer(this);
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


INT32 CGroup::AddXCtrl( const CXCtrlBase* pXCtrl )
{
	if ( !pXCtrl )
	{
		return RL_FAIL;
	}
	CXText* pXCtrlT = NULL;
	if( pXCtrl->GetID() != UNIT_TEXT_ID )
	{ 

		pXCtrlT = (CXText*)GetXCtrl( pXCtrl->GetID() );
		if ( pXCtrlT )
		{
#if _DEBUG
			OutputDebugString( _T("\r\nWarning: there is one more unit int the Row with the same ID, it will radom to remove one!\r\n") );
#endif
			return RL_OK;
		}
	}
	try
	{
		switch ( pXCtrl->GetXCtrlStyle() )
		{
		case UNS_LINK:
			{
				pXCtrlT = new CUILink( *(CUILink*)pXCtrl );
			}
			break;
		case UNS_BUTTON:
			{
				pXCtrlT = new CUIButton( *(CUIButton*)pXCtrl );
			}
			break;
		case UNS_IMAGE:
			{
				pXCtrlT= new CUIImage( *(CUIImage*)pXCtrl );
			}
			break;
		case UNS_TEXT:
		default:
			{
				pXCtrlT = new CXText( *(CXText*)pXCtrl );
			}
			break;
		}
		if ( NULL == pXCtrlT )
		{
			return RL_FAIL;	
		}
		pXCtrlT->SetOwer(this);
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

INT32 CGroup::RemoveXCtrl( INT32 nCtrlId )
{
	for ( XCTRLARRAY::iterator _lab = m_XCtrls.begin();
		_lab != m_XCtrls.end(); ++_lab )
	{
		if ( (*_lab)->GetID() == nCtrlId )
		{
			CXCtrlBase* pXCtrl = *_lab;
			*_lab = NULL;
			delete pXCtrl;
			pXCtrl = NULL;
			m_XCtrls.erase( _lab );
			return RL_OK;
		}
	}
	return RL_FAIL;
}

CXCtrlBase* CGroup::GetXCtrl( UINT32 nCtrlId )
{
	ASSERT( UNIT_TEXT_ID != nCtrlId );
	for ( XCTRLARRAY::iterator _lab = m_XCtrls.begin();
		_lab != m_XCtrls.end(); ++_lab )
	{
		if ( (*_lab)->GetID() == nCtrlId )
		{
			return *_lab;
		}
	}
	return NULL;
}

VOID CGroup::EnumXCtrls( XCTRLARRAY& vecXCtrls )const
{
// 	for ( XCTRLARRAY::const_iterator itStart = m_XCtrls.begin();\
// 		itStart != m_XCtrls.end(); itStart++)
// 	{
// 		vecXCtrls.push_back( *itStart );
// 	}
	vecXCtrls.assign( m_XCtrls.begin(), m_XCtrls.end() );
	return;
}

VOID CGroup::Draw( CDC* pDC /*= NULL */ )
{
	ASSERT( pDC );
	if ( NULL == pDC || !IsValid() )
	{
		return;
	}

	CRect rcDraw;

	CXCtrlBase* pXCtrl = NULL;
	CPoint ptOrg;
	for ( XCTRLARRAY::iterator itStart = m_XCtrls.begin();\
		itStart != m_XCtrls.end(); itStart++ )
	{
		pXCtrl = *itStart;
		if ( pXCtrl )
		{
			pXCtrl->OnDraw( pDC );
		}
	}

	for ( std::vector<HGROUP>::iterator _beg = m_childs.begin();
		_beg != m_childs.end(); ++_beg )
	{
		( *_beg )->Draw( pDC );
	}
}

CXCtrlBase* CGroup::GetXCtrlFromPoint( CPoint pt )
{
	CXCtrlBase* pXCtrl = NULL;
	CRect rcXCtrl;
	for ( XCTRLARRAY::iterator itStart = m_XCtrls.begin();\
		itStart != m_XCtrls.end(); itStart++ )
	{
		pXCtrl = *itStart;
		if( pXCtrl && pXCtrl->IsVisable() && pXCtrl->GetRect().PtInRect( pt ) )
		{
			return pXCtrl;
		}
	}
	return NULL;
}

VOID CGroup::_Clear()
{
	m_nIndex = -1;
	m_nMargin = 0;
	m_dwData = 0;
	m_size.SetSize( 0, 0 );
	m_org.SetPoint( 0, 0 );
	m_dwSate = RRS_NORMAL;
	m_pOwerWnd = NULL;
	m_pHot = NULL;
	m_childs.clear();
	if ( m_parent )
	{
		m_parent.Release();
	}
	
	for ( XCTRLARRAY::iterator itStart = m_XCtrls.begin();\
		itStart != m_XCtrls.end(); itStart++ )
	{
		CXCtrlBase* pXCtrl = *itStart;
		*itStart = NULL;
		delete pXCtrl;
		pXCtrl = NULL;
	}
	m_XCtrls.clear();
	m_childs.clear();
}

VOID CGroup::_Copy( const CGroup& row )
{
	_Clear();
	m_nIndex = row.m_nIndex;
	m_size = row.m_size;
	m_org  = row.m_org;
	m_nMargin = row.m_nMargin;
	m_dwData = row.m_dwData;
	m_pOwerWnd = row.m_pOwerWnd;
	m_pHot = NULL;

	for ( XCTRLARRAY::const_iterator itStart = row.m_XCtrls.begin();\
		itStart != row.m_XCtrls.end(); itStart++ )
	{
		AddXCtrl( *itStart );
	}
}

CRect CGroup::GetRect() const
{
	return CRect( m_org, m_size  );
}

CRect CGroup::GetClient() const
{
	CRect rcClient;
	rcClient.SetRectEmpty();
	if ( m_pOwerWnd )
	{
		rcClient = GetRect();
		rcClient.DeflateRect( m_nMargin, m_nMargin );
	}
	return rcClient;
}

const CSize& CGroup::GetSize() const
{
	return m_size;
}

bool CGroup::IsValid()
{
	return ( m_pOwerWnd && m_nIndex>=0 );
}
BOOL CGroup::OnMouseMove ( UINT nFlags, CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		if ( m_pHot && m_pHot != pXCtrl )
		{
			m_pHot->OnMouseLeave();
		}
		m_pHot = pXCtrl;
		(VOID)pXCtrl->OnMouseMove( pt );
	}
	else
	{
		if ( m_pHot )
		{
			m_pHot->OnMouseLeave();
			m_pHot = NULL;
		}
	}

	return FALSE;
}
BOOL CGroup::OnLClick( CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->OnLClick( pt );
	}
	return FALSE;
}

BOOL CGroup::OnLButtonDown( CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->OnLButtonDown( pt );
	}
	return FALSE;
}

 BOOL CGroup::OnLDBClick( CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->OnLDBClick( pt );
	}
	return FALSE;
}
 BOOL CGroup::OnRClick( CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->OnRClick( pt );
	}
	return FALSE;
}
 BOOL CGroup::OnRDBClick( CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl )
	{
		return pXCtrl->OnRDBClick( pt );
	}
	return FALSE;
}

 VOID CGroup::OnSetCursor( CPoint pt )
{
	CXCtrlBase* pXCtrl = GetXCtrlFromPoint( pt );
	if ( pXCtrl && pXCtrl->GetHotRect().PtInRect( pt ) )
	{
		pXCtrl->OnSetCursor( );
	}
	return;
}

 VOID CGroup::OnMouseLeave(  )
{
	try
	{
		if ( m_pHot )
		{
			m_pHot->OnMouseLeave();
			m_pHot = NULL;
		}
	}
	catch( ... )
	{

	}

}

 BOOL  CGroup::IsHot      (  )      
{
	if(m_pOwerWnd)
	{
		return m_pOwerWnd->GetState( m_nIndex )&RRS_HOT;
	}
	return FALSE; 
}

 BOOL  CGroup::IsFocused	 (  )	   
{ 
	if(m_pOwerWnd)
	{ 
		return m_pOwerWnd->GetState(m_nIndex)&RRS_FOCUSED;
	}
	return FALSE; 
}