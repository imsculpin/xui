#ifndef __UI_CTRLBASE_H__
#define __UI_CTRLBASE_H__
#include "UIType.h"

class IUIBase;
class IUIWindow
{
public:
	virtual HWND GetHandle()const = 0;
	virtual IMG* GetBkGnd()  = 0;
	virtual void GetRect( LPRECT lpRect )const=0;
	virtual void GetClient( LPRECT lpRect )const=0;
	virtual void SetRect( const RECT& rect, bool bErase = false )=0;
	virtual void SetArc( INT32 arcTop, INT32 arcbottom ) = 0;
	virtual void SetRgn( HRGN hRgn )=0;
	virtual HRGN GetRgn( )=0;
	virtual DWORD SetStyle( DWORD dwStyle ) = 0;
	virtual DWORD GetStyle() = 0;
	virtual BOOL SetRedraw( BOOL bRedraw = TRUE ) = 0;
	virtual bool InvalidRect( LPCRECT lpRect=NULL, BOOL bErase = FALSE )= 0;
	virtual void UpdateWindow(  ) = 0;
	virtual void Show( int nShowCmd )= 0;
	virtual IUIBase* GetChild( UINT nId )=0;
	virtual BOOL AddChild( IUIBase* pView, IUIBase* pInsertBefore = NULL ) = 0;
	virtual void RemoveChild( UINT nId ) = 0;
};

#define  VIEW_TOP		(IUIBase*)-1 
#define  VIEW_BOTTOM	(IUIBase*)-2 
class IUIBase
{
public:
	//滚动条
	virtual BOOL VertScroll( int nSBCode, int nPos, int iDrect ) = 0;
	virtual BOOL HorzScroll( int sCODE, int nPos  ) = 0;

	//other apis
	virtual void		SetParent( IUIBase* parent )	  = 0;
	virtual IUIBase*	GetParent(  )				const = 0; 
	virtual void		SetFont( HFONT pFont )			  = 0;
	virtual HFONT 		GetFont(  )					const = 0;
	virtual bool		IsHot()						const = 0;
	virtual void		GetClient( RECT& rect )		  = 0;
	virtual void        SetID( INT32 nCmd )				  = 0;
	virtual INT32		GetID( )					const = 0;
	virtual void	    SetRect( const RECT& rcSet, bool bErase = false )	  = 0;
	virtual void		GetRect( RECT& rect )      const = 0;
	virtual SIZE		GetSize(  )const				  = 0;
	virtual bool		IsVisable()					const = 0;
	virtual IMG*	    GetBkGnd(){return NULL;}
	virtual BOOL		CursorIn( POINT point  )					  = 0;
	virtual void		OnDraw(  IPainter* painter  )				  = 0;
	virtual void		DrawScroll( IPainter* painter )			  { return; }
	virtual INT32	    GetScrollPos( int nBar )	const { return 0; }
	virtual bool		InvalidRect( LPCRECT lpRect=NULL )= 0;
	virtual void		Update(  ){ if(GetUIWindow()){ GetUIWindow()->UpdateWindow(); } }
	virtual HWND		GetHandle( )const { if(GetUIWindow()){ return GetUIWindow()->GetHandle(); }return NULL; }

//protected:
	//Mouse
	virtual BOOL MouseMoving( POINT pt )    = 0;
	virtual void MouseLeave (  )			= 0;
	virtual void MouseIn    (  )			= 0;
	virtual BOOL MouseLBUp  ( POINT pt )    = 0;
	virtual BOOL MouseLClick( POINT pt )    = 0;
	virtual BOOL MouseLBDown( POINT pt )    = 0;
	virtual BOOL MouseRBUp  ( POINT pt )    = 0;
	virtual BOOL MouseRClick( POINT pt )    = 0;
	virtual BOOL MouseRBDown( POINT pt )    = 0;
	virtual BOOL MouseLDBClick( POINT pt )  = 0;
	virtual BOOL MouseRDBClick( POINT pt )  = 0;
	virtual BOOL MouseWheel   ( int zDelta )= 0;
	virtual BOOL FocusIn( POINT point )	    = 0;
	virtual BOOL FocusLeave()				= 0;
	virtual BOOL KeyDown( WPARAM wParam, LPARAM lParam )= 0;
	virtual BOOL KeyUp( WPARAM wParam, LPARAM lParam )  = 0;
	virtual BOOL OnChar( WPARAM wParam, LPARAM lParam ){return FALSE;};
	virtual IUIWindow* GetUIWindow()const= 0;
	virtual XW_CLS			GetClass()					const = 0;

	//others notify
	virtual BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam ) = 0;
};

class IUICtrl
	:public IUIBase
{
public:
	virtual INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )	                    = 0;
	virtual void SetTextColor( ARGB clr )					= 0;
	virtual void SetText( const tstring& strText )			    = 0;
	virtual void SetStyle( UINT32 nStyle )				    = 0;
	virtual void Enable( bool bEnable )							= 0;
	virtual void SetData( DWORD dwData )						= 0;
	virtual void SetShowCondition( ShowCondition condition )	= 0;
	virtual tstring			GetText(  )					const = 0;
	virtual ARGB			GetTextColor( )				const = 0;
	virtual UINT32			GetStyle(  )				const = 0;
	virtual void			Show( bool bShow = true )		  = 0;
	virtual DWORD			GetData( )					const = 0;
	virtual void			HasNotify( bool bNotify )		  = 0;
	virtual bool			HasNotify(  )				const = 0;
public:
	virtual ~IUICtrl()
	{

	}
public:
	//******************************************************************************
	// 方    法: TestTextSize
	// 访问权限: public  
	// 返 回 值: INT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 测试控件的文本空间大小
	//******************************************************************************
	static SIZE TestTextSize( IUICtrl& unit )
	{
		SIZE ret = { 0, 0 };
		HDC _hDC = ::GetDC(NULL);
		IPainter* painter = UICreateDCPainter( _hDC );
		if ( painter )
		{
			RECT rcDraw;
			unit.GetRect( rcDraw );
			painter->MeasureString( unit.GetText().c_str(), -1, &rcDraw, unit.GetFont(), unit.GetStyle() );
			::ReleaseDC( NULL, _hDC );
			UIDestroyPainter( painter );
			ret.cx = RECT_W( rcDraw );
			ret.cy = RECT_H( rcDraw );
			return ret;
		}
		return ret;
	}
};
#endif


