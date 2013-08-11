#ifndef __IMAGING_PLUS_DEFINE__
#define __IMAGING_PLUS_DEFINE__
#include <GdiPlus.h>
#include "sgt.h"
#include "smart.h"
#include "UIdefine.h"
#define SURPORT_EXT
using namespace Gdiplus;
#include <string>
#ifndef LOG
#define LOG
#endif
struct ImageInfo
{
	UINT Width, Height;
};
struct Margin
{
	int left;
	int top;
	int right;
	int bottom;
};

class IImage
{
public:
	virtual BOOL GetImageInfo(
		OUT ImageInfo* imageInfo
		)const =0;
	virtual UINT GetWidth(  )const = 0;
	virtual UINT GetHeight()const = 0;
	virtual const Bitmap* GetBitmap(  )const = 0;
	virtual Bitmap* GetBitmap(  ) = 0;
	virtual Bitmap* SetBitmap( Bitmap* bitmap ) = 0;
	virtual HBITMAP GetHBITMAP( ARGB TransColor ) = 0;
	virtual HICON GetIcon()=0;
	virtual bool GetPixel( int x, int y, Color& clr )const = 0;
	virtual bool SetPixel( int x, int y, const Color& clr ) = 0;
};

__DEFINE_SMART_HANDLE_NEED_DELETER( IImage, HIMAGE, DestroyImageDeleter<IImage> );
enum DRAWMODE
{
	DM_NORMAL  = 0x00000000,
	DM_TILE   = 0x00000001,
	DM_STRETCH = 0x00000002,
};
struct IMG
{
	HIMAGE	 hImage;	 //底图
	DRAWMODE mode;		 //绘图模式
	ARGB	 imageAvgClr;//底图平均色（ 底图为空时，显示此颜色 ）
	Margin	 margin;  //边框
	RECT     rcValid; //图片的有效区
	BYTE     alpha;	  //透明度
	IMG()
		:mode( DM_STRETCH )
		,imageAvgClr(X_TRANSPARENT)
		,alpha(0xFF)
	{
		RtlZeroMemory( &rcValid, sizeof(RECT) );
		RtlZeroMemory( &margin, sizeof(Margin) );
	}
};

inline bool IsImageValid( const IMG* pImage )
{
	if ( pImage
		&& (pImage->hImage != NULL 
		|| pImage->imageAvgClr != X_TRANSPARENT) )
	{
		return true;
	}
	return false;
}


typedef struct _tagBrushHandle{DWORD unused;}*HBrush;
typedef struct _tagPenHandle{DWORD unused;}*HPen;
typedef struct _tagPathHandle{DWORD unused;}*HPath;
class IPainter
{
public:
	virtual HDC GetDC(  ) = 0;
	virtual void ReleaseDC( HDC hdc ) = 0;
	virtual HBrush SetBrush( HBrush brush )=0;
	virtual HPen SetPen( HPen pen )=0;
	virtual ARGB SetBrushColor( ARGB argb )=0;
	virtual ARGB GetBrushColor() = 0;
	virtual ARGB SetPenColor( ARGB argb )=0;
	virtual ARGB GetPenColor() = 0;
	virtual BOOL FillRegion( HRGN hRgn )=0;
	virtual BOOL FillRectangle( int x, int y, int width, int height )=0;
	virtual BOOL PtVisible( int x, int y )=0;
	virtual BOOL RectVisible( int x, int y, int x1, int y1 )=0;
	virtual BOOL RectVisible( LPCRECT lpRect )=0;
	virtual int  SetTabStop( int nTabStop )=0;
	virtual int  GetTabStop()=0;
	virtual BOOL SetClip( int x, int y, int width, int height ) = 0;
	virtual BOOL SetClip( HRGN hRgn ) = 0;
	virtual HRGN GetClip(  ) = 0;
	virtual BOOL ResetClip(  ) = 0;

