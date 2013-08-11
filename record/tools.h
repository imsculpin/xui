#ifndef __TOOLS_H__
#define __TOOLS_H__
__if_not_exists( tstring )
{
	#include <string>
	using std::string;
	using std::wstring;
	#if defined(_UNICODE) || defined(UNICODE)
		typedef wstring tstring;
	#else
		typedef string tstring;
	#endif 
}

__if_not_exists( CSize )
{
	class CSize:public tagSIZE
	{
	public:
		CSize( LONG x = 0, LONG y = 0 )
		{
			this->cx = x;
			this->cy = y;
		}
		void SetSize( int x, int y )
		{
			cx = x;
			cy = y;
		}

		bool operator==( const CSize& size )
		{
			return cx == size.cx
				&&cy == size.cy;
		}

		bool operator !=( const CSize& size )
		{
			return !(operator ==(size));
		}
	};

	class CPoint:public tagPOINT
	{
	public:
		CPoint( LONG _x = 0, LONG _y = 0 )
		{
			x = _x;
			y = _y;
		}
		operator LPPOINT()
		{
			return this;
		}

		void Offset( LONG _x = 0, LONG _y = 0 )
		{
			x+=_x;
			y+=_y;
		}

		void SetPoint( int _x, int _y )
		{
			x = _x;
			y = _y;
		}
	};

	class CRect:public tagRECT
	{
	public:
		CRect( LONG x = 0, LONG y=0, LONG x2=0, LONG y2=0 )
		{
			SetRect( x, y, x2, y2 );
		}
		CRect( LPCRECT lpRect )
		{
			assert( lpRect );
			left=lpRect->left;
			top=lpRect->top;
			right=lpRect->right;
			bottom=lpRect->bottom;
		}

		LONG Width()const
		{
			return right-left;
		}
		LONG Height()const
		{
			return bottom-top;
		}
		CSize Size()const
		{
			return CSize( Width(), Height() );
		}

		bool IsRectEmpty()const 
		{
			return left == right || top == bottom;
		}

		void SetRect( LONG x, LONG y, LONG x2, LONG y2 )
		{
			left = x;
			top =y;
			right = x2;
			bottom = y2;
		}
		void MoveToXY( LONG x, LONG y )
		{
			right = right-left;
			left = x;
			right = left+right;
			bottom = bottom-top;
			top = y;
			bottom = top+bottom;
		}

		void MoveToX( LONG x )
		{
			right = right-left;
			left = x;
			right = left+right;
		}

		void MoveToY( LONG y )
		{
			bottom = bottom-top;
			top = y;
			bottom = top+bottom;
		}
		void OffsetRect( LONG x = 0, LONG y = 0 )
		{
			left += x;
			right += x;
			top += y;
			bottom+=y;
		}

		void DeflateRect( LONG x = 0, LONG y = 0 )
		{
			left += x;
			right -= x;
			top += y;
			bottom -= y;
		}

		void DeflateRect( LONG x , LONG y, LONG x2, LONG y2 )
		{
			left += x;
			right -= x2;
			top += y;
			bottom -= y2;
		}
		void InflateRect( LONG x = 0, LONG y = 0 )
		{
			left -= x;
			right += x;
			top -= y;
			bottom += y;
		}

		void InflateRect( LONG x , LONG y, LONG x2, LONG y2 )
		{
			left -= x;
			right += x2;
			top -= y;
			bottom += y2;
		}

		BOOL UnionRect( LPCRECT lpRect1, LPCRECT lpRect2 )
		{
			return ::UnionRect( this, lpRect1, lpRect2 );
		}

		BOOL IntersectRect( LPCRECT lpRect1, LPCRECT lpRect2 )
		{
			return ::IntersectRect( this, lpRect1, lpRect2 );
		}

		bool PtInRect( LPPOINT lpPoint )
		{
			if( lpPoint )
			{
				if( lpPoint->x < left
					|| lpPoint->x > right
					|| lpPoint->y < top
					|| lpPoint->y > bottom )
				{
					return false;
				}
				return true;
			}
			return false;
		}
		operator LPCRECT()const
		{
			return this;
		}

		operator LPRECT()
		{
			return this;
		}

		CPoint TopLeft()const
		{
			return CPoint( left, top );
		}

		CPoint BottomRight()const
		{
			return CPoint( right, bottom );
		}
		void SetRectEmpty()
		{
			left = right = bottom = top = 0;
		}
		CPoint CenterPoint(  )const
		{
			return CPoint(left+((right-left)>>2),top+((bottom-top)>>2));
		}
	};
}
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



