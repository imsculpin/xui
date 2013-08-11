
#include "UIListCtrl.h"
BOOL CUIList::InsertItem( const HXITEM& _item )
{
	HXITEM hItem = _item;
	hItem->SetParent(m_parent);
	if ( hItem->m_nId < 0 
		&& !m_parent->IsVirtualMode() )
	{
		hItem->m_nId = (INT32)m_vecShowBuffer.size();
	}
	m_vecShowBuffer.push_back( hItem );
	return TRUE;
}

CUIList::Iter CUIList::InsertItem( Iter& _before, HXITEM _item )
{
	if ( _before == End() )
	{
		if( InsertItem( _item ) )
		{
			return --End();
		}
		return End();
	}
	if ( IsValid( _before ) )
	{
		if ( _item )
		{
			_item->SetParent(m_parent);
			_item->m_nId = (*_before)->m_nId;
			Iter _insert = m_vecShowBuffer.insert( _before, _item );
			if ( IsValid( _insert ) )
			{
				for ( Iter _lab = _insert+1; _lab != End(); ++_lab )
				{
					(*_lab)->m_nId = ( *(_lab-1) )->m_nId+1;
				}
			}
			return _insert;
		}
	}
	return End();
}