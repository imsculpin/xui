#include "UIButton.h"
void CUIButton::OnDraw(  IPainter* painter  )
{
	assert( XW_NULL != m_nClass );
	if ( !IsVisable() )
	{
		return;
	}
	IMG* pImageShow = NULL;
	ARGB clrMask = -1;
	RECT rcBtn;
	GetRect( rcBtn );
	if ( RECT_ISEMPTY(rcBtn)
		|| !painter->RectVisible( &rcBtn )
		)
	{
		return;
	}
	if ( m_bEnable )
	{
		//鼠标不在单元上
		if ( !IsMouseIn() )
		{
			pImageShow = &m_imageNormal;
		}
		//鼠标点击单元
		else if( m_bLBDown && IsImageValid(&m_imagePressed) )
		{
			pImageShow = &m_imagePressed;
		}
		//鼠标在单元上
		else if( IsImageValid(&m_imageOver) )
		{
			pImageShow = &m_imageOver;
		}
		else
		{
			pImageShow = &m_imageNormal;
		}
	}
	else
	{
		pImageShow = IsImageValid(&m_imageDisable)?&m_imageDisable:&m_imageNormal;
	}
	
	if ( pImageShow )
	{ 
		painter->DrawImage( pImageShow, &rcBtn );
	}
	__super::OnDraw( painter );
}

BOOL CUIButton::CursorIn( POINT point  )
{
	if ( !IsVisable() )
	{
		return FALSE;
	}
	if ( m_hCursor && m_bEnable )
	{
		::SetCursor( m_hCursor );
		return TRUE;
	}
	return FALSE;
}

BOOL CUIButton::MouseLBUp( POINT pt )
{
	BOOL bRet = __super::MouseLBUp( pt );
	_Update();
	return bRet;
}

void CUIButton::MouseLeave()
{
	__super::MouseLeave();
	_Update();
}

BOOL CUIButton::MouseLBDown( POINT pt )
{
	BOOL bRet = __super::MouseLBDown( pt );
	_Update();
	return bRet;
}