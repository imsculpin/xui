
#include "UITab.h"
#include <WindowsX.h>
void CUITab::OnDraw(  IPainter* painter  )
{
	for ( vector<HTABITEM>::iterator _lab = m_vecItems.begin();\
		m_vecItems.end() != _lab; ++_lab )
	{
		RECT rcNode;
		(*_lab)->GetRect( rcNode );
		if ( painter->RectVisible( &rcNode ) )
		{
			(*_lab)->SetState( RRS_NORMAL );
			if ( *_lab == m_hHot )
			{
				(*_lab)->SetState( RRS_HOT );
			}
			if ( *_lab == m_hFocus )
			{
				(*_lab)->SetState( RRS_FOCUSED|(*_lab)->GetState() );
			}
			if ( *_lab == m_hSelect )
			{
				(*_lab)->SetState( RRS_SELECTED|(*_lab)->GetState() );
			}	

			//绘制选中行的背景色
			if ( m_hSelect == *_lab 
				&& ( m_imagePressed.hImage|| X_TRANSPARENT != m_imagePressed.imageAvgClr ))
			{
				(*_lab)->SetBkGnd( &m_imagePressed );
			}
			else if( m_hHot == *_lab
				&& ( m_imageHot.hImage|| X_TRANSPARENT != m_imageHot.imageAvgClr ) )
			{
				(*_lab)->SetBkGnd( &m_imageHot );
			}
			else if ( m_imageNormal.hImage|| X_TRANSPARENT != m_imageNormal.imageAvgClr )
			{
				(*_lab)->SetBkGnd( &m_imageNormal );
			}
			if ( m_hSelect == *_lab )
			{
				continue;
			}
			EraseItemBkGnd( *_lab, painter );
			(*_lab)->OnDraw( painter );
		}
	}
	if ( m_hSelect )
	{
		EraseItemBkGnd( m_hSelect, painter );
		m_hSelect->OnDraw( painter );
	}
}

BOOL CUITab::Create( IN const RECT& rect, IUIWindow* pWindow, IN UINT32 nID, \
					IUIBase* pInsertBefore /*= NULL*/, bool bVert /*= false */ )
{
	if( bVert )
	{
		m_tab = VERT;
	}
	return __super::Create( rect, pWindow, nID, pInsertBefore );
}



BOOL CUITab::MouseMoving( POINT point )
{
	if ( __super::MouseMoving( point ) )
	{
		return TRUE;
	}
	if ( NULL != m_hSelect
		&& m_hSelect->MouseMoving( point ) )
	{
		if ( m_hHot != m_hSelect )
		{
			RECT rcInvalid= { 0,0,0,0 };
			if ( m_hHot )
			{
				m_hHot->GetRect( rcInvalid );
			}
			m_hHot = m_hSelect;
			InvalidRect( &rcInvalid );
		}
		return TRUE;
	}
	
	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem
		&& m_hSelect != hItem )
	{
		if ( m_hHot != hItem )
		{
			RECT rcPre = { 0,0,0,0 };
			if ( m_hHot )
			{
				m_hHot->GetRect( rcPre );
			}
			m_hHot = hItem;
			RECT rcInvalid;
			if ( !RECT_ISEMPTY(rcPre) )
			{
				::UnionRect( &rcInvalid, &rcPre, &m_hHot->GetRect( ) );
			}
			else
			{
				m_hHot->GetRect( rcInvalid );
			}
			InvalidRect( &rcInvalid );
		}
		hItem->MouseMoving( point );
	}
	return TRUE;
}

BOOL CUITab::MouseLBDown( POINT point )
{
	if ( __super::MouseLBDown( point ) )
	{
		return TRUE;
	}
	if ( NULL != m_hSelect
		&& ::PtInRect( &m_hSelect->GetRect(), point ) )
	{
		m_hSelect->MouseLBDown( point );
		return TRUE;
	}

	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem
		&& m_hSelect != hItem )
	{
		RECT rcPre= { 0,0,0,0 };
		if ( m_hSelect )
		{
			m_hSelect->GetRect( rcPre );
		}
		m_hSelect = (hItem);
		m_hFocus = m_hSelect;
		RECT rcInvalid;
		if ( !RECT_ISEMPTY(rcPre) )
		{
			::UnionRect( &rcInvalid, &rcPre, &m_hSelect->GetRect( ) );
		}
		else
		{
			m_hSelect->GetRect( rcInvalid );
		}
		InvalidRect( &rcInvalid );
		hItem->MouseLBDown( point );
		return TRUE;
	}
	return TRUE;
}

BOOL CUITab::MouseLBUp( POINT point )
{
	if ( __super::MouseLBUp( point ) )
	{
		return TRUE;
	}

	if ( NULL != m_hSelect
		&& ::PtInRect( &m_hSelect->GetRect(), point ) )
	{
		m_hSelect->MouseLBUp( point );
		return TRUE;
	}
	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem 
		&& m_hSelect != hItem )
	{
		hItem->MouseLBUp( point );
	}
	return TRUE;
}

BOOL CUITab::MouseRBDown( POINT point )
{
	if ( __super::MouseRBDown( point ) )
	{
		return TRUE;
	}
	if ( NULL != m_hSelect
		&& m_hSelect->MouseRBDown( point ) )
	{
		return TRUE;
	}
	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem 
		&& m_hSelect != hItem )
	{
		hItem->MouseRBDown( point );
	}
	return TRUE;
}

BOOL CUITab::MouseRBUp( POINT point )
{
	if ( __super::MouseRBUp( point ) )
	{
		return TRUE;
	}
	if ( NULL != m_hSelect
		&& m_hSelect->MouseRBUp( point ) )
	{
		return TRUE;
	}
	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem 
		&& m_hSelect != hItem )
	{
		hItem->MouseRBUp( point );
	}
	return TRUE;
}


