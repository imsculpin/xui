#pragma once
#include <map>
#include "UILayPanel.h"
#include "UIScroll.h"
#include "UIPlus.h"
typedef BOOL (*FORMAT)( void* paramFormat, CUILayPanel& row );
class CUIView : public IUIBase
{
public:
	CUIView();
	virtual ~CUIView()
	{
		if(m_hCursorNormal)
		{
			::DeleteObject( m_hCursorNormal );
			m_hCursorNormal = NULL;
		} 
		if( UIGetWndMouseIn() == this )
		{
			UISetMouseIn( NULL );	
		}
		Detach();
	}
	
	/*!< 创建窗口 */
	BOOL Create( IN const RECT& rect, IUIWindow* pWindow, IN UINT32 nID, IUIBase* pInsertBefore = NULL );
	void Show( bool bShow );

	BOOL Attach( IUIWindow* window, IUIBase* pInsertBefore = NULL );
	BOOL Detach(  );
	void HookNotify( UICALLBACK revNotify, DWORD dwData = 0 )
	{
		vector<RECIEVERDATA>::iterator _lab = m_vecNotifyRecievers.begin();
		for ( ;_lab != m_vecNotifyRecievers.end(); ++_lab )
		{
			if ( _lab->reciever == revNotify )
			{
				return;
			}
		}
		RECIEVERDATA data;
		data.dwData = dwData;
		data.reciever = revNotify;
		m_vecNotifyRecievers.push_back( data );
	}

	void UnhookNotify( UICALLBACK revNotify )
	{
		for ( vector<RECIEVERDATA>::iterator _lab = m_vecNotifyRecievers.begin(); _lab != m_vecNotifyRecievers.end(); ++_lab )
		{
			if ( _lab->reciever == revNotify )
			{
				m_vecNotifyRecievers.erase( _lab );
				break;
			}
		}
	}

	//******************************************************************************
	// 方    法: AddXCtrl
	// 访问权限: public  
	// 参    数: IUICtrl * pUint
	// 返 回 值: void
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 当列表为空时,将被显示
	//******************************************************************************
	enum AREA{ CLIENT = 0x80000000, TOP, LEFT, RIGHT, BOTTOM, };
	virtual BOOL AddXCtrl( IUICtrl* pUnit, AREA = CLIENT );
	virtual IUICtrl* GetXCtrl( INT32 nCtrlId );

	DWORD GetStyle()const
	{
		return m_dwStyle;
	}

	void SetStyle( DWORD dwSytle )
	{
		m_dwStyle = dwSytle;
	}

	void SetCursor( HCURSOR hCursor )
	{
		m_hCursorNormal = hCursor;
		if ( NULL == m_hCursorNormal )
		{
			m_hCursorNormal = LoadCursor( NULL, IDC_ARROW );
		}
	}

	HCURSOR GetCursor(  )const
	{
		return m_hCursorNormal;
	}
	virtual void SetScrollPos( int nBar, int nPos, BOOL bRedraw  = TRUE  );
	virtual INT32 GetScrollPos(int nBar)const;
	CUIScroll* GetScrollBar( int nBar )
	{
		if ( nBar == VERT )
		{
			return &m_VScroll;
		}
		return &m_HScroll;
	}

	//******************************************************************************
	// 方    法: SetBkGnd
	// 访问权限: public  
	// 参    数: IN HBITMAP hBkEraseBitmap
	// 返 回 值: HBITMAP
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetBkGnd( IN const IMG& image );

	IMG* GetBkGnd( )
	{
		return &m_imageBkGnd;
	}
	virtual INT32 GetState( INT32 nIndex )
	{
		return RRS_NORMAL;
	}

	LONG GetVirturlHeight()
	{
		return m_VScroll.Max();
	}

	//******************************************************************************
	// 方    法: SetMargin
	// 访问权限: public  
	// 参    数: IN INT32 nMargin
	// 返 回 值: void
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	void SetMargin( IN Margin margin );

	//******************************************************************************
	// 方    法: GetMargin
	// 访问权限: public  
	// 返 回 值: UINT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline const Margin& GetMargin(  )const { return m_margin; }
	void EnableScroll( int nBar, bool bEnable );
	virtual void DBChanged(){};
public:

