#ifndef __DRAW_HELP_H__
#define __DRAW_HELP_H__
bool TransparentDrawBmp(
						HDC dcDest,              // handle to Dest DC
						CRect rcDraw,
						HBITMAP hBitmap,		 // handle to source DC
						COLORREF clrMask = -1    // color to make transparent
					 );

#endif