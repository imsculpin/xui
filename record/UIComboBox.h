#pragma once
#include <map>
#include "UIScroll.h"
#include "UIButton.h"
class CUIWindow;
class CUIView;
class CUIComboBox : public CUIText
{
public:
	CUIComboBox( bool bDefaultSkin = true );
	CUIComboBox( const CUIComboBox& uObj );
	CUIComboBox& operator =( const CUIComboBox& uObj );
	virtual ~CUIComboBox();

	INT32 Create( tstring strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID );

public:
	int GetLBCount() const;
	int GetLBCurSel() const;
	BOOL SetLBCurSel(int nSelect);
	void SetLBHeight( int nHeight );
	void SetLBSpace( int nSpace );
	void SetLBHotBkGnd( const IMG& img );
	void SetLBTextColor( ARGB argb ){ m_clrLBText = argb;}
	DWORD GetLBItemData(int nIndex) const;
	void SetLBItemData(int nIndex, DWORD dwItemData);
	int GetLBText(int nIndex, tstring& strText ) const;
	int AddString(const tstring& strText);
	BOOL DeleteString(int nIndex);
	void DeleteAll();
	int InsertString(int nIndex, LPCTSTR lpszString);
	void SetDropView( CUIView* pView );
	void SetDropViewMaxHeight( int nHeight ){m_nDropViewMaxHeight = nHeight;};
	void SetDropViewWidth( int nWidth ){m_nDropViewMaxWidth=nWidth;};
	void SetDropViewOffset( int x, int y ){m_nOffsetX=x;m_nOffsetY=y;};
	void SetDropViewMargin( const Margin& margin );
	void SetDropViewBkGnd( const IMG& img );
	void SetDropMargin( const Margin& margin );
	void SetDropBkGnd(  const IMG& img  );
	BOOL DropView(  bool bDrop = true  );
	void SetText( const tstring& strText )
	{
		__super::SetText( strText );
		if ( m_hEdit
			&& IsWindowVisible(m_hEdit) )
		{
			SetWindowText( m_hEdit, strText.c_str() );
		}
	}
public:
	virtual void SetFont( HFONT pFont );;
	virtual void SetRect( const RECT& rcSet );
	void SetParent( IUIBase* parent );
	void SetNormalImage( const IMG& image )
	{
		m_imageNormal =  image;
	}
	IMG* GetNormalImage(  )
	{
		return &m_imageNormal;
	}
	void SetMouseOverImage( const IMG& image )
	{
		m_imageOver =  image;
	}
	IMG* GetMouseOverImage( )
	{
		return &m_imageOver;
	}
	void SetFocusImage( const IMG& image )
	{
		m_imageFocus =  image;
	}
	IMG* GetFocusImage( )
	{
		return &m_imageFocus;
	}
	void SetDisableImage( const IMG& image )
	{
		m_imageDisable =  image;
	}
	IMG* GetDisableImage( )
	{
		return &m_imageDisable;
	}

	void SetArrowNormalImage( const IMG& image )
	{
		m_imageArrowNormal =  image;
	}

	void SetArrowMouseOverImage( const IMG& image )
	{
		m_imageArrowOver =  image;
	}

	void SetArrowFocusImage( const IMG& image )
	{
		m_imageArrowFocus =  image;
	}
	void SetArrowDisableImage( const IMG& image )
	{
		m_imageArrowDisable =  image;
	}
	void SetMargin( const Margin& margin ){ m_margin = margin; }
	Margin& GetMargin( ){ return m_margin; }
	const Margin& GetMargin()const { return m_margin; }
	void SetArrowSize( const SIZE& sizeArrow );
	void SetArrowSize( int cx, int cy );
	virtual void OnDraw(  IPainter* painter  );
	virtual BOOL CursorIn( POINT point );
	virtual BOOL MouseLBUp( POINT pt );
	virtual BOOL MouseLBDown( POINT pt );
	virtual BOOL MouseLDBClick( POINT pt );
	virtual BOOL MouseRBDown( POINT pt );
	virtual void MouseLeave( );
	virtual BOOL MouseWheel( int zDelta );
	virtual BOOL KeyDown( WPARAM wParam, LPARAM lParam );
	virtual BOOL FocusIn( POINT point );
	virtual BOOL FocusLeave( );
	virtual void Enable( bool bEnable )
	{
		if ( m_bEnable != bEnable )
		{
			m_bEnable = bEnable;
			_Update();
		}
	}
	void SetReadOnly( bool bReadOnly = true ){m_bReadOnly = bReadOnly;}
private:/*WM_GETTEXT*/
	static LRESULT WINAPI WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	static LRESULT WINAPI DropViewWindowProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	static BOOL DropViewMessage( UINT message, WPARAM wParam, LPARAM lParam, DWORD dwParam );
	void CheckAndGetView();
	struct EDITHELP
	{
		CUIComboBox* pEdit;
		WNDPROC  orgProc;
	};
	CUIWindow* m_pListWindow;
	CUIView*   m_pListView;
	CUIView*   m_pListViewDefault;
	HWND m_hEdit;
	IMG  m_imageNormal;
	IMG  m_imageOver;
	IMG  m_imageFocus;
	IMG  m_imageDisable;
	IMG  m_imageArrowNormal;
	IMG  m_imageArrowOver;
	IMG  m_imageArrowFocus;
	IMG  m_imageArrowDisable;
	IMG  m_imageDropBkGnd;
	Margin m_marginDrop;
	SIZE m_sizeArrow;
	Margin  m_margin;
	HCURSOR m_hCursor;
	tstring m_strbkup;
	ARGB    m_clrLBText;
	int     m_nDropViewMaxHeight;
	int     m_nDropViewMaxWidth;
	int		m_nOffsetX;
	int		m_nOffsetY;
	bool	m_bEnable;
	bool    m_bReadOnly;
	bool	m_bDefaultSkin;
	static  map<HWND, EDITHELP> m_mapUIEdits;
};