	virtual bool InvalidRect( LPCRECT lpRect = NULL )
	{
		if(!IsVisable() )
		{
			return false;
		}
		RECT rcInvalid;
		if ( lpRect )
		{
			rcInvalid = *lpRect;
			::OffsetRect( &rcInvalid, -m_HScroll.GetPos(), -m_VScroll.GetPos());
			if ( GetUIWindow() )
			{
				::InvalidRect( GetUIWindow()->GetHandle(), &rcInvalid );
			}
		}
		else
		{
			if ( GetUIWindow() )
			{
				::InvalidRect( GetUIWindow()->GetHandle(), &m_rect );
			}
		}
		return true;
	}
	//other apis
	virtual void SetParent( IUIWindow* parent )
	{
		Detach();
		Attach( parent );
		assert( FALSE );
	}

// 	virtual IUIWindow*		GetParent(  ) const
// 	{
// 		return m_parent;
// 	}

	virtual void SetFont( HFONT pFont )
	{ 
		m_hFont = pFont;
	};
	virtual HFONT GetFont( )const
	{ 
		return m_hFont; 
	}
	virtual bool		IsHot()const{ return false; }
	virtual void		GetClient( RECT& rect )
	{
		rect = m_rect;
		if ( !m_bScrollBarInClient )
		{
			if ( m_VScroll.IsVisable() )
			{
				rect.right -= m_VScroll.GetSize().cx;
			}
			if ( m_HScroll.IsVisable() )
			{
				rect.bottom -= m_HScroll.GetSize().cy;
			}
		}
		rect.left += m_margin.left;
		rect.right -= m_margin.right;
		::OffsetRect( &rect, 0, m_VScroll.GetPos() );
	}
	SIZE		GetSize(  )const
	{
		SIZE size = { RECT_W(m_rect), RECT_H(m_rect) };
		return size;
	}
	virtual void SetID( INT32 nCmd )
	{
		m_nCommandId = nCmd;
	}
	virtual INT32 GetID( )const 
	{
		return m_nCommandId;
	}
	virtual void SetRect( const RECT& rcSet, bool bErase = false );
	virtual void GetRect( RECT& rect )const
	{
		rect = m_rect;
	}

	bool IsVisable()const
	{
		return m_bVisable;
	}

	virtual IUIWindow* GetUIWindow()const
	{
		return m_pUIWindow;
	}

	BOOL ScreenToClient( LPPOINT lpPoint )
	{
		if ( m_pUIWindow && lpPoint )
		{
			if( ::ScreenToClient( m_pUIWindow->GetHandle(), lpPoint ) )
			{
				lpPoint->y += m_VScroll.GetPos();
				return TRUE;
			}
		}
		return FALSE;
	}
	
	BOOL ScreenToClient( LPRECT lpRect )
	{
		if( lpRect )
		{
			ScreenToClient((LPPOINT)lpRect);
			return ScreenToClient(((LPPOINT)lpRect)+1);
		}
		return FALSE;
	}