BOOL CUITab::MouseLDBClick( POINT point )
{
	if ( __super::MouseLDBClick( point ) )
	{
		return TRUE;
	}
	if ( NULL != m_hSelect
		&& m_hSelect->MouseLDBClick( point ) )
	{
		return TRUE;
	}
	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem 
		&& m_hSelect != hItem )
	{
		hItem->MouseLDBClick( point );
	}
	return TRUE;
}

BOOL CUITab::MouseRDBClick( POINT point )
{
	if ( __super::MouseRDBClick( point ) )
	{
		return TRUE;
	}
	if ( NULL != m_hSelect
		&& m_hSelect->MouseRDBClick( point ) )
	{
		return TRUE;
	}
	HTABITEM hItem = GetItemFromPoint( point );
	if ( hItem 
		&& m_hSelect != hItem )
	{
		hItem->MouseRDBClick( point );
	}
	return TRUE;
}

void CUITab::EraseItemBkGnd( HTABITEM& hTabItem,  IPainter* painter )
{
	if ( hTabItem )
	{
		RECT rcItem;
		hTabItem->GetRect( rcItem );
		IMG* pImage = hTabItem->GetBkGnd();
		if ( pImage )
		{
			painter->DrawImage( pImage, &rcItem );
		}
	}
}

CUITab::LPTABITEM CUITab::InsertItem( const RECT& rcItem, UINT nId )
{
	try
	{
		HTABITEM hTabItem = new TABITEM;
		if ( hTabItem )
		{
			hTabItem->SetRect( rcItem );
			hTabItem->SetID( nId );
			hTabItem->SetParent( this );
		}
		m_vecItems.push_back( hTabItem );
		if ( m_bAuto )
		{
			AutoPlace();
		}
		return m_vecItems.back();
	}
	catch( ... )
	{

	}
	return NULL;
}

void CUITab::AutoPlace()
{
	if ( m_vecItems.size() )
	{
		RECT rcThis;
		GetRect( rcThis );
		RECT_TO( rcThis, 0, 0 );
		RECT_INFLATE( rcThis, -m_margin.left, -m_margin.top, -m_margin.right, -m_margin.bottom );
		int nWidth = RECT_W(rcThis);
		int nHeight = RECT_H(rcThis);
		RECT rcItem = rcThis;
		if ( m_tab == HORZ )
		{
			nWidth = RECT_W(rcThis)/(int)m_vecItems.size();
			rcItem.right = rcThis.left+nWidth;
		}
		else
		{
			nHeight = RECT_H(rcThis)/(int)m_vecItems.size();
			rcItem.bottom = rcItem.top+nHeight;
		}
		for ( vector<HTABITEM>::iterator _lab = m_vecItems.begin();\
			m_vecItems.end() != _lab; ++_lab )
		{
			(*_lab)->SetRect( rcItem );
			if ( m_tab == HORZ )
			{
				rcItem.left += nWidth;
				rcItem.right = rcItem.left + nWidth;
				if ( _lab == m_vecItems.end()-1 )
				{
					rcItem.right = rcItem.right;
				}
			}
			else
			{
				rcItem.top += nHeight;
				rcItem.bottom = rcItem.top + nHeight;
				if ( _lab == m_vecItems.end()-1 )
				{
					rcItem.bottom = rcItem.bottom;
				}
			}
		}
	}
}

BOOL CUITab::RemoveItem( UINT nId )
{
	for ( vector<HTABITEM>::iterator _lab = m_vecItems.begin();\
		m_vecItems.end() != _lab; ++_lab )
	{
		if( (*_lab)->GetID() == nId )
		{
			m_vecItems.erase( _lab );
		}
	}
	return TRUE;
}

CUITab::LPTABITEM CUITab::GetItem( UINT nId )
{
	for ( vector<HTABITEM>::iterator _lab = m_vecItems.begin();\
		m_vecItems.end() != _lab; ++_lab )
	{
		if( (*_lab)->GetID() == nId )
		{
			return *_lab;
		}
	}
	return NULL;
}

DWORD CUITab::GetItemData( UINT nId )
{
	LPTABITEM pItem = GetItem( nId );
	if ( pItem )
	{
		return pItem->GetData();
	}
	return 0;
}

BOOL CUITab::SetItemData( UINT nId, DWORD dwData )
{
	LPTABITEM pItem = GetItem( nId );
	if ( pItem )
	{
		pItem->SetData( dwData );
		return TRUE;
	}
	return FALSE;
}

BOOL CUITab::ClickItem( UINT nId )
{
	HTABITEM hItem;
	for ( vector<HTABITEM>::iterator _lab = m_vecItems.begin();\
		m_vecItems.end() != _lab; ++_lab )
	{
		if( (*_lab)->GetID() == nId )
		{
			hItem = *_lab;
		}
	}
	if ( hItem
		&& m_hSelect != hItem )
	{
		RECT rcPre= { 0,0,0,0 };
		if ( m_hSelect )
		{
			m_hSelect->GetRect( rcPre );
		}
		m_hSelect = hItem;
		m_hFocus = m_hSelect;
		RECT rcInvalid;
		RECT rcItem;
		m_hSelect->GetRect( rcItem );
		if ( !RECT_ISEMPTY(rcPre) )
		{
			::UnionRect(&rcInvalid, &rcPre, &rcItem );
		}
		else
		{
			m_hSelect->GetRect( rcInvalid );
		}
		InvalidRect( &rcInvalid );
 		m_hSelect->MouseLBDown( RECT_TL(rcItem));
 		m_hSelect->MouseLBUp( RECT_TL(rcItem) );
		return TRUE;
	}
	return FALSE;
}