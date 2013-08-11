#ifndef __X_TAB_H__
#define __X_TAB_H__
#include "UIView.h"
class CUITab
	:public CUIView
{	
public:
	typedef HLAYPANEL	 HTABITEM;
	typedef CUILayPanel  TABITEM;
	typedef CUILayPanel* LPTABITEM;

public:
	CUITab()
		:m_tab( HORZ )
		,m_bAuto(false)
	{

	}

	~CUITab()
	{

	}

	BOOL Create( IN const RECT& rect, IUIWindow* pWindow, IN UINT32 nID, IUIBase* pInsertBefore  = NULL, bool bVert = false  );
	LPTABITEM InsertItem( const RECT& rcItem, UINT nId );
	BOOL	  RemoveItem( UINT nId );
	LPTABITEM GetItem( UINT nId );
	DWORD GetItemData( UINT nId );
	BOOL  ClickItem( UINT nId );
	BOOL SetItemData( UINT nId, DWORD dwData );
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
		m_imageHot =  image;
	}

	IMG* GetMouseOverImage( )
	{
		return &m_imageHot;
	}

	void SetPressedImage( const IMG& image )
	{
		m_imagePressed =  image;
	}

	IMG* GetPressedImage(  )
	{
		return &m_imagePressed;
	}

	void SetDisableImage( const IMG& image )
	{
		m_imageDisable =  image;
	}

	IMG* GetDisableImage( )
	{
		return &m_imageDisable;
	}

	void EnableAutoMode( bool bAuto = false )
	{
		if ( m_bAuto != bAuto)
		{
			m_bAuto = bAuto;
			if ( m_bAuto )
			{
				AutoPlace();
			}
		}
	}

	void SetRect( const RECT& rcSet, bool bErase = false )
	{
		__super::SetRect( rcSet );
		if ( m_bAuto )
		{
			AutoPlace();
		}
	}
	
	HTABITEM GetItemFromPoint( POINT point )
	{
		for ( vector<HTABITEM>::iterator _lab = m_vecItems.begin();\
			m_vecItems.end() != _lab; ++_lab )
		{
			if( ::PtInRect( &(*_lab)->GetRect(), point ) )
			{
				return *_lab;
			}
		}
		return NULL;
	}

protected:
	void OnDraw(  IPainter* painter );
	void EraseItemBkGnd( HTABITEM& hTabItem,  IPainter* painter );
	virtual BOOL MouseMoving( POINT point ) ;
	virtual BOOL MouseLBDown( POINT point );
	virtual BOOL MouseLBUp  ( POINT point );
	//virtual BOOL MouseLClick( POINT point );;
	virtual BOOL MouseRBDown( POINT point );
	virtual BOOL MouseRBUp  ( POINT point );
	//virtual BOOL MouseRClick( POINT point );;
	virtual BOOL MouseLDBClick( POINT point );
	virtual BOOL MouseRDBClick( POINT point );
	void MouseLeave( )
	{
		__super::MouseLeave();
		if ( m_hHot )
		{
			RECT rcInvalid= { 0,0,0,0 };
			m_hHot->GetRect( rcInvalid );
			m_hHot = NULL;
			InvalidRect( &rcInvalid );
		}
	}
	virtual bool IsEmpty()
	{
		return m_vecItems.empty();
	}
	void AutoPlace();
private:
	vector<HTABITEM>	m_vecItems;
	HTABITEM			m_hSelect;
	HTABITEM			m_hHot;
	HTABITEM			m_hFocus;
	IMG					m_imageNormal;
	IMG					m_imageHot;
	IMG					m_imagePressed;
	IMG					m_imageDisable;
	int					m_tab;
	bool				m_bAuto;
};

#endif