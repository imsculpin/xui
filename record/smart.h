#ifndef __SMART_DEFINE_H__
#define __SMART_DEFINE_H__
#include "Switch.h"
#include<tchar.h>
#include <assert.h>
#include "strfun.h"
#pragma warning( disable:4541 )
typedef struct SmartHandle{}*HSMARTPTR;

template <class T >
class Deleter
{
public:
	virtual void operator() ( T*& ptr )
	{
		return;
	}
};

template <class T >
class ReleaseDeleter:public Deleter<T>
{
public:
	virtual void operator() ( T*& ptr )
	{
		__if_exists( T::Release )
		{
			if ( ptr )
			{
				ptr->Release();
			}
			ptr = NULL;
		}
#ifdef _DEBUG
		__if_not_exists( T::Release )
		{
			OutputDebugString( _T("\nWarning: ") );
			OutputDebugString( XA2T(typeid( T ).name()).c_str() );
			OutputDebugString( _T("::Release不存在,但却被调用!\n") );
		}
#endif
	}
};

template <class T >
class DeteleDeleter:public Deleter<T>
{
public:
	virtual void operator() ( T*& ptr )
	{
#ifdef _DEBUG
		__if_exists( T::Release )
		{
			OutputDebugString( _T("\nWarning: ") );
			OutputDebugString( XA2T(typeid( T ).name()) );
			OutputDebugString( _T("::Release存在,但却调用delete方法析构!\n") );
		}
#endif
		if ( ptr )
		{
			delete ptr;
			ptr = NULL;
		}
	}
};

template <class T >
class FreeDeleter:public Deleter<T>
{
public:
	virtual void operator() ( T*& ptr )
	{
#ifdef _DEBUG
		__if_exists( T::Release )
		{
			OutputDebugString( _T("\nWarning: ") );
			OutputDebugString( XA2T(typeid( *ptr ).name()) );
			OutputDebugString( _T("::Release存在,但却调用free方法析构!\n") );
		}
#endif
		if ( ptr )
		{
			free((void*)ptr);
			ptr = NULL;
		}
	}
};

template <class T >
class DestroyImageDeleter:public Deleter<T>
{
public:
	virtual void operator() ( T*& ptr )
	{
#ifdef	_DEBUG
		__if_exists( T::Release )
		{
			OutputDebugString( _T("\nWarning: ") );
			OutputDebugString( XA2T(typeid( *ptr ).name()) );
			OutputDebugString( _T("::Release存在,但却调用DestroyImage方法析构!\n") );
		}
#endif

		if ( ptr )
		{
			DestroyImage( ptr );
			ptr = NULL;
		}
	}
};

template< class T >
T* Alloc( )
{
	T* p = (T*)malloc( sizeof(T) );
	return p;
}

template< class T, class deleter =Deleter<T>  >
class SmartPtr
{  
public:
	SmartPtr()
		:m_pInstance(NULL)
		,m_pRefCount(NULL)
	{

	};

	SmartPtr( T* ptr )
	{
		m_pRefCount = NULL;
		m_pInstance = ptr;
		if( m_pInstance )
		{
			_AddRef();
		}
	}

	SmartPtr( int null )
		:m_pInstance(NULL)
		,m_pRefCount(NULL)
	{
		assert( 0==null );
	}

	SmartPtr( const SmartPtr& smartObj )
		:m_pInstance( NULL )
		,m_pRefCount( NULL )
	{
		if ( smartObj )
		{
			m_pInstance = smartObj.m_pInstance;
			m_pRefCount = smartObj.m_pRefCount;
			if ( NULL == m_pInstance
				|| NULL == m_pRefCount )
			{
				m_pRefCount = NULL;
				m_pInstance = NULL;
				return;
			}
			_AddRef();
		}
	}

	SmartPtr& operator = ( const SmartPtr& smartObj )
	{
		if ( smartObj.m_pInstance == m_pInstance )
		{
			return *this;
		}
		_ReleaseRef();
		m_pInstance = smartObj.m_pInstance;
		m_pRefCount = smartObj.m_pRefCount;
		if ( NULL == m_pInstance
			|| NULL == m_pRefCount )
		{
			m_pRefCount = NULL;
			m_pInstance = NULL;
			return *this;
		}
		_AddRef();
		return *this;
	}

	SmartPtr& operator = ( int null )
	{
		assert( 0 == null );
		_ReleaseRef();
		return *this;
	}

	SmartPtr& operator = ( const T* ptr )
	{
		if ( ptr == m_pInstance )
		{
			return *this;
		}
		_ReleaseRef();
		if ( ptr )
		{
			m_pInstance = const_cast<T*>(ptr);
			_AddRef();
		}
		return *this;
	}

