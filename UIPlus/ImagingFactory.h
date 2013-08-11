#ifndef __IMAGING_FACTORY_DEFINE__
#define __IMAGING_FACTORY_DEFINE__
#include "ImagingPlus.h"
#ifndef WINCE
#pragma comment( lib, "gdiplus.lib" )
#ifndef LOG
#define LOG
#endif
class ImageImpl:public IImage
{
public:
	ImageImpl(  )
		:m_bitmap(NULL)
	{

	}

	~ImageImpl(  )
	{
		if ( m_bitmap )
		{
			delete m_bitmap;
			m_bitmap = NULL;
		}
	}

	BOOL GetImageInfo(
		OUT ImageInfo* imageInfo
		)const
	{
		if ( m_bitmap && imageInfo )
		{
			imageInfo->Width = m_bitmap->GetWidth();
			imageInfo->Height = m_bitmap->GetHeight();
			return TRUE;
		}
		return FALSE;
	}
	
	UINT GetWidth(  )const
	{
		if ( m_bitmap )
		{
			return m_bitmap->GetWidth();
		}
		return 0;
	}

	UINT GetHeight()const
	{
		if ( m_bitmap )
		{
			return m_bitmap->GetHeight();
		}
		return 0;
	}

	BOOL FromFile( const std::wstring& pszImageFile, BOOL useEmbeddedColorManagement = FALSE )
	{
		try
		{
			m_bitmap = Bitmap::FromFile( pszImageFile.c_str(), useEmbeddedColorManagement );
		}
		catch( ... )
		{
			
		}
		return NULL != m_bitmap;
	}

	BOOL FromStream( IStream* stream, BOOL useEmbeddedColorManagement = FALSE )
	{
		try
		{
			m_bitmap = Bitmap::FromStream( stream, useEmbeddedColorManagement );
			if ( m_bitmap 
				&& m_bitmap->GetLastStatus() == Ok )
			{
				return TRUE;
			}
			delete m_bitmap;
			m_bitmap = NULL;
		}
		catch( ... )
		{

		}
		return FALSE;
	}

	BOOL FromIcon( HICON hIcon )
	{
		try
		{
			m_bitmap = Bitmap::FromHICON(hIcon);
			if ( m_bitmap 
				&& m_bitmap->GetLastStatus() == Ok )
			{
				return TRUE;
			}
			delete m_bitmap;
			m_bitmap = NULL;
		}
		catch( ... )
		{

		}
		return FALSE;
	}

	BOOL FromHBITMAP(IN HBITMAP hbm, IN HPALETTE hpal)
	{
		try
		{
			m_bitmap = Bitmap::FromHBITMAP( hbm, hpal );
			if ( m_bitmap 
				&& m_bitmap->GetLastStatus() == Ok )
			{
				return TRUE;
			}
			delete m_bitmap;
			m_bitmap = NULL;
		}
		catch( ... )
		{

		}
		return FALSE;
	}

	BOOL Clone( Bitmap* pBitMap, int nPixelFormat = PixelFormatDontCare )
	{
		if ( pBitMap )
		{
			try
			{
				m_bitmap = pBitMap->Clone( 0, 0, pBitMap->GetWidth(), pBitMap->GetHeight(), nPixelFormat );
			}
			catch( ... )
			{
			}
		}
		return NULL != m_bitmap;
	}

	HBITMAP GetHBITMAP( ARGB TransColor );

	Bitmap* Attach( Bitmap* pBitmap )
	{
		Bitmap* pre = m_bitmap;
		m_bitmap = pBitmap;
		return pre;
	}

	Bitmap* Detach(  )
	{
		Bitmap* pre = m_bitmap;
		m_bitmap = NULL;
		return pre;
	}
	HICON GetIcon()
	{
		HICON hIcon = NULL;
		if ( m_bitmap )
		{
			if( Ok == m_bitmap->GetHICON( &hIcon ) )
			{
				return hIcon;
			}
		}
		return NULL;
	}

	bool GetPixel( int x, int y, Color& clr )const
	{
		if ( m_bitmap )
		{
			if( Ok == m_bitmap->GetPixel( x, y, &clr ) )
			{
				return true;
			}
		}
		return false;
	}

	bool SetPixel( int x, int y, const Color& clr )
	{
		if ( m_bitmap )
		{
			if( Ok == m_bitmap->SetPixel( x, y, clr ) )
			{
				return true;
			}
		}
		return false;
	}

	const Bitmap* GetBitmap(  )const
	{
		return m_bitmap;
	}

	Bitmap* GetBitmap(  )
	{
		return m_bitmap;
	}

