#pragma once
#include <map>
#include "UIScroll.h"
#include <list>
using std::list;
class CUIRichEdit : public CUIText
{
	//�нṹ 
	class Line
	{
	protected:
		Line()
		{

		}

	public:
		virtual ~Line()
		{

		}
		enum LINETYPE
		{
			_NULL,
			STRING,
			OBJECT,
		};
		LINETYPE type;
		RECT rcLine;
		int index;
	};

	class NullLine:public Line
	{
	public:

	};

	class StringLine:public Line
	{
	public:
		tstring strLineText;
		vector<RECT> veclineUnitRect;
		vector<int>  vecCharPos;
	};

	class ObjectLine:public Line
	{
	public:
		DWORD dwObject;
		RECT  rcUnit;
	};
public:
	CUIRichEdit()
		:m_bEnable( true )
		,m_bReadOnly(false)
		,m_nTabStop(30)
		,m_pCaretLine(NULL)
		,m_nWhoHitCaret(0)
		,m_nLineCount(0)
		,m_painter(NULL)
		,m_bScrollBarInClient(true)
	{
		SetStyle( GetStyle()| XWS_SINGLELINE );
	}

	CUIRichEdit( const CUIRichEdit& uObj );
	CUIRichEdit& operator =( const CUIRichEdit& uObj );
	virtual ~CUIRichEdit();

	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )
	{
		assert( XW_NULL == m_nClass );
		__super::Create( "", rect, nCommandId );
		m_nClass	 = XW_RICHEDIT;
		for ( int _i = 0; _i < (int)strText.length(); ++_i )
		{
			OnChar( strText[_i], 0 );
		}
		return RL_OK;
	}
public:
	void SetText( const tstring& strText )
	{
		assert( GetHandle() );
		for ( list<Line*>::iterator _lab = m_listContent.begin();
			_lab != m_listContent.end(); ++_lab )
		{
			delete (*_lab);
		}
		m_listContent.clear();
		for ( int _i = 0; _i < (int)strText.length(); ++_i )
		{
			OnChar( strText[_i], 0 );
		}
	}

	void SetRect( const RECT& rcSet, bool bErase /* = false */ )
	{
		__super::SetRect( rcSet, bErase );
		for ( list<Line*>::iterator _lab = m_listContent.begin();
			_lab != m_listContent.end(); ++_lab )
		{
			(*_lab)->rcLine.right = RECT_W(rcSet) - m_margin.right;
		}
	}

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
	virtual BOOL OnChar( WPARAM wParam, LPARAM lParam );
	virtual BOOL KeyDown( WPARAM wParam, LPARAM lParam )
	{
		return __super::KeyDown( wParam, lParam );
	}
	virtual BOOL MouseWheel( int zDelta )
	{
		return FALSE;
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
	void SetTabWidth( int nTabWidth );
	void SetReadOnly( bool bReadOnly = true ){m_bReadOnly = bReadOnly;}
	void  GetClient( RECT& rect )
	{
		__super::GetRect( rect );
		if ( !m_bScrollBarInClient )
		{
			if ( m_VScroll.IsVisable() )
			{
				rect.right -= m_VScroll.GetSize().cx;
			}
		}
		rect.left += m_margin.left;
		rect.right -= m_margin.right;
		::OffsetRect( &rect, 0, m_VScroll.GetPos() );
		RECT_INFLATE( rect, -m_margin.left, -m_margin.top, -m_margin.right, -m_margin.bottom );
	}

	void ScrollBarInClient( bool bIn = false )
	{
		m_bScrollBarInClient = bIn;
	}
protected:
	void _UpdateScroll( int nMaxVScroll = 1, int nMaxHScroll = 1 );
	void CopyContent( list<Line*>& contentDest, const list<Line*>& contentSrc );
	Line* GetBackLine( Line::LINETYPE lineType, bool bnew = false );
	Line* GetCaretLine( POINT point );
	void InputChar( const tstring& strChar );

private:
	list<Line*> m_listContent;
	Line*		m_pCaretLine;
	CUIScroll   m_VScroll;
	IPainter*   m_painter;
	int			m_nWhoHitCaret;
	int			m_nLineCount;
	IMG		m_imageNormal;
	IMG		m_imageOver;
	IMG		m_imageFocus;
	IMG		m_imageDisable;
	Margin  m_margin;
	HCURSOR m_hCursor;
	int		m_nTabStop;
	bool	m_bEnable;
	bool    m_bReadOnly;
	bool    m_bScrollBarInClient;
};
