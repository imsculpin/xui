#include "RL_define.h"
CPTDC dcSrc;
CPTDC maskDC;
CPTDC newImageDC;
CPTDC newMaskDC;
bool TransparentDrawBmp(
						HDC dcDest,              // handle to Dest DC
						CRect rcDraw,
						HBITMAP hBitmap,		 // handle to source DC
						COLORREF clrMask/* = -1*/    // color to make transparent
					 )
{
	if ( !maskDC )
	{
		maskDC.GetDC(NULL);
	}
	if ( !newImageDC )
	{
		newImageDC.GetDC(NULL);
	}
	if ( !newMaskDC )
	{
		newMaskDC.GetDC(NULL);
	}
	if ( !dcSrc )
	{
		dcSrc.GetDC( dcDest );
	}

	HBITMAP maskBitmap  = NULL;
	HBITMAP newImage    = NULL;
	HBITMAP newMask     = NULL;

	HBITMAP oldNewImage = NULL;
	HBITMAP oldBitmap   = NULL;
	HBITMAP oldMask     = NULL;
	HBITMAP oldNewMask  = NULL;

	if ( NULL == hBitmap 
		|| rcDraw.IsRectEmpty() 
		|| rcDraw.IsRectNull() )
	{
		return false;
	}

	BITMAP bm;
	GetObject( hBitmap, sizeof(BITMAP), &bm );
	oldBitmap = (HBITMAP)SelectObject( dcSrc, hBitmap );
	if ( -1 == clrMask )
	{
		StretchBlt( dcDest, rcDraw.left, rcDraw.top, rcDraw.Width(), \
			rcDraw.Height(), dcSrc, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY );
		SelectObject( dcSrc, oldBitmap );
		return true;
	}

	UINT nWidthSrc = bm.bmWidth;
	UINT nHeightSrc = bm.bmHeight;
	UINT nWidthDest = rcDraw.Width();
	UINT nHeightDest = rcDraw.Height();

	maskBitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, 1, NULL);
	if (maskBitmap == NULL)
	{   
		SelectObject( dcSrc, oldBitmap );
		return false;
	}

	oldMask =  (HBITMAP)SelectObject(maskDC, maskBitmap);

	SetBkColor(maskDC, RGB(0,0,0));
	SetTextColor(maskDC, RGB(255,255,255));
	if (!BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,NULL,0,0,BLACKNESS))
	{    
		SelectObject( dcSrc, oldBitmap );
		SelectObject( maskDC, oldMask );
		DeleteObject( maskBitmap );
		return false;
	}

	SetBkColor(dcSrc, clrMask);
	BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,dcSrc,0,0,SRCINVERT);

	SetBkColor(dcSrc, RGB(0,0,0));
	SetTextColor(dcSrc, RGB(255,255,255));
	BitBlt(dcSrc, 0,0,nWidthSrc,nHeightSrc,maskDC,0,0,SRCAND);

	newMask = CreateBitmap(nWidthDest, nHeightDest, 1,
		GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

	if (newMask == NULL)
	{
		SelectObject( dcSrc, oldBitmap );
		SelectObject( maskDC, oldMask );
		DeleteObject( maskBitmap );
		return false;
	}

	SetStretchBltMode(newMaskDC, COLORONCOLOR);
	oldNewMask = (HBITMAP) SelectObject(newMaskDC, newMask);
	StretchBlt(newMaskDC, 0, 0, nWidthDest, nHeightDest, maskDC, 0, 0,
		nWidthSrc, nHeightSrc, SRCCOPY);

	newImage = CreateBitmap(nWidthDest, nHeightDest, 1,
		GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

	if (newImage == NULL)
	{
		SelectObject( dcSrc, oldBitmap );
		SelectObject( maskDC, oldMask );
		SelectObject( newMaskDC, oldNewMask );
		DeleteObject( maskBitmap );
		DeleteObject( newMask );
		return false;
	}

	oldNewImage = (HBITMAP)SelectObject(newImageDC, newImage);
	StretchBlt(newImageDC, 0, 0, nWidthDest, nHeightDest, dcSrc, 0, 0, nWidthSrc,
		nHeightSrc, SRCCOPY);

	BitBlt( dcDest, rcDraw.left, rcDraw.top, nWidthDest, nHeightDest,
		newMaskDC, 0, 0, SRCAND);

	BitBlt( dcDest, rcDraw.left, rcDraw.top, nWidthDest, nHeightDest,
		newImageDC, 0, 0, SRCPAINT);

	SelectObject( dcSrc, oldBitmap );
	SelectObject( maskDC, oldMask );
	SelectObject( newMaskDC, oldNewMask );
	SelectObject( newImageDC, oldNewImage );
	DeleteObject( maskBitmap );
	DeleteObject( newMask );
	DeleteObject( newImage );
	return true;
}