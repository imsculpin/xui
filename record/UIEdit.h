#pragma once
#include <map>
#include "UIScroll.h"
#include "UIButton.h"
class CUIEdit : public CUIText
{
public:
	CUIEdit( bool bDefaultSkin = true )
		:m_hEdit( NULL )
		,m_bEnable( true )
		,m_chPassword(_T('*'))
		,m_bPassword(false)
		,m_bGetText(false)
		,m_bReadOnly(false)
		,m_nTabStop(30)
		,m_nMax(-1)
		,m_bNumberOnly(false)
		,m_bDefaultSkin( bDefaultSkin )
	{
		SetStyle( GetStyle()| XWS_SINGLELINE );
	}

	CUIEdit( const CUIEdit& uObj );
	CUIEdit& operator =( const CUIEdit& uObj );
	virtual ~CUIEdit()
	{
		if( m_hEdit )
		{
			if ( m_mapUIEdits.size() )
			{
				m_mapUIEdits.erase( m_hEdit );
			}
			DestroyWindow( m_hEdit );
			m_hEdit = NULL;
		}
	}

	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )
	{
		assert( XW_NULL == m_nClass );
		__super::Create( strText, rect, nCommandId );
		m_nClass	 = XW_EDIT;
		return RL_OK;
	}

	tstring GetText()const;
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
	void SetMargin( const Margin& margin ){ m_margin = margin; }
	Margin& GetMargin( ){ return m_margin; }
	const Margin& GetMargin()const { return m_margin; }
	virtual void OnDraw(  IPainter* painter  );
	virtual BOOL CursorIn( POINT point );
	virtual BOOL MouseLBUp( POINT pt );
	virtual BOOL MouseLBDown( POINT pt );
	virtual BOOL MouseRBDown( POINT pt );
	virtual void MouseLeave( );
	virtual BOOL MouseWheel( int zDelta )
	{
		return TRUE;
	}
	virtual BOOL FocusIn( POINT point );
	virtual BOOL FocusLeave( );
	void EnableMultiline( bool bEnable = false );
	virtual void Enable( bool bEnable )
	{
		if ( m_bEnable != bEnable )
		{
			m_bEnable = bEnable;
			_Update();
		}
	}
	void SetTabStop( int nTab );
	void SetPasswordMode( bool bPassword, TCHAR ch = 0 );
	void SetReadOnly( bool bReadOnly = true ){m_bReadOnly = bReadOnly;}
	void SetLimitText( int nMax ){ if( m_hEdit ){SendMessage(m_hEdit, EM_SETLIMITTEXT, nMax, 0);}m_nMax = nMax; }
	void SetNumberOnly( bool bNumOnly = false ){m_bNumberOnly=bNumOnly;}
	bool IsPasswordMode(){return m_bPassword;}
	void  GetClient( RECT& rect )
	{
		__super::GetRect( rect );
		RECT_INFLATE( rect, -m_margin.left, -m_margin.top, -m_margin.right, -m_margin.bottom );
	}

protected:
	void Copy( );

	void SelectAll(  )
	{
		if ( m_hEdit )
		{
			SendMessage( m_hEdit, EM_SETSEL, 0, -1 );
		}
	}

	void Cut(  )
	{
		if ( m_hEdit )
		{
			SendMessage(m_hEdit, WM_CUT, 0, 0);
		}
	}

	void Paste(  )
	{
		if ( m_hEdit )
		{
			SendMessage(m_hEdit, WM_PASTE, 0, 0);
		}
	}

private:
	static LRESULT WINAPI WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	struct EDITHELP
	{
		CUIEdit* pEdit;
		WNDPROC  orgProc;
	};
	tstring m_strPassword;
	HWND	m_hEdit;
	IMG		m_imageNormal;
	IMG		m_imageOver;
	IMG		m_imageFocus;
	IMG		m_imageDisable;
	Margin  m_margin;
	HCURSOR m_hCursor;
	TCHAR	m_chPassword;
	int     m_nMax;
	int		m_nTabStop;
	bool	m_bEnable;
	bool    m_bPassword;
	bool    m_bReadOnly;
	bool    m_bNumberOnly;
	bool    m_bDefaultSkin;
	mutable bool    m_bGetText;
	static  map<HWND, EDITHELP> m_mapUIEdits;
};
