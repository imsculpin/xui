#ifndef __UI_CTRLBUTTON_H__
#define __UI_CTRLBUTTON_H__
#include "UIText.h"
class CUIButton:public CUIText
{	
public:
	CUIButton( bool bDefaultSkin = true )
		:m_hCursor(NULL)
		,m_crHotText(CLR_DEFAULT_TEXT)
		,m_crDisable( CLR_DEFAULT_GRAY )
		,m_bEnable(true)
		,m_bDefaultSkin( bDefaultSkin )
	{

	}

	CUIButton( const CUIButton& uObj )
		:CUIText( uObj )
		,m_hCursor( uObj.m_hCursor )
		,m_crHotText ( uObj.m_crHotText )
		,m_bEnable(uObj.m_bEnable)
		,m_imageNormal(uObj.m_imageNormal)
		,m_imageOver(uObj.m_imageOver)
		,m_imagePressed(uObj.m_imagePressed)
		,m_bDefaultSkin(uObj.m_bDefaultSkin)
	{
		if ( m_bDefaultSkin 
			&& m_parent
			&& UIGetDefaultSkinFn())
		{
			(*UIGetDefaultSkinFn( ))(this);
		}
	}
	
	CUIButton& operator =( const CUIButton& uObj )
	{
		if ( this == &uObj )
		{
			return *this;
		}
		CUIText::operator =(uObj);
		m_hCursor = uObj.m_hCursor;
		m_crHotText  = uObj.m_crHotText;
		m_bEnable = uObj.m_bEnable;
		m_imageNormal = uObj.m_imageNormal;
		m_imageOver = uObj.m_imageOver;
		m_imagePressed = uObj.m_imagePressed;
		m_bDefaultSkin = uObj.m_bDefaultSkin;
		if ( m_bDefaultSkin 
			&& m_parent
			&& UIGetDefaultSkinFn())
		{
			(*UIGetDefaultSkinFn( ))(this);
		}
		return *this;
	}

	virtual ~CUIButton()
	{
	}

	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )
	{
		assert( XW_NULL == m_nClass );
		__super::Create( strText, rect, nCommandId );
		m_nClass	 = XW_BUTTON;
		return RL_OK;
	}
public:
	void SetParent( IUIBase* parent )
	{
		__super::SetParent( parent );
		if ( m_bDefaultSkin 
			&& m_parent
			&& UIGetDefaultSkinFn())
		{
			(*UIGetDefaultSkinFn( ))(this);
		}
	}

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
	virtual void Enable( bool bEnable )
	{
		if ( m_bEnable != bEnable )
		{
			m_bEnable = bEnable;
			_Update();
		}
	}

public:
	virtual void SetCursor( HCURSOR hCursor )
	{
		assert( XW_NULL != m_nClass );
		m_hCursor = hCursor;
	}

	virtual void SetHotTextColor( ARGB clrHotText )
	{
		assert( XW_NULL != m_nClass );
		m_crHotText = clrHotText;
		_Update();
	}

	virtual HCURSOR GetCursor()
	{
		assert( XW_NULL != m_nClass );
		return m_hCursor;
	}

	virtual ARGB GetHotTextColor()
	{
		assert( XW_NULL != m_nClass );
		return m_crHotText;
	}

public:
	virtual void OnDraw(  IPainter* painter  );
	virtual BOOL CursorIn( POINT point );
	virtual BOOL MouseLBUp( POINT pt );
	virtual BOOL MouseLBDown( POINT pt );
	virtual void MouseLeave( );
	virtual BOOL KeyDown( WPARAM wParam, LPARAM lParam )
	{
		return TRUE;
	}
	virtual BOOL KeyUp( WPARAM wParam, LPARAM lParam )
	{
		return TRUE;
	}

private:
	bool	 m_bEnable;
	bool	 m_bDefaultSkin;
	ARGB m_crHotText;
	ARGB m_crDisable;
	HCURSOR  m_hCursor;
	IMG  m_imageNormal;
	IMG  m_imageOver;
	IMG  m_imagePressed;
	IMG  m_imageDisable;
};

#endif