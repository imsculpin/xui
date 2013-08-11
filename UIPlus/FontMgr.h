#ifndef __FONT_MGR_DEFINE__
#define __FONT_MGR_DEFINE__
#include "sgt.h"
#include <vector>
#include <GdiPlus.h>
using namespace Gdiplus;
#include "strfun.h"
#include "UIPlus.h"
using std::vector;
#define FACEREF(fontParam) (*((BYTE*)&fontParam))
#define STYLEREF(fontParam)(*(((BYTE*)&fontParam)+1))
#define SIZEREF(fontParam) (*(((SHORT*)&fontParam)+1))

class CFontMgr
{
public:
	CFontMgr()
		:m_nDefaultStyle(0)
		,m_nDefalutSize(12)
		,m_strDefaultFont(_T("ËÎÌו"))
		,m_hDefaultFont(NULL)
	{}
	~CFontMgr()
	{
		m_vecFaces.clear();
		m_vecFontsIndex.clear();
		for ( int _i = 0; _i < (int)m_vecFont.size(); ++_i )
		{
			DeleteObject(m_vecFont.at( _i ));
		}
		m_vecFont.clear();
	}
	CFontMgr(const CFontMgr& ){};
	
	HFONT GetDefault(  );
	void SetDefault( int iHeight, LPCTSTR lpszFontFace, UINT style = F_NORMAL )
	{
		m_nDefalutSize = iHeight;
		m_nDefaultStyle = style;
		if ( lpszFontFace )
		{
			m_strDefaultFont = lpszFontFace;
		}
		m_hDefaultFont = GetFont( iHeight, lpszFontFace, style );
	}
	HFONT GetFont( int iHeight, LPCTSTR lpszFontFace = NULL, UINT style = F_NORMAL );
	HFONT CreateFont( int iHeight, LPCTSTR lpszFontFace, UINT style = F_NORMAL );
protected:
	int	  GetFaceIndex( LPCTSTR lpszFontFace )
	{
		if ( lpszFontFace )
		{
			for ( int _i = 0; _i < (int)m_vecFaces.size(); ++_i )
			{
				if ( _tcsnicmp(m_vecFaces[_i].c_str(), lpszFontFace, _tcslen(lpszFontFace)) == 0 )
				{
					return _i;
				}
			}
			m_vecFaces.push_back( lpszFontFace );
			return (int)m_vecFaces.size()-1;
		}
		lpszFontFace = m_strDefaultFont.c_str();
		return GetFaceIndex( lpszFontFace );
	}
private:
	vector<tstring> m_vecFaces;
	vector<DWORD>   m_vecFontsIndex;
	vector<HFONT>	m_vecFont;
	tstring			m_strDefaultFont;
	int				m_nDefalutSize;
	int				m_nDefaultStyle;
	HFONT			m_hDefaultFont;
};
#endif