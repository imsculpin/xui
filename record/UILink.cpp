#include "UILink.h"
#include "UILayPanel.h"
#include "UIListCtrl.h"
BOOL CUILink::CursorIn( POINT point  )
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

void CUILink::OnDraw(  IPainter* painter  )
{
	assert( XW_NULL != m_nClass );
	if ( !IsVisable() )
	{
		return;
	}

	if ( !m_strText.empty() )
	{
		RECT rcDraw;
		GetRect( rcDraw );
		if (  RECT_ISEMPTY(rcDraw)
			|| !painter->RectVisible( &rcDraw ))
		{
			return;
		}
		ARGB clrOld = painter->SetBrushColor( IsMouseIn()?GetHotTextColor():GetTextColor() );
		HFONT fontUnderline = NULL;
		HFONT pOldFont = NULL;
		HFONT hFont = GetFont();
		if ( hFont )
		{
			if( IsMouseIn() )
			{
				LOGFONT	lfFont = {0};
				GetObject( hFont, sizeof(LOGFONT), &lfFont );
				lfFont.lfUnderline = true;
				fontUnderline = CreateFontIndirect( &lfFont );
				hFont = fontUnderline;
			}
		}
		painter->DrawString( m_strText.c_str(), -1, &rcDraw, hFont, m_nStyle );
		painter->SetBrushColor( clrOld );
		if ( fontUnderline )
		{
			DeleteObject( fontUnderline );
		}
	}
}