	~SmartPtr()
	{ 
		_ReleaseRef();
	}

public:
	operator HSMARTPTR()
	{
		return (HSMARTPTR)m_pInstance;
	}

//  	T* operator &()
//  	{
//  		assert( m_pInstance );
//  		return m_pInstance;
//  	}

	operator T*()
	{
		return m_pInstance;
	}
	
	operator const T*()const
	{
		return m_pInstance;
	}

//  	const T* operator &()const
//  	{
//  		assert( m_pInstance );
//  		return m_pInstance;
//  	}

	operator bool()
	{
		return NULL != m_pInstance;
	}

	operator bool()const
	{
		return NULL != m_pInstance;
	}

	T& operator *()
	{
		assert( m_pInstance );
		return *m_pInstance;
	}

	const T& operator *()const
	{
		assert( m_pInstance );
		return *m_pInstance;
	}

	bool operator &&( bool _bool )const
	{
		return (NULL != m_pInstance)&&_bool;
	}

	bool operator ||( bool _bool )const
	{
		return (NULL != m_pInstance)||_bool;
	}

	T* operator ->()
	{
		assert( m_pInstance );
		return m_pInstance;
	}

	const T* operator ->()const
	{
		assert( m_pInstance );
		return m_pInstance;
	}

	bool operator ! (  )const 
	{
		return !m_pInstance;
	}

	bool operator < ( const SmartPtr& _Right )
	{
		if ( NULL == m_pInstance
			|| NULL == _Right.m_pInstance )
		{
			return false;
		}
		return *m_pInstance < *m_pInstance;
	}

	const bool operator < ( const SmartPtr& _Right )const 
	{
		if ( NULL == m_pInstance
			|| NULL == _Right.m_pInstance )
		{
			return false;
		}
		return *m_pInstance < *m_pInstance;
	}

	bool operator == ( const SmartPtr& _Right )
	{
		return m_pInstance == _Right.m_pInstance;
	}

	const bool operator == ( const SmartPtr& _Right )const 
	{
		return m_pInstance == _Right.m_pInstance;
	}

	bool operator == ( int null /*= NULL*/ )
	{
		assert( null == NULL );
		return m_pInstance == NULL;
	}

	const bool operator == ( int null /*= NULL*/ )const 
	{
		assert( null == NULL );
		return m_pInstance == NULL;
	}

	bool operator != ( const SmartPtr& _Right )
	{
		return !(*this==_Right);
	}

	const bool operator != ( const SmartPtr& _Right )const 
	{
		return !(*this==_Right);
	}

	bool operator != ( int null /*= NULL*/ )
	{
		assert( null == NULL );
		return m_pInstance != NULL;
	}

	const bool operator != ( int null /*= NULL*/ )const 
	{
		assert( null == NULL );
		return m_pInstance != NULL;
	}

	unsigned long AddRef()
	{
		return _AddRef();
	}

	unsigned long Release()
	{
		return _ReleaseRef();
	}
	
	BOOL Attach( T* p )
	{
		_ReleaseRef();
		m_pInstance = p;
		return TRUE;
	}

	T* Detach(  )
	{
		T* pInstance = m_pInstance;
		m_pInstance = NULL;
		m_pRefCount = NULL;
		return pInstance;
	}
protected:
	unsigned long _AddRef()
	{
		if ( NULL == m_pRefCount && m_pInstance )
		{
			try
			{
				m_pRefCount = new unsigned long;
				*m_pRefCount = 0;
			}
			catch( ... )
			{
				return 0;
			}

		}
		return InterlockedIncrement( (LONG*)m_pRefCount );
	}

	unsigned long _ReleaseRef()
	{
		if ( m_pInstance && m_pRefCount )
		{
			if ( InterlockedDecrement( (LONG*)m_pRefCount ) <= 0 )
			{ 
				deleter _delete;
				_delete( m_pInstance );
				*m_pRefCount = 0;
				delete m_pRefCount;
				m_pRefCount = NULL;
				return 0;
			}
			unsigned long count = *m_pRefCount;
			m_pRefCount = NULL;
			m_pInstance = NULL;
			return count;
		}
		m_pRefCount = NULL;
		m_pInstance = NULL;
		return 0;
	}

private:
	char	  byte; 
	T*		  m_pInstance;
	unsigned long*	  m_pRefCount;
};
#define  __DEFINE_SMART_TYPE( Type ) typedef SmartPtr<Type> Type##Ptr
#define  __DEFINE_SMART_TYPE_NEED_DELETER( Type, _deleter ) typedef SmartPtr<Type,_deleter > Type##Ptr
#define  __DEFINE_SMART_HANDLE( Type, handlename ) typedef SmartPtr<Type> handlename
#define  __DEFINE_SMART_HANDLE_NEED_DELETER( Type, handlename, _deleter ) typedef SmartPtr<Type,_deleter > handlename
#endif