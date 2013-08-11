#ifndef __UI_STRFUN_H__
#define __UI_STRFUN_H__
#include <string>
#include <Windows.h>
using std::string;
using std::wstring;
__if_not_exists( tstring )
{
#if defined(_UNICODE) || defined(UNICODE)
	typedef wstring tstring;
#else
	typedef string tstring;
#endif 
}

inline std::wstring XT2W( const tstring& tstr )
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

inline tstring XW2T ( const wstring& tstr )
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

inline tstring XA2T ( const string& tstr )
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

inline std::string XT2A( const tstring& tstr )
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