	virtual BOOL DrawRectangle( int x, int y, int width, int height )=0;
	virtual BOOL DrawEllipse( int x, int y, int width, int height )=0;
	virtual BOOL DrawImage( IImage* pImage, LPCRECT lpcDest, LPRECT lpcSrc = NULL, UINT uDrawMode = DM_NORMAL, int rotate = 0 ) = 0;
	virtual BOOL DrawImage( IMG* pImage, LPCRECT lpcDest, int rotate = 0 ) = 0;
	virtual BOOL DrawLine( int x, int y, int x1, int y1 ) = 0;
	virtual BOOL DrawPath( HPath path ) = 0;
	virtual BOOL FillPath( HPath path ) = 0;
	virtual BOOL DrawString( LPCTSTR lpszText, int nLength, LPRECT lpcRect, HFONT hFont, UINT uFormat, int rotate = 0 ) = 0;
	virtual BOOL MeasureString( LPCTSTR lpszText, int nLength, /*[in/out]*/LPRECT lpcRect, HFONT hFont, UINT uFormat ) = 0;
	virtual BOOL MeasureCharacterRects( LPCTSTR lpszText, int nLength, HFONT hFont, UINT uFormat, LPRECT lpRect, int nRectCount ) = 0;
};

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef UIPLUS_EXPORTS
#	define IMGAPI(ret) _declspec(dllexport) ret _cdecl
#else
#	define IMGAPI(ret) _declspec(dllimport) ret _cdecl
#endif
	IMGAPI(IImage*) CreateImageA( LPCSTR pszImageFileName );
	IMGAPI(IImage*) CreateImageW( LPCWSTR pszImageFileName );
	IMGAPI(IImage*) CloneImage( Bitmap* pBitmap );
	IMGAPI(IImage*) CreateImage( LPCVOID lpBuffer, LONG lBufferSize );
	IMGAPI(HRGN)	CreateRgn( const IImage* pImage );
	IMGAPI(IImage*) CreateImageFromBitmap( HBITMAP hbm, HPALETTE hpal );
	IMGAPI(IImage*) CreateImageFromIcon( HICON hIcon );
	IMGAPI(ARGB)	GetAvgColor( const IImage* pImage );
	IMGAPI(void)	DestroyImage( IImage* pImage );
#ifdef SURPORT_EXT
	IMGAPI(HRGN)	CreatePartRgn( const IImage* pImage, LPCRECT lpRect );
	IMGAPI(IImage*) GetTileImage( HDC hdc, IImage* pImage, ARGB bkClr, int nBorder = -1 );
	IMGAPI(BOOL) DrawOuterGlow( HDC hDC, LPCRECT lpRect, LPCTSTR lpszString,\
		HFONT hFont, int align, int format );//Outer Glow
	IMGAPI(IImage*) GetGrayImage( const IImage* pImage );

	//Graphics
	IMGAPI(IPainter*) UICreateImagePainter( IImage* pImage, bool bClear=false );
	IMGAPI(IPainter*) UICreateDCPainter( HDC hDC, bool bClear=false );
	IMGAPI(IPainter*) UICreateWndPainter( HWND hWnd, bool bClear=false );
	IMGAPI(void) UIDestroyPainter( IPainter* painter );
	IMGAPI(HPen) UICreateSolidPen( ARGB argb, int width );
	IMGAPI(HPen) UICreatePen( HBrush hBrush );
	IMGAPI(void) UIDestroyPen( HPen hPen );
	IMGAPI(HBrush) UICreateSolidBrush( ARGB argb );
	IMGAPI(HBrush) UICreateLinearBrush( LPRECT lpRect, ARGB color1, ARGB color2, int mode = 0 );
	IMGAPI(HBrush) UICreateTextureBrush( IImage* pImage, LPRECT pRect = NULL );
	IMGAPI(void)  UIDestroyBrush( HBrush hBrush );
	IMGAPI(HPath) UICreatePolyPath( const Point* points, int nCount );
	IMGAPI(HPath) UICreatePath( const Rect* pRect );
	IMGAPI(void)  UIDestroyPath( HPath hPath );
#endif
#ifdef __cplusplus
}
#endif
#ifndef UIPLUS_EXPORTS
inline IImage* CreateImage( LPCWSTR pszImageFileName )
{
	return CreateImageW( pszImageFileName );
}
inline IImage* CreateImage( LPCSTR pszImageFileName )
{
	return CreateImageA( pszImageFileName );
}
inline IImage* CreateImage( const std::wstring& pszImageFileName )
{
	return CreateImageW( pszImageFileName.c_str() );
}
inline IImage* CreateImage( const std::string& pszImageFileName )
{
	return CreateImageA( pszImageFileName.c_str() );
}
#endif

#endif