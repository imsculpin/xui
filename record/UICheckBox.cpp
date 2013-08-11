#include "UICheckBox.h"
#include <WindowsX.h>
#include "UIView.h"

CUICheckBox::CUICheckBox( const CUICheckBox& uObj ) :CUIText( uObj )
,m_bChecked(uObj.m_bChecked)
,m_bEnable(uObj.m_bEnable)
,m_btnCheck(uObj.m_btnCheck)
,m_btnUnCheck(uObj.m_btnUnCheck)
,m_bDefaultSkin(uObj.m_bDefaultSkin)
{
	m_btnCheck.SetParent(this);
	m_btnUnCheck.SetParent(this);
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
}

CUICheckBox& CUICheckBox::operator=( const CUICheckBox& uObj )
{
	if ( this == &uObj )
	{
		return *this;
	}
	CUIText::operator =(uObj);
	m_bChecked=uObj.m_bChecked;
	m_bEnable =uObj.m_bEnable;
	m_btnCheck = uObj.m_btnCheck;
	m_btnUnCheck = uObj.m_btnUnCheck;
	m_bDefaultSkin = uObj.m_bDefaultSkin;
	m_btnCheck.SetParent(this);
	m_btnUnCheck.SetParent(this);
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
	return *this;
}

void CUICheckBox::OnDraw(  IPainter* painter  )
{
	if ( !IsVisable() )
	{
		return;
	}
	RECT rcThis;
	GetRect( rcThis );
	if ( !RECT_ISEMPTY(rcThis)
		&& painter->RectVisible( &rcThis ) )
	{
		if ( m_bChecked )
		{
			m_btnCheck.OnDraw( painter );
		}
		else
		{
			m_btnUnCheck.OnDraw( painter );
		}
		if ( !m_strText.empty() )
		{
			rcThis.left+=m_btnCheck.GetSize().cx + 2;
			ARGB clrOld = painter->SetBrushColor( GetTextColor() );
			painter->DrawString( m_strText.c_str(), -1, &rcThis, GetFont(), m_nStyle );
			painter->SetBrushColor( clrOld );
		}
	}
}

void CUICheckBox::SetCheck( bool bCheck, bool bNotifyDataChange /*= false */ )
{
	m_btnCheck.Show( bCheck );
	m_btnUnCheck.Show( !bCheck );
	m_bChecked = bCheck;
	if ( GetUIWindow() )
	{
		_Update();
	}
	if ( bNotifyDataChange
		&& m_parent )
	{
		NM_PARAM param = {0};
		RECT rect;
		GetRect( rect );
		param.point.x = rect.left;
		param.point.y = rect.top;
		param.nCtrlId = GetID();
		if ( m_parent )
		{
			param.nParentId = m_parent->GetID();
		}
		param.wParam = m_bChecked;
		m_parent->Notify( XWM_CHANGED, 0, (LPARAM)(LONG_PTR)&param );
	}
}

void CUICheckBox::Show( bool bShow /*= true */ )
{
	if (  m_bShow != bShow )
	{
		if ( bShow )
		{
			m_btnCheck.Show( m_bChecked );
			m_btnUnCheck.Show( !m_bChecked );
		}
		else
		{
			m_btnCheck.Show(bShow);
			m_btnUnCheck.Show(bShow);
		}
	}
	__super::Show( bShow );
}

void CUICheckBox::SetNormalImage( const IMG& imageCheck, const IMG& imageUncheck )
{
	m_btnCheck.SetNormalImage(imageCheck);
	m_btnUnCheck.SetNormalImage(imageUncheck);
	ImageInfo info = {0};
	if ( imageCheck.hImage )
	{
		imageCheck.hImage->GetImageInfo( &info );
	}
	else if ( imageUncheck.hImage )
	{
		imageUncheck.hImage->GetImageInfo( &info );
	}
	int _Y = (GetSize().cy-info.Height)/2;
	m_btnCheck.SetRect( RECT_C(0, _Y, info.Width, _Y+info.Height) );
	m_btnUnCheck.SetRect( RECT_C(0, _Y, info.Width, _Y+info.Height) );
}


INT32 CUICheckBox::Create( const tstring& strText, const RECT& rect, INT32 nCommandId /*= UNIT_TEXT_ID */ )
{
	__super::Create( strText, rect, nCommandId );
	m_nClass = XW_CHECKBOX;
	m_btnCheck.Create( _T(""), RECT_C( 0, 0, 0, 0 ) );
	m_btnUnCheck.Create( _T(""), RECT_C( 0, 0, 0, 0 ) );
	m_btnCheck.SetParent( this );
	m_btnUnCheck.SetParent( this );
	SetStyle( XWS_SINGLELINE|XWS_ALIGNCENTER );
	return RL_OK;
}

BOOL CUICheckBox::MouseMoving( POINT pt )
{
	if ( m_bEnable )
	{
		RECT rcCheck;
		m_btnCheck.GetRect( rcCheck );
		__super::MouseMoving( pt );
		if ( m_bChecked )
		{	
			return m_btnCheck.MouseMoving( RECT_TL(rcCheck) );
		}
		return m_btnUnCheck.MouseMoving( RECT_TL(rcCheck) );
	}
	return FALSE;
}

void CUICheckBox::MouseLeave()
{
	if ( m_bEnable )
	{
		__super::MouseLeave();
		if ( m_bChecked )
		{
			return m_btnCheck.MouseLeave();
		}
		return m_btnUnCheck.MouseLeave();
	}
}