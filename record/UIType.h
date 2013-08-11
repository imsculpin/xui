#ifndef __UI_TYPE_H__
#define __UI_TYPE_H__
#include "Switch.h"
#include "strfun.h"
#include "ImagingPlus.h"
#include "UIdefine.h"
#ifdef _AFXEXT
#include <afxmt.h>
#endif

#define VALUE_IN_RANGE( value, min, max )   (( value >= min )&&( value <= max ))
#define VALUE_OUT_RANGE( value, min, max )  !VALUE_IN_RANGE(value, min, max)
#define VALUE_LOW( value1, value2 )         ( value1 < value2 )
#define VALUE_HIGHT( value1, value2 )       ( value1 > value2 )
#define VALUE_EQUAL( value1, value2 )       ( value1 == value2 )
#define VALUE_HIGHT_EQUAL( value1, value2 ) !VALUE_LOW( value1, value2 )
#define VALUE_LOW_EQUAL( value1, value2 )   !VALUE_HIGHT( value1, value2 )

template< class T = long >
class RANGE
{
public:
	T m_min;
	T m_max;
	RANGE()
		//  		__if_not_exists( T::T )
		//  		{
		:m_min(-1)
		,m_max(-1)
		/* 		}*/
	{

	}
	RANGE( const T& min, const T& max )
	{
		m_min = min;
		m_max   = max;
	}
	RANGE( const RANGE& range )
	{
		m_min = range.m_min;
		m_max   = range.m_max;
	}
	RANGE& operator= ( const RANGE& range )
	{
		m_min = range.m_min;
		m_max   = range.m_max;
		return *this;
	}

	bool operator==( const RANGE& range )const
	{
		if( m_min == range.m_min
			&& m_max == range.m_max )
		{
			return true;
		}
		return false;
	}

	bool operator!=( const RANGE& range )const
	{
		if( m_min != range.m_min
			|| m_max != range.m_max )
		{
			return true;
		}
		return false;
	}

	BOOL DataInRange( const T& nData )const
	{
		return IsValid()&&VALUE_IN_RANGE( nData, m_min, m_max );
	}

	T Distance()const
	{
		return m_max-m_min;
	}

	RANGE& operator ++(  )
	{
		++m_min;
		++m_max;
		return *this;
	}

	RANGE& operator ++( int )
	{
		m_min++;
		m_max++;
		return *this;
	}

	RANGE& operator --(  )
	{
		--m_min;
		--m_max;
		return *this;
	}

	RANGE& operator --( int )
	{
		m_min--;
		m_max--;
		return *this;
	}

	RANGE Intersect( const RANGE& range )
	{
		if ( IsValid() )
		{
			if ( DataInRange( range.m_min ) )
			{
				return RANGE( range.m_min, min( m_max, range.m_max ) );
			}
			if ( DataInRange( range.m_max ) )
			{
				return RANGE( max( m_min, range.m_min ), range.m_max );
			}
		}
		return RANGE();
	}

	BOOL IsValid( )const
	{
		return !((VALUE_EQUAL( m_min, m_max )&&VALUE_EQUAL( -1, m_min ))\
			|| m_max < m_min );
	}

	void Reset()
	{
		__if_exists( T::T )
		{
			m_min = m_max = T();
		}
		__if_not_exists( T::T )
		{
			m_min = m_max = -1;
		}
	}
};

__if_not_exists( CCriticalSection )
{
	class CCriticalSection
	{
	public:
		CCriticalSection()
		{
			InitializeCriticalSection( &m_sect );
		}

		virtual ~CCriticalSection()
		{
			DeleteCriticalSection( &m_sect );
		}
		// Attributes
	public:
		operator CRITICAL_SECTION*()
		{
			return &m_sect;
		}

		// Operations
	public:
		BOOL Unlock()
		{
			LeaveCriticalSection( &m_sect );
			return TRUE;
		}
		BOOL Lock()
		{
			EnterCriticalSection( &m_sect );
			return TRUE;
		}

	private:
		CRITICAL_SECTION m_sect;
	};

}


//显示条件原型
typedef bool (*ShowCondition)();
//Callback 原型
typedef BOOL (*UICALLBACK)( UINT message, WPARAM wParam, LPARAM lParam, DWORD dwParam );

//NMHDR
//XWM_MESSAGE消息的LPARAM结构
struct NM_PARAM
{
	UINT		  nCtrlId;		//控件的ID
	UINT		  nParentId;	//控件的父ID
	union
	{
		WPARAM    wVirtualKey;
		INT 	  nIndex;
		WPARAM    wParam;
	};
	LPARAM   lParam;
	POINT    point;
};
//UICtrl消息

#endif