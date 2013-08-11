#pragma once
#include <map>
#include "CPTDC.h"
#include "UIPlus.h"
#include <vector>
#include "UIType.h"
using std::vector;
using std::map;
class CUIPopMenu
{
public:
	CUIPopMenu();
	~CUIPopMenu();
	UINT TrackPopupMenu( MENUDATA* pMenuData, int nCount, int x, int y, int fuFlags, ACCEL* pAccels = NULL, int nAccelCount = 0 );
	void EndMenu( bool bOnlyFree = false );
protected:
	BOOL CreateMenu( HWND hWndParent = NULL  );
	static LRESULT WINAPI WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual LRESULT WinProc( UINT message, WPARAM wParam, LPARAM lParam );
	void DrawMenu( IPainter* painter, LPCRECT lpRectClip = NULL );
	void ShowSubMenu(  );
	void GetTopLeft( int &x, int& y, UINT fuFlags );
	static CUIPopMenu* FromHandle( HWND hWnd )
	{
		CUIPopMenu* pWin = NULL;
		if ( m_mapWindows.size() )
		{
			map<HWND, CUIPopMenu*>::iterator _itFound = m_mapWindows.find( hWnd );
			if( _itFound != m_mapWindows.end() )
			{
				return _itFound->second;
			}
		}
		return NULL;
	}

	bool IsSubMenuGetFocus( HWND hFocus )
	{
		if ( m_subMenu )
		{
			if ( m_subMenu->m_hWnd == hFocus )
			{
				return true;
			}
			return m_subMenu->IsSubMenuGetFocus( hFocus );
		}
		return false;
	}

	//刷新指定区域
	void InvalidRect( LPRECT lpRect );
	//画背景
	void DrawBkGnd( IPainter* painter );

	UINT HotkeyTest( TCHAR cKey )
	{
		TCHAR szKey[2] = {0};
		szKey[0] = cKey;
		TCHAR szTmp[2] = {0};

		for ( int _i = 0; _i < m_nAccelCount; ++_i )
		{
			szTmp[0] = (TCHAR)m_pAccel[_i].key;
			if ( _tcsnicmp( szTmp, szKey, 2 ) == 0 )
			{
				return m_pAccel[_i].cmd;
			}
		}
		return 0;
	}

private:
	friend bool PtInMenu( POINT point );
	vector<RECT> m_itemsRect;
	UINT  m_nCmdRet;
	UINT  m_parentIndex;
	HWND  m_hWnd;
	CPTDC m_bufferDC;
	HBITMAP m_hBufferBmp;
	HBITMAP m_hOldBufferBmp;
	CPTDC   m_bkGndDC;
	HBITMAP m_hbkBitmap;
	HBITMAP m_hbkOldBitmap;
	BOOL	m_bPoping;
	MENUDATA*   m_pMenuData;
	int			m_nMenuCount;
	ACCEL*		m_pAccel;
	HACCEL      m_hAccel;
	int			m_nAccelCount;
	int			m_fuFlags;
	int         m_nMouseIn;
	int			m_orgX;
	int			m_orgY;
	BITMAPINFOHEADER bmih;
	CUIPopMenu* m_subMenu;
	CUIPopMenu* m_parentMenu;
	static	 map<HWND, CUIPopMenu*> m_mapWindows;
public:
	static	 IMG*    m_pBkGnd;
	static	 IMG*    m_pSubBkGnd;
	static   IMG*    m_pBkItem;  //菜单项鼠标经过时的背景
	static   IMG*	 m_pSeparator; //分隔栏图标
	static   Margin  m_margin;
	static   Margin  m_subMargin;
	static   IImage* m_pSubArrow;//子菜单标识图标
	static   UINT	 m_uItemHeight;//菜单高度
	static   UINT    m_uIconWidth; //菜单项左侧图标宽度
	static   UINT    m_uDrawMask;  //菜单项绘制掩码 见MENUDRAWMASK
	static   UINT    m_uAlpha;     //菜单透明度
	static   UINT    m_itemOffset;
	static   HFONT   m_font;
	static   ARGB    m_clrFont;
	static   ARGB    m_clrGray;
	static   bool	 m_bLoopRuning;
	static   bool    m_bLayered;
	static   MENUMSGFILTER m_msgFilter;
};
