#ifndef __UI_CTRLGROUP_H__
#define __UI_CTRLGROUP_H__
#include "UIText.h"
class CUIView;
class CUILayPanel;
__DEFINE_SMART_HANDLE_NEED_DELETER( CUILayPanel, HLAYPANEL, DeteleDeleter<CUILayPanel> );
class CUILayPanel
	:public IUIBase
{
public:
	CUILayPanel( INT32 nIndex = -1 )
		:m_nId(nIndex)
		,m_parent(NULL)
		,m_dwData( 0 )
		,m_dwSate( RRS_NORMAL )
		,m_nMargin( 0 )
		,m_pHot(NULL)
		,m_bVisable(true)
		,m_pBkGnd( NULL )
		,m_bLBDown(false)
		,m_bRBDown(false)
		,m_bMouseIn(false)
	{
		RECT_EMPTY( m_rect);
	}

	~CUILayPanel(void)
	{
		_Clear();
	}


	CUILayPanel( const CUILayPanel& unit )
	{
		_Copy(unit);
	}

	const CUILayPanel& operator = ( const CUILayPanel& unit )
	{
		_Copy( unit );
		return *this;
	}
public:
	IUICtrl* AddXCtrl( XW_CLS enStyle, const tstring& strText,\
		const RECT& rcXCtrl, INT32 nCommandId = UNIT_TEXT_ID );
	INT32 AddXCtrl( const IUICtrl* pXCtrl );
	INT32 RemoveXCtrl( INT32 nCtrlId );
	IUICtrl* GetXCtrl( UINT32 nCtrlId )const;
	void EnumXCtrls( std::vector<IUICtrl*>& vecXCtrls )const;
	void OnDraw(  IPainter* painter  );
	IUICtrl* GetXCtrlFromPoint( POINT pt );
public:
	void  SetSize ( const SIZE& size )  
	{
		SIZE _pre = {RECT_W(m_rect),RECT_H(m_rect)};
		m_rect.bottom = m_rect.top+size.cy;
		m_rect.right=m_rect.left+size.cx;
		if( _pre.cx != size.cx
			|| _pre.cy != size.cy )
		{
			Notify( XLAY_SIZECHANGED, 0, (LPARAM)(LONG_PTR)this );
		}
	}
	void  SetSize ( int cx, int cy )  
	{
		SIZE _pre = {RECT_W(m_rect),RECT_H(m_rect)};
		m_rect.bottom = m_rect.top+cy;
		m_rect.right=m_rect.left+cx;
		if( m_nId >= 0
			&& (_pre.cx != cx
			|| _pre.cy != cy) )
		{
			Notify( XLAY_SIZECHANGED, 0, (LPARAM)(LONG_PTR)this );
		}
	}
	void  SetData ( DWORD dwData )		 { m_dwData = dwData; }
	void  Show( bool bShow = false ){ m_bVisable = bShow; }
	void  SetOrg	 ( int x, int y )
	{
		m_rect.bottom = m_rect.bottom-m_rect.top+y;
		m_rect.top = y;
		m_rect.right = m_rect.right-m_rect.left+x;
		m_rect.left = x;
	}
	void  SetParent	 ( IUIBase* pOwner )  { m_parent = pOwner; }
	void  SetState	 ( DWORD dwState ) { m_dwSate = dwState; }
	void  SetMargin	 ( INT32 nMargin ){ m_nMargin = nMargin; }
	void   GetRect	 ( RECT& rect )const;
	RECT  GetRect	( )const
	{
		RECT rect = m_rect;
		if ( m_parent )
		{
			RECT rcParent;
			m_parent->GetRect( rcParent );
			::OffsetRect(&rect, rcParent.left, rcParent.top);
		}
		return rect;
	}
	void   GetClient( RECT& rect );
	POINT& GetOrg    (  ){return *((LPPOINT)&m_rect);};
	SIZE  GetSize	 (  )const;
	IUIBase* GetParent  (  )const { return m_parent; }
	DWORD   GetData   (  )const { return m_dwData; }
	INT32   GetMargin(  )const{ return m_nMargin;}
	DWORD	GetState(  )const{ return m_dwSate; }
	bool  IsHot       (  )const;
	bool  IsFocused	  (  )const;
	bool  IsValid	  (  )const;
	bool  IsVisable	  (  )const
	{
// 		if ( m_parent )
// 		{
// 			return m_parent->IsVisable()&&m_bVisable;
// 		}
		return m_bVisable;
	}
	bool  IsEmpty     (  )const{return m_XCtrls.empty();};
	void SetFont( HFONT pFont )
	{
		return;
	}
	HFONT  GetFont( )const
	{
		if ( m_parent )
		{
			return m_parent->GetFont(  );
		}
		return false;
	}
	bool  InvalidRect( LPCRECT lpRect = NULL )
	{
		if ( m_parent )
		{
			return m_parent->InvalidRect( lpRect );
		}
		return false;
	}

	virtual void SetID( INT32 nCmd )
	{
		m_nId = nCmd;
	}

	virtual INT32 GetID( )const
	{
		return m_nId;
	}
	virtual void SetRect( const RECT& rcSet, bool bErase = false )
	{
		SIZE _pre = {RECT_W(m_rect),RECT_H(m_rect)};
		m_rect = rcSet;		
		if( ( _pre.cx != RECT_W(m_rect)
			|| _pre.cy != RECT_H(m_rect) ) )
		{
			Notify( XLAY_SIZECHANGED, 0, (LPARAM)(LONG_PTR)this );
		}
	}

	void CalcSize();

	IMG* SetBkGnd( IMG* pBkGnd )
	{
		IMG* pPre = m_pBkGnd;
		m_pBkGnd = pBkGnd;
		return pPre;
	}

	IMG* GetBkGnd()
	{
		return m_pBkGnd;
	}

	XW_CLS GetClass()const
	{
		return XW_NULL;
	}

public:
	BOOL MouseMoving ( POINT pt );
	BOOL MouseLBUp( POINT pt );
	BOOL MouseLBDown( POINT pt );
	BOOL MouseLDBClick( POINT pt );
	BOOL MouseRBUp( POINT pt );
	BOOL MouseRDBClick( POINT pt );
	BOOL CursorIn( POINT pt );
	void MouseLeave(  );
	void MouseIn(  );
	BOOL MouseLClick	( POINT pt );
	BOOL MouseRBDown ( POINT pt );
	BOOL MouseRClick	( POINT pt );
	BOOL MouseWheel   ( int zDelta )
	{
		return FALSE;
	}
	virtual BOOL FocusIn( POINT point )
	{
		return FALSE;
	}

	virtual BOOL FocusLeave(  )
	{
		return FALSE;
	}
	virtual BOOL KeyDown( WPARAM wParam, LPARAM lParam )
	{
		NM_PARAM param = {0};
		param.lParam = lParam;
		param.nCtrlId = m_nId;
		param.wVirtualKey = wParam;
		if ( m_parent )
		{
			param.nParentId = m_parent->GetID();
		}
		Notify( XLAY_KEYDOWN, 0, (LPARAM)(LONG_PTR)&param );
		return TRUE;
	}
	virtual BOOL KeyUp( WPARAM wParam, LPARAM lParam )
	{
		NM_PARAM param = {0};
		param.lParam = lParam;
		param.nCtrlId = m_nId;
		param.wVirtualKey = wParam;
		if ( m_parent )
		{
			param.nParentId = m_parent->GetID();
		}
		Notify( XLAY_KEYUP, 0, (LPARAM)(LONG_PTR)&param );
		return TRUE;
	}
	BOOL VertScroll( int nSBCode, int nPos, int iDrect )
	{
		return FALSE;
	}
	BOOL HorzScroll( int sCODE, int nPos  )
	{
		return FALSE;
	}
	BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if ( m_parent )
		{
			return m_parent->Notify( message, wParam, lParam );
		}
		return FALSE;
	}
	virtual IUIWindow* GetUIWindow()const
	{
		if ( m_parent )
		{
			return m_parent->GetUIWindow();
		}
		return NULL;
	}
	INT32 GetScrollPos(int nBar)const
	{
		if ( m_parent )
		{
			return m_parent->GetScrollPos( nBar );
		}
		return 0;
	}
protected:

	void _Clear();
	void _Copy( const CUILayPanel& row );
	inline void _OnNotify( XLAY_MESSAGE message, POINT point )
	{
		NM_PARAM param = {0};
		param.point = point;
		param.nCtrlId = m_nId;
		param.nIndex  = m_nId;
		if ( m_parent )
		{
			param.nParentId = m_parent->GetID();
		}
		Notify( message, 0, (LPARAM)(LONG_PTR)&param );
	}

 	friend class CUIView;
public:
	INT32  m_nId;

private:
	INT32	  m_nMargin;
	DWORD	  m_dwData;
	DWORD	  m_dwSate;
	RECT      m_rect;
	IUIBase*  m_parent;
	IUICtrl*  m_pHot;
	IMG*	  m_pBkGnd;
	typedef std::vector<IUICtrl*> XCTRLARRAY;
	XCTRLARRAY m_XCtrls;
	bool	   m_bMouseIn;
	bool	   m_bLBDown;
	bool	   m_bRBDown;
	bool	   m_bVisable;
};
#endif