	Bitmap* SetBitmap( Bitmap* bitmap )
	{
		Bitmap* pPre = m_bitmap;
		m_bitmap = bitmap;
		return pPre;
	}
	
private:
	Bitmap*	m_bitmap;
};
#endif

class CPainter:public IPainter
{
public:
	CPainter( HWND hWnd, bool bClear = false )try
		:m_pGraphics( new Graphics(hWnd) )
		,m_pBrush(new SolidBrush( Color::White ) )
		,m_pPen( new Pen( Color::Black ) )
		,m_nTabStop(0)
	{
		if ( NULL == hWnd )
		{
			delete m_pGraphics;
			m_pGraphics = NULL;
			delete m_pBrush;
			m_pBrush = NULL;
			delete m_pPen;
			m_pPen = NULL;
			return;
		}
		m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		if ( bClear )
		{
			m_pGraphics->Clear( 0 );
		}
	}
	catch( ... )
	{

	}

	CPainter( HDC hdc, bool bClear = false )try
		:m_pGraphics(new Graphics(hdc))
		,m_pBrush(new SolidBrush( Color::White ) )
		,m_pPen( new Pen( Color::Black ) )
		,m_nTabStop(0)
	{
		if ( NULL == hdc )
		{
			delete m_pGraphics;
			m_pGraphics = NULL;
			delete m_pBrush;
			m_pBrush = NULL;
			delete m_pPen;
			m_pPen = NULL;
			return;
		}
		m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		if ( bClear )
		{
			m_pGraphics->Clear( 0 );
		}
	}
	catch( ... )
	{

	}
	
	CPainter( IImage* pImage, bool bClear = false )try
		:m_pGraphics(new Graphics(pImage->GetBitmap()))
		,m_pBrush(new SolidBrush( Color::White ) )
		,m_pPen( new Pen( Color::Black ) )
		,m_nTabStop(0)
	{
		if ( NULL == pImage->GetBitmap() )
		{
			delete m_pGraphics;
			m_pGraphics = NULL;
			delete m_pBrush;
			m_pBrush = NULL;
			delete m_pPen;
			m_pPen = NULL;
			return;
		}
		m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		if ( bClear )
		{
			m_pGraphics->Clear( 0 );
		}
	}
	catch( ... )
	{

	}

	~CPainter( )
	{
		if ( m_pGraphics )
		{
			delete m_pGraphics;
			m_pGraphics = NULL;
		}
		if ( m_pBrush )
		{
			delete m_pBrush;
			m_pBrush = NULL;
		}
		if ( m_pPen )
		{
			delete m_pPen;
			m_pPen = NULL;
		}
	}

	bool IsValid()
	{
		return m_pGraphics != NULL;
	}

	HDC GetDC(  )
	{
		if ( m_pGraphics )
		{
			return m_pGraphics->GetHDC();
		}
		return NULL;
	}
	void ReleaseDC( HDC hdc )
	{
		if ( hdc
			&& m_pGraphics )
		{
			m_pGraphics->ReleaseHDC( hdc );
		}
	}
public:
	HBrush SetBrush( HBrush brush )
	{
		HBrush hOld = (HBrush)(void*)m_pBrush;
		m_pBrush = (Brush*)(void*)brush;
		return hOld;
	}

	ARGB SetBrushColor( ARGB argb )
	{
		if ( m_pBrush 
			&& m_pBrush->GetType() == BrushTypeSolidColor )
		{
			Color argbOld;
			((SolidBrush*)m_pBrush)->GetColor( &argbOld );
			((SolidBrush*)m_pBrush)->SetColor( Color(argb) );
			return argbOld.GetValue();
		}
		return X_TRANSPARENT;
	}
	virtual ARGB GetBrushColor()
	{
		if ( m_pBrush && m_pBrush->GetType() == BrushTypeSolidColor )
		{
			Color argb;
			((SolidBrush*)m_pBrush)->GetColor( &argb );
			return argb.GetValue();
		}
		return 0;
	}

	virtual ARGB GetPenColor()
	{
		if ( m_pPen )
		{
			Color argb;
			m_pPen->GetColor( &argb );
			return argb.GetValue();
		}
		return 0;
	}

	HPen SetPen( HPen pen )
	{
		HPen hOld = (HPen)(void*)m_pBrush;
		m_pPen = (Pen*)(void*)pen;
		return hOld;
	}

	ARGB SetPenColor( ARGB argb )
	{
		if ( m_pPen )
		{
			Color argbOld;
			m_pPen->GetColor( &argbOld );
			m_pPen->SetColor( Color(argb) );
			return argbOld.GetValue();
		}
		return X_TRANSPARENT;
	}

