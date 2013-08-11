#ifndef __UI_DB_H__
#define __UI_DB_H__
#include "UIDefine.h"
#define  WM_DB_DATACHANGED		WM_USER + 2
class __declspec(novtable) IDataInterface
{
public:
	IDataInterface()
		:m_pRelatedUI(NULL)
	{

	}
	virtual void SetRelatedUI( CUIView* pUI )
	{
		m_pRelatedUI = pUI;
	}

	virtual void Reflash( )
	{
		if ( m_pRelatedUI )
		{
			m_pRelatedUI->DBChanged( );
			m_pRelatedUI->InvalidRect(NULL);
		}
	}
	virtual LPVOID Get( int nIndex );
	virtual BOOL   Get( int nIndex, LPVOID pData );
	virtual INT32 size(  );

protected:
	CUIView* m_pRelatedUI;
};

template <class T>
class CDBase:public IDataInterface
{
public:
	typedef typename vector<T>  DBASE;
	virtual INT32 size()
	{
		return (INT32)m_database.size();
	}

	DBASE& GetDBaseRef()
	{
		return m_database;
	}

	void Delete( INT32 nIndex )
	{
		if ( VALUE_IN_RANGE( nIndex, 0, size()-1 ) )
		{
			DBASE::iterator rowDelete = m_database.begin()+nIndex;
			m_database.erase( rowDelete );
		}
		else if( -1 == nIndex )
		{
			m_database.pop_back();
		}
	}

	void Add( INT32 nIndex, const T& row )
	{
		if ( VALUE_IN_RANGE( nIndex, 0, size()-1 ) )
		{
			DBASE::iterator _where = m_database.begin()+nIndex;
			m_database.insert( m_database.begin()+nIndex, row  );
		}
		else if( -1 == nIndex )
		{
			m_database.push_back( row );
		}
	}

	void Delete( const RANGE<>& rowRange )
	{
		//no impl
	}

	void Add( const DBASE& db )
	{
		if ( !db.empty() )
		{
			(void)copy( db.begin(), db.end(), m_database.end() );
		}
		return;
	}
	
	void Clear()
	{
		m_database.clear();
	}

	LPVOID Get( int nIndex )
	{
		if ( VALUE_IN_RANGE( nIndex, 0, size()-1 ) )
		{
			return (LPVOID)&m_database[nIndex];
		}
		return NULL;
	}

	virtual BOOL   Get( int nIndex, LPVOID pData )
	{
		if ( !VALUE_IN_RANGE( nIndex, 0, size()-1 )
			|| NULL == pData )
		{
			return FALSE;
		}
		DBASE::value_type* pValue = reinterpret_cast<DBASE::value_type*>(pData);
		*pValue = m_database[nIndex];
		return TRUE;
	}

private:
	DBASE m_database;
};
#endif