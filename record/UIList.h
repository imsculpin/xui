#ifndef __UI_LIST_H__
#define __UI_LIST_H__
#include "UIDefine.h"
typedef HLAYPANEL HXITEM;
typedef CUILayPanel XItem;
class CUIListCtrl;
class CUIList
{
public:
	CUIList( CUIListCtrl* parent = NULL )
		:m_parent( parent )
	{

	}
	void Parent( CUIListCtrl* parent )
	{
		m_parent = parent;
	}
	CUIListCtrl* Parent(  )
	{
		return m_parent;
	}
public:
	typedef  std::vector< HXITEM > LIST;
	typedef  HXITEM				LPITEM;
	typedef  const HXITEM		    LPCITEM;
	typedef  XItem				    ITEM;
	typedef  LIST::iterator		Iter;
	typedef  LIST::const_iterator  Const_Iter;

public:
	HXITEM InsertItem( const XItem& item )
	{		
		try
		{
			HXITEM hItem = new ITEM(item);
			if( InsertItem( hItem ) )
			{
				return hItem;
			}
			return NULL;
		}
		catch (...)
		{
			
		}
		return NULL;
	}

	BOOL InsertItem( const HXITEM& _item );

	template< class _Fun >
	HXITEM FoundItem( _Fun founder, DWORD dwParam )
	{
		for ( LIST::iterator _beg = m_vecShowBuffer.begin();
			_beg != m_vecShowBuffer.end(); ++_beg )
		{
			if ( founder( *_beg, dwParam ) )
			{
				return *_beg;
			}
		}
		return NULL;
	}
	
	Iter Hxitem2Iter( const HXITEM& hItem )
	{
		return find( m_vecShowBuffer.begin(), m_vecShowBuffer.end(), hItem );
	}

	INT32 Size()const
	{
		return (INT32)m_vecShowBuffer.size();
	}

	HXITEM First()
	{
		if ( !m_vecShowBuffer.empty() )
		{
			return *m_vecShowBuffer.begin();
		}
		return NULL;
	}

	HXITEM Last( )
	{
		if ( !m_vecShowBuffer.empty() )
		{
			return *m_vecShowBuffer.rbegin();
		}
		return NULL;
	}

	HXITEM Next( const HXITEM& hItem )
	{
		LIST::iterator _itFound = find( m_vecShowBuffer.begin(), m_vecShowBuffer.end(), hItem );
		if ( _itFound != m_vecShowBuffer.end()
			&& ++_itFound != m_vecShowBuffer.end() )
		{
			return *_itFound;
		}
		return NULL;
	}

	void RemoveItem( const HXITEM& hItem )
	{
		RemoveItem( Hxitem2Iter( hItem ) );
	}
	
	void RemoveAll( )
	{
		m_vecShowBuffer.clear();
	}

	bool IsEmpty()
	{
		return m_vecShowBuffer.empty();
	}

public:
	Iter Begin(  )
	{
		return m_vecShowBuffer.begin();
	}

	Iter Next( Iter& iter )
	{
		if ( IsValid( iter ) )
		{
			return End();
		}
		return ++iter;
	}

	Const_Iter Next( Const_Iter& iter )const
	{
		if ( IsValid( iter ) )
		{
			return End();
		}
		return ++iter;
	}

	Iter End(  )
	{
		return m_vecShowBuffer.end();
	}

	Const_Iter Begin(  )const 
	{
		return m_vecShowBuffer.begin();
	}

	Iter Offset( Iter& iter, int _offset )
	{
		try
		{
			if ( IsValid( iter )
				&& IsValid( iter+_offset ) )
			{
				return iter+_offset;
			}
		}
		catch( ... )
		{

		}
		return End();
	}

	Const_Iter Offset( Const_Iter& iter, int _offset )const
	{
		try
		{
			if ( IsValid( iter )
				&& IsValid( iter+_offset ) )
			{
				return iter+_offset;
			}
		}
		catch( ... )
		{

		}
		return End();
	}

	Const_Iter End(  )const
	{
		return m_vecShowBuffer.end();
	}

	Iter InsertItem( Iter& _before, const XItem& _item )
	{
		try
		{
			HXITEM hItem = new ITEM(_item);
			if ( hItem )
			{
				return InsertItem( _before, hItem );
			}
		}
		catch( ... )
		{

		}
		return End();
	}

	Iter InsertItem( Iter& _before, HXITEM _item );
	BOOL RemoveItem( Iter _Remove )
	{
		if ( IsValid( _Remove ) )
		{
			Iter _Next = _Remove;
			while ( ++_Next != End() )
			{
				if ( *_Next )
				{
					--(*_Next)->m_nId;
				}
			}
			m_vecShowBuffer.erase( _Remove );
			return TRUE;
		}
		return FALSE;
	}

	bool IsValid( const Iter& iter )
	{
		if( iter >= Begin()
			&& iter < End() )
		{
			return true;
		}
		return false;
	}

	bool IsValid( Const_Iter& iter )const
	{
		if( iter >= Begin()
			&& iter < End() )
		{
			return true;
		}
		return false;
	}

private:
	LIST				m_vecShowBuffer;
	CUIListCtrl*				m_parent;
};
#endif