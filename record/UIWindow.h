#ifndef __UI_WINADAPTER_H__
#define __UI_WINADAPTER_H__
#include "Switch.h"
#include <assert.h>
#include "UICtrlBase.h"
#include "UIPlus.h"
#include <map>
using std::map;

#define  WS_DEFAULT WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_SYSMENU|WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX
#define  WS_EX_DEFAULT WS_EX_LEFT|WS_EX_LTRREADING

 #define  X_UI	_T("X_UI")
 class CUIWindow:public IUIWindow
 {
 public:
	CUIWindow()
		:m_arcTop(0)
		,m_arcBottom(0)
		,m_dwStyle(0)
		,m_hRgn(NULL)
		,m_nAlpha(0xFF)
		,m_bModal(false)
		,m_hWnd(NULL)
		,m_nResult( IDCANCEL )
		,m_nMaxX(0)
		,m_nMaxY(0)
		,m_nMinX(0)
		,m_nMinY(0)
	{
	}
	virtual ~CUIWindow()
	{
		Destroy();
	}

 	BOOL Create( const RECT& rect, unsigned long nStyle, unsigned long nExStyle, CUIWindow* parent )
	{
		HWND hParent = NULL;
		if ( parent )
		{
			hParent = parent->GetHandle();
		}
		return Create( rect, nStyle, nExStyle, hParent );
	}
	
	void Destroy()
	{
		if ( m_hWnd )
		{
			DestroyWindow(Detach());
			m_hWnd = NULL;
		}
	}
	BOOL Create( const RECT& rect, unsigned long nStyle = WS_DEFAULT, unsigned long nExStyle = WS_EX_DEFAULT, HWND hWnd = NULL );
	int DoModal( const RECT& rect, ULONG nStyle = WS_DEFAULT, ULONG uExStyle = WS_EX_DEFAULT );
	void EndModal( int nResult );
	bool IsModal()
	{
		return m_bModal;
	}
 	HWND GetHandle(  )const
 	{
 		if( this!=NULL )
		{
			return m_hWnd;
		}
		return NULL;
 	}

	BOOL SetRedraw( BOOL bRedraw = TRUE )
	{
		assert( m_hWnd );
		return 0 == SendMessage( WM_SETREDRAW, (WPARAM)bRedraw, 0 );
	}

	//******************************************************************************
	// 方    法: SetBkGnd
	// 访问权限: public  
	// 参    数: IN img 背景图
	// 返 回 值: ARGB
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	void SetBkGnd( const IMG& img, bool bUpdate = true );

	IMG* GetBkGnd( )
	{
		return &m_imageBkGnd;
	}
	
	BOOL Attach( HWND hWnd );
	HWND Detach();
	static CUIWindow* FromHandle( HWND hWnd );

	void GetRect( LPRECT lpRect )const
	{
		GetWindowRect( lpRect );
	}

	void GetClient( LPRECT lpRect )const
	{
		GetClientRect( lpRect );
	}

	void SetRect( const RECT& rect, bool bErase = false )
	{
		if( IsWindow(m_hWnd) )
		{
			SetWindowPos( m_hWnd, NULL, rect.left, rect.top, RECT_W(rect), RECT_H(rect), SWP_NOZORDER|SWP_NOACTIVATE );
		}
		if ( bErase )
		{
			UpdateBkGnd( m_hWnd );
		}
	}

	void SetArc( INT32 arcTop, INT32 arcbottom )
	{
		m_arcTop = arcTop;
		m_arcBottom = arcbottom;
	}

	virtual void SetRgn( HRGN hRgn );

	virtual HRGN GetRgn( )
	{
		return m_hRgn;
	}

	void SetMax( int nMaxX = -1, int nMaxY = -1 )
	{
		m_nMaxX = nMaxX;
		m_nMaxY = nMaxY;
	}

	void SetMin( int nMinX = -1, int nMinY = -1 )
	{
		m_nMinX = nMinX;
		m_nMinY = nMinY;
	}

	BOOL SetWindowText( const tstring& strText )
	{
		assert( m_hWnd );
		if ( m_hWnd )
		{
			return ::SetWindowText( m_hWnd, strText.c_str() );
		}
		return FALSE;
	}

	void GetWindowText( tstring& strText )
	{
		TCHAR szText[260] = {0};
		::GetWindowText( m_hWnd, szText, 260 );
		strText = szText;
	}

	BOOL EnableBlurWindow( bool bEnable );
	virtual bool InvalidRect( LPCRECT lpRect=NULL, BOOL bErase = FALSE )
	{
		if ( m_hWnd )
		{
			if ( bErase )
			{
				UpdateBkGnd( m_hWnd );
			}
			return TRUE == ::InvalidRect( m_hWnd, lpRect, 0 );
		}
		return false;
	}
	virtual void UpdateWindow(  )
	{
		if ( m_hWnd )
		{
			::UpdateWindow( m_hWnd );
		}
	}
	virtual void Show( int nShowCmd )
	{
		if ( m_hWnd )
		{
			::ShowWindow( m_hWnd, nShowCmd );
		}
	}
	virtual IUIBase* GetChild( UINT nId )
	{
		return NULL;
	}
	virtual BOOL AddChild( IUIBase* pView, IUIBase* pInsertBefore = NULL )
	{
		return FALSE;
	}
	virtual void RemoveChild( UINT nId )
	{
		return;
	}

 public:
	 inline operator HWND() const
	 { return this == NULL ? NULL : m_hWnd; }
	 inline BOOL operator==(const CUIWindow& wnd) const
	 { return ((HWND) wnd) == m_hWnd; }
	 inline BOOL operator!=(const CUIWindow& wnd) const
	 { return ((HWND) wnd) != m_hWnd; }
	 inline LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
	 { assert(::IsWindow(m_hWnd)); return ::SendMessage(m_hWnd, message, wParam, lParam); }
	 inline BOOL PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
	 { assert(::IsWindow(m_hWnd)); return ::PostMessage(m_hWnd, message, wParam, lParam); }
	 inline BOOL DragDetect(POINT pt) const
	 { assert(::IsWindow(m_hWnd)); return ::DragDetect(m_hWnd, pt); }
	 inline void SetFont(HFONT hFont, BOOL bRedraw)
	 { assert(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, bRedraw); }
	 inline HFONT GetFont() const
	 { assert(::IsWindow(m_hWnd)); return (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0); }
	 inline HMENU GetMenu() const
	 { assert(::IsWindow(m_hWnd)); return ::GetMenu(m_hWnd); }
	 inline BOOL SetMenu(HMENU hMenu)
	 { assert(::IsWindow(m_hWnd)); return ::SetMenu(m_hWnd, hMenu); }
	 inline void DragAcceptFiles(BOOL bAccept)
	 { assert(::IsWindow(m_hWnd)); ::DragAcceptFiles(m_hWnd, bAccept); }
	 inline HMENU GetSystemMenu(BOOL bRevert) const
	 { assert(::IsWindow(m_hWnd)); return ::GetSystemMenu(m_hWnd, bRevert); }
	 inline BOOL IsIconic() const
	 { assert(::IsWindow(m_hWnd)); return ::IsIconic(m_hWnd); }
	 inline BOOL IsZoomed() const
	 { assert(::IsWindow(m_hWnd)); return ::IsZoomed(m_hWnd); }
#ifdef _AFX_NO_OCC_SUPPORT
	 inline void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
	 { assert(::IsWindow(m_hWnd)); ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint); }