	BOOL FillRegion( HRGN hRgn )
	{
		if ( m_pBrush
			&& hRgn )
		{
			Region region( hRgn );
			m_pGraphics->FillRegion( m_pBrush, &region );
			return TRUE;
		}
		return FALSE;
	}

	BOOL FillRectangle( int x, int y, int width, int height )
	{
		if ( m_pBrush )
		{
			return Ok == m_pGraphics->FillRectangle( m_pBrush, x, y, width, height );
		}
		return FALSE;
	}

	BOOL DrawRectangle( int x, int y, int width, int height )
	{
		if ( m_pPen )
		{
			return Ok == m_pGraphics->DrawRectangle( m_pPen, x, y, width, height );
		}
		return S_FALSE;
	}

	BOOL DrawEllipse( int x, int y, int width, int height )
	{
		if ( m_pPen )
		{
			return Ok == m_pGraphics->DrawEllipse( m_pPen, x, y, width, height );
		}
		return FALSE;
	}

	BOOL PtVisible( int x, int y )
	{
		return m_pGraphics->IsVisible( x, y );
	}

	BOOL RectVisible( int x, int y, int x1, int y1 )
	{
		return m_pGraphics->IsVisible( x, y, x1-x, y1-y );
	}

	BOOL RectVisible( LPCRECT lpRect )
	{
		return m_pGraphics->IsVisible( lpRect->left, lpRect->top, RECT_W(*lpRect), RECT_H(*lpRect) );
	}

	virtual BOOL SetClip( int x, int y, int width, int height )
	{
		Rect rect( x, y, width, height );
		return Ok == m_pGraphics->SetClip( rect );
	}
	virtual BOOL SetClip( HRGN hRgn )
	{
		return Ok == m_pGraphics->SetClip( hRgn );
	}

	virtual BOOL ResetClip(  )
	{
		return Ok == m_pGraphics->ResetClip();
	}

	virtual HRGN GetClip(  )
	{
		Region region;
		if( Ok == m_pGraphics->GetClip( &region ) )
		{
			return region.GetHRGN(m_pGraphics);
		}
		return NULL;
	}
	
	int SetTabStop( int nTabStop );
	int GetTabStop();
	BOOL DrawImage( IImage* pImage, LPCRECT lpcDest, LPRECT lpcSrc = NULL, UINT uDrawMode = DM_NORMAL, int rotate = 0 );
	BOOL DrawImage( IMG* pImage, LPCRECT lpcDest, int rotate = 0 );
	BOOL DrawString( LPCTSTR lpszText, int nLength, LPRECT lpcRect, HFONT hFont, UINT uFormat, int rotate = 0 );
	BOOL MeasureString( LPCTSTR lpszText, int nLength, /*[in/out]*/LPRECT lpcRect, HFONT hFont, UINT uFormat );
	BOOL MeasureCharacterRects( LPCTSTR lpszText, int nLength, HFONT hFont, UINT uFormat, LPRECT lpRect, int nRectCount );
	BOOL DrawPath( HPath path );
	BOOL FillPath( HPath path );
	virtual BOOL DrawLine( int x, int y, int x1, int y1 )
	{
		if ( m_pGraphics
			&& m_pPen )
		{
			m_pGraphics->DrawLine( m_pPen, x, y, x1, y1 );
			return TRUE;
		}
		return FALSE;
	}
private:
	Graphics* m_pGraphics;
	Pen*	  m_pPen;
	Brush*    m_pBrush; 
	int		  m_nTabStop;
};

class ImagingFactory
{
	DECLARE_SGT_ATTRIBUTE();
public:
	IImage*  CreateImage( LPCSTR pszImageFileName );
	IImage*  CreateImage( LPCWSTR pszImageFileName );
	IImage*  CreateImage( LPCVOID lpBuffer, LONG lBufferSize );
	IImage*  CreateImage( HICON hIcon );
	IImage*  CreateImage( HBITMAP hbm, HPALETTE hpl );
	HRGN	 CreateRgn( const IImage* pImage );
	HRGN	 CreatePartRgn( const IImage* pImage, LPCRECT lpRect );
	IImage*  CloneImage( Bitmap* pBitmap );
	ARGB	 GetAvgColor( const IImage* pImage );
	void	 DestroyImage( IImage* pImage );
protected:
	ImagingFactory(void);
	~ImagingFactory(void);
private:
#ifdef WINCE
	IImagingFactory* m_pHelper;
#else
	ULONG_PTR m_ulGdiToken;
#endif
	
};
DECLARE_SGT_CLASS( ImagingFactory );
#endif