//显示条件原型
typedef bool (*ShowCondition)();
inline void CombineRect( CRect& rcDest, const CRect& rcSrc )
{
	if ( rcDest.left>rcSrc.left )
	{
		rcDest.left = rcSrc.left;
	}
	if ( rcDest.right<rcSrc.right )
	{
		rcDest.right = rcSrc.right;
	}
	if ( rcDest.top>rcSrc.top )
	{
		rcDest.top = rcSrc.top;
	}
	if ( rcDest.bottom<rcSrc.bottom )
	{
		rcDest.bottom = rcDest.bottom;
	}
}
#ifndef T2W
inline std::wstring T2W( const tstring& tstr )
{
#if defined( UNICODE )||defined(_UNICODE)
	return tstr;
#else
	std::wstring strResult;
	int nBuffer = ::MultiByteToWideChar( CP_ACP, 0, tstr.c_str(), -1, NULL, 0 );
	++nBuffer;
	wchar_t* pBuffer = new wchar_t[nBuffer];
	::memset( pBuffer, 0, nBuffer*sizeof(wchar_t) );
	::MultiByteToWideChar( CP_ACP, 0, tstr.c_str(), -1, pBuffer, nBuffer );
	strResult = pBuffer;
	delete pBuffer;
	pBuffer = NULL;
	return strResult;
#endif
}
#endif

#ifndef W2T
inline tstring W2T ( const wstring& tstr )
{
#if defined( UNICODE )||defined(_UNICODE)
	return tstr;
#else
	std::string strResult;
	int nBuffer = ::WideCharToMultiByte( CP_ACP, 0, tstr.c_str(), -1, NULL, 0, NULL, NULL );
	++nBuffer;
	char* pBuffer = new char[nBuffer];
	::memset( pBuffer, 0, nBuffer*sizeof(char) );
	::WideCharToMultiByte( CP_ACP, 0, tstr.c_str(), -1, pBuffer, nBuffer, NULL, NULL );
	strResult = pBuffer;
	delete pBuffer;
	pBuffer = NULL;
	return strResult;
#endif
}
#endif

#ifndef A2T
inline tstring A2T ( const string& tstr )
{
#if defined( UNICODE )||defined(_UNICODE)
	std::wstring strResult;
	int nBuffer = ::MultiByteToWideChar( CP_ACP, 0, tstr.c_str(), -1, NULL, 0 );
	++nBuffer;
	wchar_t* pBuffer = new wchar_t[nBuffer];
	::memset( pBuffer, 0, nBuffer*sizeof(wchar_t) );
	::MultiByteToWideChar( CP_ACP, 0, tstr.c_str(), -1, pBuffer, nBuffer );
	strResult = pBuffer;
	delete pBuffer;
	pBuffer = NULL;
	return strResult;
#else
	return tstr;
#endif
}
#endif

#ifndef T2A
inline std::string T2A( const tstring& tstr )
{
#if defined( UNICODE )||defined(_UNICODE)
	std::string strResult;
	int nBuffer = ::WideCharToMultiByte( CP_ACP, 0, tstr.c_str(), -1, NULL, 0, NULL, NULL );
	++nBuffer;
	char* pBuffer = new char[nBuffer];
	::memset( pBuffer, 0, nBuffer*sizeof(char) );
	::WideCharToMultiByte( CP_ACP, 0, tstr.c_str(), -1, pBuffer, nBuffer, NULL, NULL );
	strResult = pBuffer;
	delete pBuffer;
	pBuffer = NULL;
	return strResult;
#else
	return tstr;
#endif
}
#endif

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


typedef UINT (WINAPI *THREADPROC)(LPVOID);
class CThread
{
public:
	// Constructors
	CThread();
	BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0,
		LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

	// only valid while running
	HANDLE m_hThread;       // this thread's HANDLE
	operator HANDLE() const;
	DWORD m_nThreadID;      // this thread's ID

	int GetThreadPriority();
	BOOL SetThreadPriority(int nPriority);

	// Operations
	DWORD SuspendThread();
	DWORD ResumeThread();
	BOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);

	// Overridables
	// thread initialization
	virtual BOOL InitInstance();

	// running and idle processing
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
	virtual BOOL IsIdleMessage(MSG* pMsg);  // checks for special messages
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// thread termination
	virtual int ExitInstance(); // default will 'delete this'

	// Advanced: virtual access to m_pMainWnd
	virtual HWND GetMainWnd()
	{
		return m_hMainWnd;
	}

	// Implementation
public:
	virtual ~CThread();
	virtual void Delete();
	// 'delete this' only if m_bAutoDelete == TRUE

public:
	// constructor used by implementation of AfxBeginThread
	CThread(THREADPROC pfnThreadProc, LPVOID pParam);

	// Attributes
		
	HWND m_hMainWnd;       // main window (usually same AfxGetApp()->m_pMainWnd)
	HWND m_hActiveWnd;     // active main window (may not be m_pMainWnd)
	BOOL m_bAutoDelete;     // enables 'delete this' after thread termination
	
	// valid after construction
	LPVOID	   m_pThreadParams; // generic parameters passed to starting function
	THREADPROC m_pfnThreadProc;
private:
	MSG*    m_pMsgLast;
	MSG*    m_pMsgCur;
};

inline bool IsSystemGreaterOrEqual2000(  )
{
	OSVERSIONINFO version = {0};
	version.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	if(GetVersionEx( &version ))
	{
		return version.dwMajorVersion >= 5;
	}
	return false;
}

inline bool IsSystemGreaterOrEqualXp(  )
{
	OSVERSIONINFO version = {0};
	version.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	if(GetVersionEx( &version ))
	{
		return version.dwMajorVersion > 5 ||( (version.dwMajorVersion == 5)&&(version.dwMinorVersion>0) );
	}
	return false;
}
#endif