#ifndef __X_CTRLCHECKBOX_H__
#define __X_CTRLCHECKBOX_H__
#include "UIButton.h"
class CUICheckBox
	:public CUIText
{	
public:
	CUICheckBox( bool bDefaultSkin = true )
		:m_bChecked( false )
		,m_bEnable(true)
		,m_bDefaultSkin( bDefaultSkin )
		,m_btnCheck(false)
		,m_btnUnCheck(false)
	{

	}
	CUICheckBox( const CUICheckBox& uObj );
	CUICheckBox& operator=( const CUICheckBox& uObj );
	virtual ~CUICheckBox()
	{
	}

	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID );
	void SetNormalImage( const IMG& imageCheck, const IMG& imageUncheck );
	void GetNormalImage( IMG& imageCheck, IMG& imageUncheck )
	{
		imageCheck = *m_btnCheck.GetNormalImage();
		imageUncheck = *m_btnUnCheck.GetNormalImage();
	}

	void SetMouseOverImage( const IMG& imageCheck, const IMG& imageUncheck )
	{
		m_btnCheck.SetMouseOverImage(imageCheck);
		m_btnUnCheck.SetMouseOverImage(imageUncheck);
	}

	void GetMouseOverImage( IMG& imageCheck, IMG& imageUncheck )
	{
		imageCheck = *m_btnCheck.GetMouseOverImage();
		imageUncheck = *m_btnUnCheck.GetMouseOverImage();
	}

	void SetPressedImage( const IMG& imageCheck, const IMG& imageUncheck )
	{
		m_btnCheck.SetPressedImage(imageCheck);
		m_btnUnCheck.SetPressedImage(imageUncheck);
	}

	void GetPressedImage( IMG& imageCheck, IMG& imageUncheck )
	{
		imageCheck = *m_btnCheck.GetPressedImage();
		imageUncheck = *m_btnUnCheck.GetPressedImage();
	}

	void SetDisableImage( const IMG& imageCheck, const IMG& imageUncheck )
	{
		m_btnCheck.SetDisableImage(imageCheck);
		m_btnUnCheck.SetDisableImage(imageUncheck);
	}

	void GetDisableImage( IMG& imageCheck, IMG& imageUncheck )
	{
		imageCheck = *m_btnCheck.GetDisableImage();
		imageUncheck = *m_btnUnCheck.GetDisableImage();
	}

	void Show( bool bShow = true );
	
	void Enable( bool bEnable )
	{
		m_btnCheck.Enable( bEnable );
		m_btnUnCheck.Enable( bEnable );
		__super::Enable( bEnable );
	}

	void SetCheck( bool bCheck, bool bNotifyDataChange = false );
	bool GetCheck()const
	{
		return m_bChecked;
	}
	void SetRect( const RECT& rcSet )
	{
		__super::SetRect( rcSet );
		SIZE size = m_btnCheck.GetSize();
		int _Y = (GetSize().cy-size.cy)>>1;
		m_btnCheck.SetRect( RECT_C(0, _Y, size.cx, _Y+size.cy) );
		m_btnUnCheck.SetRect( RECT_C(0, _Y, size.cx, _Y+size.cy) );
	}
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
public:
	void OnDraw(  IPainter* painter );
	virtual BOOL MouseMoving( POINT pt );
	virtual void MouseLeave(  );
	virtual BOOL MouseLClick( POINT pt )
	{
		SetCheck( !m_bChecked, true );
		return TRUE;
	}
	BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam )
	{
		return FALSE;
	}
private:
	CUIButton  m_btnCheck;
	CUIButton  m_btnUnCheck;
	bool	   m_bChecked;
	bool	   m_bEnable;
	bool	   m_bDefaultSkin;
};

#endif