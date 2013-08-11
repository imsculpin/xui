#include "Switch.h"
#include "FontMgr.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif

HFONT CFontMgr::GetFont( int iHeight, LPCTSTR lpszFontFace /*= NULL*/, UINT style /*= F_NORMAL */ )
{
	HFONT hFontRet = NULL;
	DWORD dwFontIndex = 0;
	SIZEREF( dwFontIndex ) = (SHORT)iHeight;
	STYLEREF( dwFontIndex ) = (BYTE)style;
	//查找字体类型索引
	FACEREF(dwFontIndex)=GetFaceIndex( lpszFontFace );
	//查找字体索引并返回字体
	for ( int _j = 0; _j < (int)m_vecFontsIndex.size(); ++_j )
	{
		if ( dwFontIndex == m_vecFontsIndex[_j] )
		{
			if ( (int)m_vecFont.size() > _j )
			{
				hFontRet = m_vecFont[_j];
			}
			else
			{
				//补充空缺的字体位置
				m_vecFont.insert( m_vecFont.end(), _j-m_vecFont.size()+1, NULL );
			}
			if ( NULL == hFontRet 
				|| ::GetObjectType(hFontRet) == 0 )
			{
				hFontRet = CreateFont( iHeight, lpszFontFace, style );
				m_vecFont.at( _j ) = hFontRet;
			}
			return hFontRet;
		}
	}
	//未找到字体时,创建字体
	m_vecFontsIndex.push_back( dwFontIndex );
	hFontRet = CreateFont( iHeight, lpszFontFace, style );
	m_vecFont.push_back( hFontRet );
	return hFontRet;
}

HFONT CFontMgr::GetDefault()
{
	if ( NULL == m_hDefaultFont )
	{
		return GetFont( m_nDefalutSize, m_strDefaultFont.c_str(), m_nDefaultStyle );
	}
	return m_hDefaultFont;
}

HFONT CFontMgr::CreateFont( int iHeight, LPCTSTR lpszFontFace, UINT style /*= F_NORMAL */ )
{
	HDC hGlobal = GetDC( NULL );
	if ( hGlobal )
	{
		if ( NULL == lpszFontFace )
		{
			lpszFontFace = m_strDefaultFont.c_str();
		}
		Graphics graphics( hGlobal );
		Gdiplus::FontFamily fontFamily( XT2W( lpszFontFace ).c_str() );
		INT nStyle = FontStyleRegular;
		if ( style & F_BLOD )
		{
			nStyle |= FontStyleBold;
		}
		if ( style & F_ITALIC )
		{
			nStyle |= FontStyleItalic;
		}
		if ( style & F_UNDERLINET )
		{
			nStyle |= FontStyleUnderline;
		}
		if ( style & F_STRIKEOUT )
		{
			nStyle |= FontStyleStrikeout;
		}
		Gdiplus::Font font( &fontFamily, (REAL)iHeight, nStyle, UnitPixel );
		LOGFONT lfFont = { 0 };
#if			defined(_UNICODE) || defined(UNICODE)
		font.GetLogFontW( &graphics, &lfFont );
#else
		font.GetLogFontA( &graphics, &lfFont );
#endif
		ReleaseDC( NULL, hGlobal );
		return CreateFontIndirect( &lfFont );
	}
	return NULL;
}