#endif //_AFX_NO_OCC_SUPPORT
#ifdef _AFX_NO_OCC_SUPPORT
	 inline BOOL SetWindowPos( int x, int y, int cx, int cy, UINT nFlags)
	 { assert(::IsWindow(m_hWnd)); return ::SetWindowPos(m_hWnd, NULL,
	 x, y, cx, cy, nFlags|SWP_NOZORDER); }
#endif //_AFX_NO_OCC_SUPPORT
	 inline int SetWindowRgn(HRGN hRgn, BOOL bRedraw)
	 { assert(::IsWindow(m_hWnd)); return ::SetWindowRgn(m_hWnd, hRgn, bRedraw); }
	 inline int GetWindowRgn(HRGN hRgn) const
	 { assert(::IsWindow(m_hWnd) && hRgn != NULL); return ::GetWindowRgn(m_hWnd, hRgn); }
	 inline void BringWindowToTop()
	 { assert(::IsWindow(m_hWnd)); ::BringWindowToTop(m_hWnd); }
	 inline void GetWindowRect(LPRECT lpRect) const
	 { assert(::IsWindow(m_hWnd)); ::GetWindowRect(m_hWnd, lpRect); }
	 inline void GetClientRect(LPRECT lpRect) const
	 { assert(::IsWindow(m_hWnd)); ::GetClientRect(m_hWnd, lpRect); }
	 inline void MapWindowPoints(HWND hWndto, LPPOINT lpPoint, UINT nCount) const
	 { assert(::IsWindow(m_hWnd)); ::MapWindowPoints(m_hWnd, hWndto, lpPoint, nCount); }
	 inline void MapWindowPoints(HWND hWndto, LPRECT lpRect) const
	 { assert(::IsWindow(m_hWnd)); ::MapWindowPoints(m_hWnd, hWndto, (LPPOINT)lpRect, 2); }
	 inline void ClientToScreen(LPPOINT lpPoint) const
	 { assert(::IsWindow(m_hWnd)); ::ClientToScreen(m_hWnd, lpPoint); }
	 inline void ClientToScreen(LPRECT lpRect ) const
	 { assert(::IsWindow(m_hWnd)); ::ClientToScreen(m_hWnd, (LPPOINT)lpRect);::ClientToScreen(m_hWnd, ((LPPOINT)lpRect)+1); }
	 inline void ScreenToClient(LPPOINT lpPoint) const
	 { assert(::IsWindow(m_hWnd)); ::ScreenToClient(m_hWnd, lpPoint); }
	 inline void ScreenToClient( LPRECT lpRect ) const
	 { assert(::IsWindow(m_hWnd)); ::ScreenToClient( m_hWnd, (LPPOINT)lpRect );::ScreenToClient( m_hWnd, ((LPPOINT)lpRect)+1 ); }
	 inline HDC BeginPaint(LPPAINTSTRUCT lpPaint)
	 { assert(::IsWindow(m_hWnd)); ::BeginPaint(m_hWnd, lpPaint); }
	 inline void EndPaint(LPPAINTSTRUCT lpPaint)
	 { assert(::IsWindow(m_hWnd)); ::EndPaint(m_hWnd, lpPaint); }
	 inline HDC GetDC()
	 { assert(::IsWindow(m_hWnd)); return ::GetDC(m_hWnd); }
	 inline HDC GetWindowDC()
	 { assert(::IsWindow(m_hWnd)); ::GetWindowDC(m_hWnd); }
	 inline int ReleaseDC(HDC hDC)
	 { assert(::IsWindow(m_hWnd)); return ::ReleaseDC(m_hWnd, hDC); }
	 inline void InvalidateRgn(HRGN hRgn, BOOL bErase)
	 { assert(::IsWindow(m_hWnd)); ::InvalidateRgn(m_hWnd, hRgn, bErase); }
	 inline void ValidateRect(LPCRECT lpRect)
	 { assert(::IsWindow(m_hWnd)); ::ValidateRect(m_hWnd, lpRect); }
	 inline void ValidateRgn(HRGN hRgn)
	 { assert(::IsWindow(m_hWnd)); ::ValidateRgn(m_hWnd, hRgn); }
	 inline HDC GetDCEx(HRGN hRgn, DWORD flags)
	 { assert(::IsWindow(m_hWnd)); return ::GetDCEx(m_hWnd, hRgn, flags); }
	 inline UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT nElapse,
		 void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD))
	 { assert(::IsWindow(m_hWnd)); return ::SetTimer(m_hWnd, nIDEvent, nElapse,
	 lpfnTimer); }
	 inline BOOL KillTimer(UINT_PTR nIDEvent)
	 { assert(::IsWindow(m_hWnd)); return ::KillTimer(m_hWnd, nIDEvent); }
	 // Win4
	 inline HICON SetIcon(HICON hIcon, BOOL bBigIcon)
	 { assert(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, WM_SETICON, bBigIcon, (LPARAM)hIcon); }
	 inline HICON GetIcon(BOOL bBigIcon) const
	 { assert(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, WM_GETICON, bBigIcon, 0); }
	 inline void Print(HDC hdc, DWORD dwFlags) const
	 { assert(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_PRINT, (WPARAM)hdc, dwFlags); }
	 inline void PrintClient(HDC hdc, DWORD dwFlags) const
	 { assert(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_PRINTCLIENT, (WPARAM)hdc, dwFlags); }
	 DWORD SetStyle( DWORD dwStyle )
	 {
		 assert( m_hWnd );
		 if ( m_hWnd )
		 {
			 if ( WS_CAPTION&dwStyle )
			 {
				 m_dwStyle |= XWS_CAPTION;
				 dwStyle &= (~WS_CAPTION);
			 }
			 return SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );
		 }
		 return 0;
	 }

	 DWORD GetStyle()
	 {
		 assert( m_hWnd );
		 if ( m_hWnd )
		 {
			 DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
			 if ( m_dwStyle & XWS_CAPTION )
			 {
				 dwStyle |= WS_CAPTION;
			 }
			 return dwStyle;
		 }
		 return 0;
	 }

	 DWORD SetExStyle( DWORD dwStyle )
	 {
		 assert( m_hWnd );
		 if ( m_hWnd )
		 {
			 return SetWindowLong( m_hWnd, GWL_EXSTYLE, dwStyle );
		 }
		 return 0;
	 }

	 DWORD GetExStyle()
	 {
		 assert( m_hWnd );
		 if ( m_hWnd )
		 {
			 return GetWindowLong(m_hWnd, GWL_EXSTYLE);
		 }
		 return 0;
	 }
	 
 protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitWnd(){ return TRUE; };
	virtual void OnSize( UINT nType, int cx, int cy )
	{
		return;
	}
	virtual LRESULT WinProc( UINT message, WPARAM wParam, LPARAM lParam );
 protected:
	static LRESULT WINAPI WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	HWND m_hWnd;
 private:
	DWORD    m_dwStyle;
 	INT32	 m_arcTop;
	INT32    m_arcBottom;
	int		 m_nAlpha;
	int		 m_nMaxX;
	int		 m_nMaxY;
	int		 m_nMinX;
	int      m_nMinY;
	IMG		 m_imageBkGnd;
	HRGN	 m_hRgn;
	bool	 m_bModal;
	int		 m_nResult;
	static	 map<HWND, CUIWindow*> m_mapWindows;
 };

#endif