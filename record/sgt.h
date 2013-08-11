#ifndef __LC_SINGLETON_H__
#define __LC_SINGLETON_H__
#include <tchar.h>
#include <assert.h>
#ifndef LOG
#define LOG
#endif
typedef struct _tagSingletonHandle{}*HSINGLETON;

template< class T >
class CXSingleton
{  
public:
	CXSingleton()
	{
		_AddRef();
		if ( NULL == m_pInstance )
		{
			try
			{
				m_pInstance = new T();
			}
			catch( ... )
			{
				LOG( _T("out of memory!") );
			}
			assert( NULL != m_pInstance );
		}
	};

	~CXSingleton()
	{ 
		_ReleaseRef();
	}

public:
	operator HSINGLETON()
	{
		return (HSINGLETON)m_pInstance;
	}

// 	T* operator &()
// 	{
// 		return m_pInstance;
// 	}

// 	const T* operator &()const
// 	{
// 		return m_pInstance;
// 	}

	T& operator *()
	{
		return *m_pInstance;
	}

	const T& operator *()const
	{
		return *m_pInstance;
	}

	T* operator ->()
	{
		assert( NULL != m_pInstance );
		return m_pInstance;
	}

	const T* operator ->()const
	{
		assert( NULL != m_pInstance );
		return m_pInstance;
	}

protected:
	unsigned long _AddRef()
	{
		return InterlockedIncrement( (LONG*)&m_nRefCount );
	}

	unsigned long _ReleaseRef()
	{
		if ( InterlockedDecrement( (LONG*)&m_nRefCount ) <= 0 )
		{
			if ( m_pInstance )
			{
				delete  m_pInstance;
				m_pInstance = NULL;
				m_nRefCount = 0;
			}
		}
		return m_nRefCount;
	}

private:
	char			  byte; 
	static T*		  m_pInstance;
	static unsigned long	  m_nRefCount;
};

#define  DECLARE_SGT_ATTRIBUTE()	template< class _class>friend  class CXSingleton

#define  DECLARE_SGT_CLASS( _class ) typedef CXSingleton<_class> SGT_##_class

template< class T >
T* CXSingleton<T>::m_pInstance = NULL;

template<class T>
unsigned long CXSingleton<T>::m_nRefCount = 0;

#endif