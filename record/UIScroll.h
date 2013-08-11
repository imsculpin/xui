#ifndef __X_CTRLSCROLL_H__
#define __X_CTRLSCROLL_H__
#include "UIButton.h"
#include "UIImage.h"
class CUIScroll
	:public IUIBase
{	
public:
	CUIScroll( IUIBase* parent = NULL, bool bDefaultSkin = true )
		:m_parent(parent)
		,m_pHot(NULL)
		,m_nbar(VERT)
		,m_nClass(XW_SCROLL)
		,m_nPos(0)
		,m_bVisable(false)
		,m_bMovingThumb(false)
		,m_miniThumb(20)
		,m_scrollStep( 50 )
		,m_bDefaultSkin(bDefaultSkin)
		,m_thumb(false)
		,m_arrow1(false)
		,m_arrow2(false)
	{
		m_pointLbDown.x = -1;
		m_pointLbDown.y = -1;
	}
	BOOL Create( int bar , IUIBase* parent, UINT nCmdId );
	void Show( bool bShow = true )
	{
		m_bar.Show(bShow);
		m_arrow2.Show(bShow);
		m_arrow1.Show(bShow);
		m_thumb.Show(bShow);
		m_bVisable = bShow;
	}
	void SetPos( long nPos );
	const long  GetPos(  )const
	{
		return m_nPos;
	}
	long& Min(){ return m_scrollRange.m_min; };
	long& Max(){ return m_scrollRange.m_max; };
	void SetRange( long min, long max );
	void SetScroll( const CUIButton& btnArrow1, const CUIButton& btnArrow2,
		const CUIImage& imgBar, const CUIButton& btnThumb );
	
	//other apis
	virtual void SetParent( IUIBase* parent )	
	{
		m_parent = parent;
		if ( m_bDefaultSkin 
			&& UIGetDefaultSkinFn())
		{
			(*UIGetDefaultSkinFn( ))(this);
		}
	}
	virtual IUIBase* GetParent(  )const 
	{
		return m_parent;
	}
	virtual void SetFont( HFONT pFont )  
	{
		return;
	}
	virtual HFONT GetFont(  )const
	{
		if ( m_parent )
		{
			return m_parent->GetFont( );
		}
		return NULL;
	}
	virtual bool InvalidRect( LPCRECT lpRect = NULL )   
	{
 		if ( m_parent
			&&m_parent->GetUIWindow( )
			&&m_parent->GetUIWindow( )->GetHandle() )
 		{
 			::InvalidRect( m_parent->GetUIWindow( )->GetHandle(), lpRect );
 		}
		return true;
	}
	virtual bool IsHot()const							  
	{
		return false;
	}
	bool IsVisable()const
	{
		if ( m_parent
			&& !m_parent->IsVisable() )
		{
			return false;
		}
		return m_bVisable;
	}
	virtual void GetClient( RECT& rect )		  
	{
		GetRect( rect );
	}
	virtual void SetID( INT32 nCmd )				 
	{
		m_nCommandId = nCmd;
	}
	virtual INT32 GetID( )const 
	{
		return m_nCommandId;
	}
	virtual void SetRect( const RECT& rcSet, bool bErase = false )	  
	{
		assert( FALSE );
		return;
	}

	virtual void GetRect( RECT& rect )const					      
	{
		rect = m_rect;
	}

	SIZE GetSize(  )const
	{
		SIZE size = { RECT_W(m_rect), RECT_H(m_rect) };
		return size;
	}
	void OnSize(  );
	BOOL MouseWheel( int zDelta );
	void SetMiniThumb( int nMin )
	{
		m_miniThumb = nMin;
		OnSize();
	}
	void SetScrollStep( int nStep )
	{
		if ( nStep > 0 )
		{
			m_scrollStep = nStep;
		}
	}
	virtual XW_CLS GetClass()const 
	{
		return m_nClass;
	}
public:
	void OnDraw(  IPainter* painter );
	IUICtrl*  GetXCtrlFromPoint( POINT pt );
	BOOL MouseMoving ( POINT pt );
	BOOL MouseLBUp( POINT pt );
	BOOL MouseLBDown( POINT pt );
	BOOL MouseLDBClick( POINT pt )
	{
		IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
		if ( pXCtrl )
		{
			if( pXCtrl->MouseLDBClick( pt ) )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL MouseRBUp( POINT pt )
	{
		IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
		if ( pXCtrl )
		{
			if( pXCtrl->MouseRBUp( pt ) )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL MouseRDBClick( POINT pt )
	{
		IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
		if ( pXCtrl )
		{
			return pXCtrl->MouseRDBClick( pt );
		}
		return FALSE;
	}

	BOOL CursorIn( POINT pt )
	{
		IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
		if ( pXCtrl )
		{
			return pXCtrl->CursorIn( pt );
		}
		return FALSE;
	}

	void MouseLeave(  )
	{
		try
		{
			if ( m_pHot )
			{
				m_pHot->MouseLeave();
				m_pHot = NULL;
			}
		}
		catch( ... )
		{

		}
	}

	virtual void MouseIn    (  )
	{

	}

	virtual BOOL MouseLClick( POINT pt )
	{
		return FALSE;
	}

	virtual BOOL MouseRClick( POINT pt )
	{
		return FALSE;
	}

	BOOL MouseRBDown( POINT pt )
	{
		IUICtrl* pXCtrl = GetXCtrlFromPoint( pt );
		if ( pXCtrl )
		{
			if( pXCtrl->MouseRBDown( pt ) )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	virtual BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam );
	//¹ö¶¯Ìõ
	virtual BOOL VertScroll( int nSBCode, int nPos, int iDrect )
	{
		if ( m_parent )
		{
			return m_parent->VertScroll( nSBCode, nPos, iDrect );
		}
		return FALSE;
	}
	virtual BOOL HorzScroll( int sCODE, int nPos  )
	{
		if ( m_parent )
		{
			return m_parent->HorzScroll( sCODE, nPos );
		}
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
		return TRUE;
	}
	virtual BOOL KeyUp( WPARAM wParam, LPARAM lParam )
	{
		return TRUE;
	}

	virtual IUIWindow* GetUIWindow()const
	{
		if ( m_parent )
		{
			return m_parent->GetUIWindow();
		}
		return NULL;
	}

private:
	CUIScroll( const CUIScroll& );
	CUIScroll& operator = ( const CUIScroll& );
	enum{ ID_BAR, ID_ARROW1, ID_ARROW2, ID_THUMB, };
	CUIButton m_arrow1;
	CUIButton m_arrow2;
	CUIImage  m_bar;
	CUIButton m_thumb;
	RANGE<>   m_scrollRange;
	IUIBase*  m_parent;
	IUIBase*  m_pHot;
	UINT	  m_nCommandId;
	int		  m_nbar;
	int		  m_nPos;
	int       m_miniThumb;
	int		  m_scrollStep;
	XW_CLS    m_nClass;
	POINT	  m_pointLbDown;
	RECT      m_rect;
	bool      m_bVisable;
	bool      m_bMovingThumb;
	bool	  m_bDefaultSkin;
};

#endif