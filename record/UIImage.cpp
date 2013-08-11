#include "UIImage.h"
#include "UILayPanel.h"
#include "UIView.h"
void CUIImage::OnDraw(  IPainter* painter )
{
	if ( !IsVisable() )
	{
		return;
	}

	RECT rcDraw;
	GetRect( rcDraw );
	if ( RECT_ISEMPTY(rcDraw)
		|| !painter->RectVisible( &rcDraw ))
	{
		return;
	}
	if ( X_TRANSPARENT != m_clrBack )
	{
		ARGB clrOld = painter->SetBrushColor( m_clrBack );
		painter->FillRectangle( rcDraw.left, rcDraw.top, RECT_W(rcDraw), RECT_H(rcDraw) );
		painter->SetBrushColor( clrOld );
	}
	painter->DrawImage( &m_image, &rcDraw );
	return;
}