	BOOL ClientToScreen( LPPOINT lpPoint )
	{
		if ( m_pUIWindow && lpPoint )
		{
			lpPoint->y -= m_VScroll.GetPos();
			if( ::ClientToScreen( m_pUIWindow->GetHandle(), lpPoint ) )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL ClientToScreen( LPRECT lpRect )
	{
		if( lpRect )
		{
			ClientToScreen((LPPOINT)lpRect);
			return ClientToScreen(((LPPOINT)lpRect)+1);
		}
		return FALSE;
	}

	void ScrollBarInClient( bool bIn = false )
	{
		m_bScrollBarInClient = bIn;
	}
	virtual bool IsEmpty()
	{
		return true;
	}
	void SetParent( IUIBase* parent ) 
	{ 
		m_parent = parent;
		return;
	}
	IUIBase* GetParent()const
	{
		return m_parent;
	}
protected:

	virtual void OnDraw(  IPainter* painter );
	void DrawScroll( IPainter* painter );
	void _UpdateScroll( int nMaxVScroll = 1, int nMaxHScroll = 1 );
	

public:
	//Mouse
	virtual BOOL MouseMoving( POINT point ) ;
	virtual void MouseLeave (  );
	virtual void MouseIn    (  )
	{

	}
	BOOL CursorIn( POINT point );
	virtual BOOL MouseLBUp  ( POINT point );
	virtual BOOL MouseLClick( POINT point ){ return FALSE;};
	virtual BOOL MouseLBDown( POINT point );
	virtual BOOL MouseRBUp  ( POINT point );
	virtual BOOL MouseRClick( POINT point ){ return FALSE;};
	virtual BOOL MouseRBDown( POINT point );
	virtual BOOL MouseLDBClick( POINT point );
	virtual BOOL MouseRDBClick( POINT point );
	virtual BOOL MouseWheel   ( int zDelta );
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
		param.nCtrlId = m_nCommandId;
		param.wVirtualKey = wParam;
		param.nParentId = 0;
		return Notify( XLAY_KEYDOWN, m_nCommandId, (LPARAM)(LONG_PTR)&param );
	}
	virtual BOOL KeyUp( WPARAM wParam, LPARAM lParam )
	{
		NM_PARAM param = {0};
		param.lParam = lParam;
		param.nCtrlId = m_nCommandId;
		param.wVirtualKey = wParam;
		param.nParentId = 0;
		return Notify( XLAY_KEYUP, m_nCommandId, (LPARAM)(LONG_PTR)&param );
	}

	//others notify
	virtual BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam )
	{  
		BOOL bRet = FALSE;
		for ( vector<RECIEVERDATA>::iterator _lab = m_vecNotifyRecievers.begin();
			_lab != m_vecNotifyRecievers.end(); ++_lab )
		{
			bRet = (*(_lab->reciever))( message, m_nCommandId, lParam, _lab->dwData );
		}
		return FALSE;
	};

	//滚动条
	virtual BOOL VertScroll( int nSBCode, int nPos, int iDrect );
	virtual BOOL HorzScroll( int nSBCode, int nPos  );

	XW_CLS GetClass()const
	{
		return XW_NULL;
	}
protected:
	HLAYPANEL* IsPtInMargin( POINT point )
	{
		if ( NULL != m_panelLeft 
			&& ::PtInRect( &m_panelLeft->GetRect( ), point ) )
		{
			return &m_panelLeft;
		}
		if ( NULL != m_panelRight 
			&& ::PtInRect( &m_panelRight->GetRect( ), point ) )
		{
			return &m_panelRight;
		}
		if ( NULL != m_panelTop
			&& ::PtInRect( &m_panelTop->GetRect( ), point )) 
		{
			return &m_panelTop;
		}
		if ( NULL != m_panelBottom 
			&& ::PtInRect( &m_panelBottom->GetRect( ), point ) )
		{
			return &m_panelBottom;
		}
		return NULL;
	}
	void Clear();
protected:
	CUIScroll		m_VScroll;
	CUIScroll	    m_HScroll;
	Margin			m_margin;
	IUIWindow*		m_pUIWindow;
	IUIBase*        m_parent;

protected:
	RECT			m_rect;
private:
	HLAYPANEL			m_panelClient;//面板
	HLAYPANEL			m_panelLeft;//面板
	HLAYPANEL			m_panelRight;//面板
	HLAYPANEL			m_panelTop;//面板
	HLAYPANEL			m_panelBottom;//面板
	IMG				m_imageBkGnd;
	HCURSOR			m_hCursorNormal;
	DWORD			m_dwStyle;
	HFONT			m_hFont;
	UINT32			m_nCommandId;
	bool			m_bVisable;
	bool			m_bScrollBarInClient;
	struct RECIEVERDATA
	{
		UICALLBACK reciever;
		DWORD	 dwData;
	};
	vector<RECIEVERDATA> m_vecNotifyRecievers;
};

inline void CUIView::SetBkGnd( const IMG& image )
{
	m_imageBkGnd = image;
	if ( m_imageBkGnd.hImage )
	{
		m_imageBkGnd.imageAvgClr = ::GetAvgColor( m_imageBkGnd.hImage );
	}
	if( GetUIWindow() && GetUIWindow()->GetHandle() )
	{
#ifdef SURPORT_EXT
		UpdateBkGnd( GetUIWindow()->GetHandle(), this );
#else
		UpdateBkGnd( GetUIWindow()->GetHandle() );
#endif
	}
}
