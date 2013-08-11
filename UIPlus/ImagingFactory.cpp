#include "Switch.h"
#include "ImagingFactory.h"
#include "CPTDC.h"
#include "UIType.h"
#include "UIPlus.h"

#ifdef WINCE
typedef HRESULT (__stdcall *DllGetClassObjectPtr)(
	__in   REFCLSID rclsid,
	__in   REFIID riid,
	__out  LPVOID *ppv
	);
const static GUID CLSID_ImagingFactory = {0x327abda8,0x072b,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e};
const static GUID IID_IImagingFactory = {0x327abda7,0x072b,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e};
#endif

SGT_ImagingFactory g_ImageFactory;
ImagingFactory::ImagingFactory(void)
#ifdef WINCE
:m_pHelper(NULL)
#else
:m_ulGdiToken(0)
#endif
{
#ifdef WINCE
	HRESULT hr = S_FALSE;
	if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		LOG(_T( "CoInitializeEx Failed\n" ));
		return;
	}
	HANDLE hImageLib = LoadLibrary( _T("imaging.dll") );
	if ( hImageLib )
	{
		//获取绘画工厂
		DllGetClassObjectPtr GetObject = (DllGetClassObjectPtr)GetProcAddress( (HMODULE)hImageLib, _T("DllGetClassObject") );
		if ( GetObject )
		{
			IClassFactory* pFactory= NULL;
			if( SUCCEEDED( hr = GetObject( CLSID_ImagingFactory, IID_IClassFactory, (void **)&pFactory ) ))
			{
				if ( !pFactory )
				{
					LOG( _T("ImagingFactory Factory Failed\n") );
					CoUninitialize();
					return;
				}
				//创建绘画工厂对象
				if( !SUCCEEDED(pFactory->CreateInstance(NULL, IID_IImagingFactory, (void**)&m_pHelper )) || NULL == m_pHelper )
				{
					LOG( _T("ImagingFactory Failed\n") );
					pFactory->Release();
					CoUninitialize();
					return;
				}
				//OK 
				pFactory->Release();
			}
		}
	}
#else
	GdiplusStartupInput input;
	GdiplusStartup( &m_ulGdiToken, &input, NULL );
#endif
}

ImagingFactory::~ImagingFactory(void)
{
#ifdef WINCE
	if ( m_pHelper )
	{
		m_pHelper->Release();
		m_pHelper = NULL;
		CoUninitialize();
	}
#else
	GdiplusShutdown( m_ulGdiToken );
#endif
}

IImage* ImagingFactory::CreateImage( LPCSTR pszImageFileName )
{
	ImageImpl* pImage = NULL;
	LPWSTR pwszFile = NULL;
	try
	{
		pwszFile = new WCHAR[MAX_PATH];
		RtlZeroMemory( pwszFile, MAX_PATH*sizeof(WCHAR) );
	}
	catch( ... )
	{
		delete pwszFile;
		pwszFile = NULL;
	}
	MultiByteToWideChar( CP_ACP, 0, pszImageFileName, -1, pwszFile, MAX_PATH );

	if ( pwszFile )
	{
#ifdef WINCE
		if ( m_pHelper )
		{	
			if (!SUCCEEDED (m_pHelper->CreateImageFromFile ( pwszFile, &pImage))) //从文件中创建对象
			{ 
				LOG( _T("CreateImageFromFile Failed\n") );
			}	
		}
#else
		try
		{
			pImage = new ImageImpl;
			if( !pImage->FromFile(pwszFile) )
			{
				delete pImage;
				pImage = NULL;
			}
		}
		catch( ... )
		{
			delete pImage;
			pImage = NULL;
		}
#endif
		delete pwszFile;
		pwszFile = NULL;

	}
	return pImage;
}

IImage* ImagingFactory::CreateImage( LPCWSTR pszImageFileName )
{
	ImageImpl* pImage = NULL;
	if ( pszImageFileName )
	{
#ifdef WINCE
		if ( m_pHelper )
		{	
			if (!SUCCEEDED (m_pHelper->CreateImageFromFile ( pszImageFileName, &pImage))) //从文件中创建对象
			{ 
				LOG( _T("CreateImageFromFile Failed\n") );
			}	
		}
#else
		try
		{
			pImage = new ImageImpl;
			if( !pImage->FromFile(pszImageFileName) )
			{
				delete pImage;
				pImage = NULL;
			}
		}
		catch( ... )
		{
			delete pImage;
			pImage = NULL;
		}
#endif
	}
	return pImage;
}

HRGN ImagingFactory::CreateRgn( const IImage* hImage )
{
	if ( !hImage )
	{
		return NULL;
	}
	//创建总的窗体区域，初始region为0
	HRGN hrgn;
	hrgn = ::CreateRectRgn(0,0,0,0);

	ImageInfo info;
	hImage->GetImageInfo( &info );
	for(UINT y=0;y<info.Height ;y++)
	{
		HRGN rgnTemp; //保存临时region

		UINT iX = 0;
		Color clr;
		do
		{
			//跳过透明色找到下一个非透明色的点.
			while (iX < info.Width  
				&& hImage->GetPixel(iX, y, clr )
				&& clr.GetAlpha( ) == 0 )
				iX++;

			//记住这个起始点
			int iLeftX = iX;

			//寻找下个透明色的点
			while (iX < info.Width  
				&& hImage->GetPixel(iX, y, clr )
				&& clr.GetAlpha( ) != 0)
				++iX;
			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp=::CreateRectRgn(iLeftX, y, iX, y+1);
			//合并到主"region".
			CombineRgn( hrgn,hrgn,rgnTemp, RGN_OR);
			//删除临时"region",否则下次创建时和出错
			::DeleteObject(rgnTemp);
		}while(iX <info.Width );
		iX = 0;
	}
	return hrgn;
}

IImage* ImagingFactory::CreateImage( LPCVOID lpBuffer, LONG lBufferSize )
{
	ImageImpl* pImage = NULL;
	HGLOBAL  hGlobalBuffer = ::GlobalAlloc(GMEM_MOVEABLE, lBufferSize);
	if (hGlobalBuffer)
	{
		void* pBuffer = ::GlobalLock(hGlobalBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, lpBuffer, lBufferSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hGlobalBuffer, FALSE, &pStream) == S_OK)
			{
				try
				{
					pImage = new ImageImpl;
					if( !pImage->FromStream(pStream) )
					{
						delete pImage;
						pImage = NULL;
					}
				}
				catch( ... )
				{
					delete pImage;
					pImage = NULL;
				}
				pStream->Release();
			}
			::GlobalUnlock(hGlobalBuffer);
		}
		::GlobalFree(hGlobalBuffer);
		hGlobalBuffer = NULL;
	}
	return pImage;
}
IImage*  ImagingFactory::CreateImage( HICON hIcon )
{
	ImageImpl* pImage = NULL;
	try
	{
		pImage = new ImageImpl;
		if( pImage->FromIcon( hIcon ) )
		{
			return pImage;
		}
		delete pImage;
		pImage = NULL;
	}
	catch( ... )
	{
		delete pImage;
		pImage = NULL;
	}
	return pImage;
}

IImage*  ImagingFactory::CreateImage( HBITMAP hbm, HPALETTE hpl )
{
	ImageImpl* pImage = NULL;
	try
	{
		pImage = new ImageImpl;
		if( pImage->FromHBITMAP(  hbm, hpl ) )
		{
			return pImage;
		}
		delete pImage;
		pImage = NULL;
	}
	catch( ... )
	{
		delete pImage;
		pImage = NULL;
	}
	return pImage;
}

ARGB ImagingFactory::GetAvgColor( const IImage* hImage )
{
	if ( !hImage )
	{
		return -1;
	}
	ImageInfo info;
	hImage->GetImageInfo( &info );
	if ( info.Width == info.Height
		&& info.Width == 0 )
	{
		return -1;
	}
	long r = 0;
	long g = 0;
	long b = 0;
	long a = 0;
	Color clr = 0;
	for(UINT y=0;y<info.Height ;y++)
	{
		for ( UINT x= 0; x<info.Width; ++x )
		{
			hImage->GetPixel( x, y, clr );
			r += clr.GetRed();
			g += clr.GetGreen();
			b += clr.GetBlue();
			a += clr.GetAlpha();
		}
	}
	//alpha 混合
	long colors = info.Width* info.Height;
	BYTE alpha = (BYTE)(a/colors)&0xFF;
	return Color( 0xFE, (BYTE)(((r/colors)*alpha)>>8)&0xFF, (BYTE)(((g/colors)*alpha)>>8)&0xFF, (BYTE)(((b/colors)*alpha)>>8)&0xFF ).GetValue();
}

void ImagingFactory::DestroyImage( IImage* pImage )
{
	ImageImpl* pImpl = (ImageImpl*)pImage;
	delete pImpl;
	pImage = NULL;
}

IImage* ImagingFactory::CloneImage( Bitmap* pBitmap )
{
	ImageImpl* pImage = NULL;
	try
	{
		pImage = new ImageImpl;
		if( !pImage->Clone( pBitmap ) )
		{
			delete pImage;
			pImage = NULL;
		}
	}
	catch( ... )
	{
		delete pImage;
		pImage = NULL;
	}
	return pImage;
}

HRGN ImagingFactory::CreatePartRgn( const IImage* pImage, LPCRECT lpRect )
{
	if ( !pImage )
	{
		return NULL;
	}
	//创建总的窗体区域，初始region为0
	HRGN hrgn = ::CreateRectRgn(0,0,0,0);
	ImageInfo info;
	pImage->GetImageInfo( &info );
	UINT xMax = (UINT)min( lpRect->right, (LONG)info.Width );
	UINT yMax = (UINT)min( lpRect->bottom, (LONG)info.Height );
	
	for(UINT y=lpRect->top;y<yMax ;y++)
	{	
		UINT iX = lpRect->left;
		Color clr;
		do
		{
			HRGN rgnTemp; //保存临时region
			//跳过透明色找到下一个非透明色的点.
			while (iX < xMax 
				&& pImage->GetPixel(iX, y, clr )
				&& clr.GetAlpha( ) == 0 )
					++iX;

			//记住这个起始点
			int iLeftX = iX;

			//寻找下个透明色的点
			while (iX < xMax  
				&& pImage->GetPixel(iX, y, clr )
				&& clr.GetAlpha( ) != 0)
				++iX;
			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp=::CreateRectRgn(iLeftX, y, iX, y+1);
			//合并到主"region".
			CombineRgn( hrgn,hrgn,rgnTemp, RGN_OR);
			//删除临时"region",否则下次创建时和出错
			::DeleteObject(rgnTemp);
		}while(iX <info.Width );
		iX = lpRect->left;
	}
	return hrgn;
}

HBITMAP ImageImpl::GetHBITMAP( ARGB TransColor )
{
	HBITMAP hBmp;
	if ( m_bitmap )
	{
		Color clrBk(0);
		if ( TransColor != (ARGB)-1 )
		{
			clrBk.SetFromCOLORREF( TransColor );
		}
		m_bitmap->GetHBITMAP( clrBk, &hBmp );
	}
	return hBmp;
}

BOOL CPainter::DrawImage( IImage* pImage, LPCRECT lpcDest, LPRECT lpcSrc /*= NULL*/, UINT uDrawMode /*= DM_NORMAL*/, int rotate /*= 0 */ )
{
	if ( !pImage )
	{
		return FALSE;
	}
	if ( !pImage->GetBitmap() )
	{
		return FALSE;
	}
	if ( rotate > 0 )
	{
		Matrix   matrix; 
		matrix.RotateAt((REAL)rotate,
			PointF( (pImage->GetWidth() - 1)/(REAL)2.0, (REAL)(pImage->GetHeight()-1)/(REAL)2.0), 
			MatrixOrderAppend);
		m_pGraphics->SetTransform(&matrix);
	}
	RECT tmpSrc = {0};
	switch( uDrawMode )
	{
	case DM_TILE:
		{
			if ( NULL == lpcSrc )
			{
				TextureBrush brush( pImage->GetBitmap() );
				m_pGraphics->FillRectangle( &brush, lpcDest->left, lpcDest->top, lpcDest->right-lpcDest->left, lpcDest->bottom-lpcDest->top );
			}
			else
			{
				Rect rect(lpcSrc->left, lpcSrc->top, RECT_W(*lpcSrc),RECT_H(*lpcSrc));
				TextureBrush brush( pImage->GetBitmap(), rect );
				m_pGraphics->FillRectangle( &brush, lpcDest->left, lpcDest->top, lpcDest->right-lpcDest->left, lpcDest->bottom-lpcDest->top );
			}
		}
		break;
	case DM_STRETCH:
		{
			if( NULL == lpcSrc )
			{
				lpcSrc = &tmpSrc;
				tmpSrc.left = 0;
				tmpSrc.top = 0;
				tmpSrc.right = pImage->GetWidth();
				tmpSrc.bottom = pImage->GetHeight();
			}
		}
	default:
		{
			if ( NULL == lpcSrc )
			{	
				if ( lpcDest )
				{
					Rect rectDest( lpcDest->left, lpcDest->top, lpcDest->right-lpcDest->left, lpcDest->bottom-lpcDest->top );
					m_pGraphics->DrawImage( pImage->GetBitmap(), rectDest );
				}
				else
				{
					m_pGraphics->DrawImage( pImage->GetBitmap(), 0, 0 );
				}
			}
			else
			{
				ImageAttributes ImgAtt;
				ImgAtt.SetWrapMode(WrapModeTileFlipXY);
				if ( lpcDest )
				{
					Rect rectDest( lpcDest->left, lpcDest->top, lpcDest->right-lpcDest->left, lpcDest->bottom-lpcDest->top );
					m_pGraphics->DrawImage( pImage->GetBitmap(), rectDest, lpcSrc->left,\
						lpcSrc->top, lpcSrc->right-lpcSrc->left,lpcSrc->bottom-lpcSrc->top,UnitPixel, &ImgAtt );
				}
				else
				{
					 m_pGraphics->DrawImage( pImage->GetBitmap(), 0, 0, lpcSrc->left,\
						lpcSrc->top, lpcSrc->right-lpcSrc->left,lpcSrc->bottom-lpcSrc->top,UnitPixel, &ImgAtt );
				}
			}
		}
		break;
	}
 	if ( rotate > 0 )
 	{
 		m_pGraphics->ResetTransform();
 	}
	return TRUE;
}

BOOL CPainter::DrawImage( IMG* pImage, LPCRECT lpcDest, int rotate /*= 0 */ )
{
	if ( !pImage
		|| !lpcDest )
	{
		return FALSE;
	}
	ImageInfo info = {0};
	if ( pImage->hImage )
	{
		if ( RECT_ISEMPTY( pImage->rcValid ) )
		{
			pImage->hImage->GetImageInfo( &info );
		}
		else
		{
			info.Width = RECT_W( pImage->rcValid );
			info.Height = RECT_H( pImage->rcValid );
		}
	}
	else if( pImage->imageAvgClr != X_TRANSPARENT )
	{
		ARGB clrOld = SetBrushColor( pImage->imageAvgClr );
		FillRectangle( lpcDest->left, lpcDest->top, RECT_W(*lpcDest), RECT_H(*lpcDest) );
		SetBrushColor( clrOld );
		return TRUE;
	}
	else
	{
		return TRUE;
	}

	RECT rcTmp;
	RECT rcImg;
	if ( pImage->margin.top > 0 )
	{
		//topleft
		RECT_S( rcTmp, lpcDest->left, lpcDest->top, lpcDest->left+pImage->margin.left,lpcDest->top+pImage->margin.top );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			rcImg = rcTmp;
			RECT_TO(rcImg, pImage->rcValid.left, pImage->rcValid.top );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}

		//topmidle
		RECT_S( rcTmp, lpcDest->left+pImage->margin.left, lpcDest->top, lpcDest->right-pImage->margin.right,lpcDest->top+pImage->margin.top );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			RECT_S( rcImg, pImage->rcValid.left+pImage->margin.left, pImage->rcValid.top,\
				pImage->rcValid.left+info.Width-pImage->margin.right, pImage->rcValid.top+pImage->margin.top );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}

		//topright
		RECT_S( rcTmp, lpcDest->right-pImage->margin.right, lpcDest->top, lpcDest->right,lpcDest->top+pImage->margin.top );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			rcImg = rcTmp;
			RECT_TO(rcImg, pImage->rcValid.left+info.Width-pImage->margin.right, pImage->rcValid.top );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}
	}

	if ( pImage->margin.left > 0 )
	{
		//midleleft
		RECT_S(rcTmp, lpcDest->left, lpcDest->top+pImage->margin.top, lpcDest->left+pImage->margin.left,lpcDest->bottom-pImage->margin.bottom );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			RECT_S(rcImg, pImage->rcValid.left, pImage->rcValid.top+pImage->margin.top, \
				pImage->rcValid.left+pImage->margin.left, pImage->rcValid.top+info.Height-pImage->margin.bottom );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}

	}

	if ( pImage->margin.right > 0 )
	{
		//midleright
		RECT_S( rcTmp, lpcDest->right-pImage->margin.right, lpcDest->top + pImage->margin.top, lpcDest->right,lpcDest->bottom-pImage->margin.bottom );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			RECT_S(rcImg,pImage->rcValid.left+info.Width - pImage->margin.right, pImage->rcValid.top+pImage->margin.top,\
				pImage->rcValid.left+info.Width, pImage->rcValid.top+info.Height-pImage->margin.bottom );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}
	}

	//midle
	RECT_S(rcTmp, lpcDest->left+pImage->margin.left, lpcDest->top + pImage->margin.top, lpcDest->right-pImage->margin.right,lpcDest->bottom-pImage->margin.bottom );
	if ( !RECT_ISEMPTY(rcTmp) )
	{
		RECT_S( rcImg, pImage->rcValid.left+pImage->margin.left, pImage->rcValid.top+pImage->margin.top, \
			pImage->rcValid.left+info.Width-pImage->margin.right, pImage->rcValid.top+info.Height-pImage->margin.bottom);
		DrawImage( pImage->hImage, &rcTmp, &rcImg );
	}

	if ( pImage->margin.bottom > 0 )
	{
		//bottomleft
		RECT_S(rcTmp, lpcDest->left, lpcDest->bottom - pImage->margin.bottom, lpcDest->left+pImage->margin.left, lpcDest->bottom );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			RECT_S(rcImg, pImage->rcValid.left, pImage->rcValid.top+info.Height-pImage->margin.bottom, \
				pImage->rcValid.left+pImage->margin.left, pImage->rcValid.top+info.Height );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}

		//bottommidle	
		RECT_S( rcTmp, lpcDest->left+pImage->margin.left, lpcDest->bottom - pImage->margin.bottom, lpcDest->right-pImage->margin.right,lpcDest->bottom );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			RECT_S( rcImg, pImage->rcValid.left+pImage->margin.left, pImage->rcValid.top+info.Height-pImage->margin.bottom,\
				pImage->rcValid.left+info.Width-pImage->margin.right, pImage->rcValid.top+info.Height );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}

		//bottomright
		RECT_S( rcTmp, lpcDest->right-pImage->margin.right, lpcDest->bottom - pImage->margin.bottom, lpcDest->right,lpcDest->bottom );
		if ( !RECT_ISEMPTY(rcTmp) )
		{
			RECT_S( rcImg, pImage->rcValid.left+info.Width-pImage->margin.right, pImage->rcValid.top+info.Height-pImage->margin.bottom,\
				pImage->rcValid.left+info.Width, pImage->rcValid.top+info.Height );
			DrawImage( pImage->hImage, &rcTmp, &rcImg );
		}
	}
	return TRUE;
}

// BOOL _DrawText( IPainter* pPainter, LPCTSTR lpszText, int nLength, LPRECT lpcRect, HFONT hFont, UINT uFormat )
// {
// 	HDC _hDC = pPainter->GetDC();
// 	if ( _tcslen( lpszText ) > 0 )
// 	{
// 		COLORREF clrText  = pPainter->GetBrushColor();
// 		int nOldBkMode = SetBkMode( _hDC,  TRANSPARENT );
// 		COLORREF clrOld = ::SetTextColor( _hDC,  clrText );
// 
// 		UINT nFormat = DT_NOPREFIX;
// 		if ( uFormat & XWS_ALIGNLEFT )
// 		{
// 			nFormat |= DT_LEFT;
// 		}
// 		else if( uFormat & XWS_ALIGNRIGHT )
// 		{
// 			nFormat |= DT_RIGHT;
// 		}
// 
// 		if ( uFormat & XWS_ALIGNHCENTER  )
// 		{
// 			nFormat |= DT_CENTER;
// 		}
// 		if( uFormat & XWS_ALIGNVCENTER )
// 		{
// 			nFormat |= DT_VCENTER;
// 		}
// 
// 		if ( uFormat & XWS_ENDELLIPSE )
// 		{
// 			nFormat |= DT_END_ELLIPSIS;
// 		}
// 		if ( uFormat & XWS_PATHELLIPSE )
// 		{
// 			nFormat |= DT_PATH_ELLIPSIS;
// 		}
// 		if ( uFormat & XWS_SINGLELINE )
// 		{
// 			nFormat |= DT_SINGLELINE;
// 		}
// 		else
// 		{
// 			nFormat |= DT_WORDBREAK | DT_EDITCONTROL;
// 		}
// 		HFONT hDrawFont = hFont?hFont:UIGetDefaultFont();
// 		HFONT pOldFont = NULL;
// 		if( hDrawFont )
// 		{
// 			pOldFont = (HFONT)SelectObject(_hDC, hDrawFont );
// 		}
// 		DrawTextEx( _hDC, const_cast<LPSTR>(lpszText), nLength, lpcRect, nFormat, NULL );
// 		SetBkMode( _hDC,  nOldBkMode );
// 		::SetTextColor( _hDC,  clrOld );
// 		if ( hDrawFont )
// 		{
// 			(VOID)SelectObject(_hDC, pOldFont );
// 		}
// 		pPainter->ReleaseDC( _hDC );
// 	}
// 	return TRUE;
// }

BOOL CPainter::DrawString( LPCTSTR lpszText, int nLength, LPRECT lpcRect, HFONT hFont, UINT uFormat, int rotate /*= 0 */ )
{
	if ( lpszText && lpszText[0]!=0 )
	{	
		//return _DrawText( this, lpszText, nLength, lpcRect, hFont, uFormat );
		HFONT hDrawFont = hFont?hFont:UIGetDefaultFont();
		StringFormat strFormat;
 		if ( uFormat & XWS_ALIGNLEFT )
 		{
 			strFormat.SetAlignment( StringAlignmentNear );
 		}
 		else if( uFormat & XWS_ALIGNRIGHT)
 		{
 			strFormat.SetAlignment( StringAlignmentFar );
 		}
 
 		if ( uFormat & XWS_ALIGNVCENTER )
 		{
 			strFormat.SetLineAlignment( StringAlignmentCenter );
 		}
 		if( uFormat & XWS_ALIGNHCENTER )
 		{
 			strFormat.SetAlignment( StringAlignmentCenter );
 		}
 		if ( uFormat & XWS_ENDELLIPSE )
 		{
 			strFormat.SetTrimming( StringTrimmingEllipsisCharacter );
 		}
 		if ( uFormat & XWS_PATHELLIPSE )
 		{
 			strFormat.SetTrimming( StringTrimmingEllipsisPath );
 		}
 		if ( uFormat & XWS_SINGLELINE )
 		{
			strFormat.SetFormatFlags( strFormat.GetFormatFlags()|StringFormatFlagsLineLimit|StringFormatFlagsNoWrap );
 		}
 		if ( uFormat & XWS_NOPREFIX )
 		{
 			strFormat.SetHotkeyPrefix( HotkeyPrefixNone );
 		}
 		else if ( uFormat & XWS_PREFIXHIDE )
 		{
 			strFormat.SetHotkeyPrefix( HotkeyPrefixHide );
 		}
 		else
 		{
 			strFormat.SetHotkeyPrefix( HotkeyPrefixShow );
 		}
		if ( uFormat & XWS_VERTTEXTOUT )
		{
			strFormat.SetFormatFlags( strFormat.GetFormatFlags()|StringFormatFlagsDirectionVertical );
		}
		if ( uFormat & XWS_RIGHTTOLEFT )
		{
			strFormat.SetFormatFlags( strFormat.GetFormatFlags()|StringFormatFlagsDirectionRightToLeft );
		}
		if( !(uFormat & XWS_CLIPMODE) )
		{
			strFormat.SetFormatFlags( StringFormatFlagsNoClip );
		}
		float tabs[] = {(float)m_nTabStop};
		strFormat.SetTabStops(0.0f, 1, tabs );
		HDC hdc = m_pGraphics->GetHDC();
		Font font( hdc, hDrawFont );
		m_pGraphics->ReleaseHDC( hdc );
		m_pGraphics->SetTextRenderingHint( TextRenderingHintAntiAliasGridFit );
		m_pGraphics->DrawString( XT2W( lpszText ).c_str(), nLength, &font, \
			RectF( (REAL)lpcRect->left, (REAL)lpcRect->top, (REAL)RECT_W(*lpcRect), (REAL)RECT_H(*lpcRect) ), &strFormat, m_pBrush );
		
	}
	return TRUE;
}

BOOL CPainter::MeasureString( LPCTSTR lpszText, int nLength, LPRECT lpRect, HFONT hFont, UINT uFormat )
{
	if ( lpszText && lpszText[0]!=0 )
	{	
		HFONT hDrawFont = hFont?hFont:UIGetDefaultFont();
		StringFormat strFormat;
		if ( uFormat & XWS_ALIGNLEFT )
		{
			strFormat.SetAlignment( StringAlignmentNear );
		}
		else if( uFormat & XWS_ALIGNRIGHT)
		{
			strFormat.SetAlignment( StringAlignmentFar );
		}

		if ( uFormat & XWS_ALIGNVCENTER )
		{
			strFormat.SetLineAlignment( StringAlignmentCenter );
		}
		if( uFormat & XWS_ALIGNHCENTER )
		{
			strFormat.SetAlignment( StringAlignmentCenter );
		}
		if ( uFormat & XWS_ENDELLIPSE )
		{
			strFormat.SetTrimming( StringTrimmingEllipsisCharacter );
		}
		if ( uFormat & XWS_PATHELLIPSE )
		{
			strFormat.SetTrimming( StringTrimmingEllipsisPath );
		}
		if ( uFormat & XWS_SINGLELINE )
		{
			strFormat.SetFormatFlags( strFormat.GetFormatFlags()|StringFormatFlagsLineLimit|StringFormatFlagsNoWrap );
		}
		if ( uFormat & XWS_NOPREFIX )
		{
			strFormat.SetHotkeyPrefix( HotkeyPrefixNone );
		}
		else if ( uFormat & XWS_PREFIXHIDE )
		{
			strFormat.SetHotkeyPrefix( HotkeyPrefixHide );
		}
		else
		{
			strFormat.SetHotkeyPrefix( HotkeyPrefixShow );
		}
		if ( uFormat & XWS_VERTTEXTOUT )
		{
			strFormat.SetFormatFlags( strFormat.GetFormatFlags()|StringFormatFlagsDirectionVertical );
		}
		if ( uFormat & XWS_RIGHTTOLEFT )
		{
			strFormat.SetFormatFlags( strFormat.GetFormatFlags()|StringFormatFlagsDirectionRightToLeft );
		}
		float tabs[] = {(float)m_nTabStop};
		strFormat.SetTabStops(0.0f, 1, tabs );
		RectF rectOut;
		HDC hdc = m_pGraphics->GetHDC();
		Font font( hdc, hDrawFont );
		m_pGraphics->ReleaseHDC( hdc );
		m_pGraphics->SetTextRenderingHint( TextRenderingHintAntiAliasGridFit );
		m_pGraphics->MeasureString( XT2W( lpszText ).c_str(), nLength, &font, \
			RectF( (REAL)lpRect->left, (REAL)lpRect->top, (REAL)RECT_W(*lpRect),(REAL)RECT_H(*lpRect) ),&strFormat, &rectOut );
		lpRect->right = lpRect->left+(int)rectOut.Width;
		lpRect->bottom = lpRect->top+(int)rectOut.Height;
	}
	return TRUE;
}

int CPainter::SetTabStop( int nTabStop )
{
	int nOld = m_nTabStop;
	m_nTabStop = nTabStop;
	return nOld;
}

int CPainter::GetTabStop()
{
	return m_nTabStop;
}

BOOL CPainter::DrawPath( HPath path )
{
	if ( path
		&& m_pGraphics
		&& m_pPen )
	{
		return Ok == m_pGraphics->DrawPath( m_pPen, (GraphicsPath*)path );	
	}
	return FALSE;
}

BOOL CPainter::FillPath( HPath path )
{
	if ( path
		&& m_pGraphics
		&& m_pBrush )
	{
		return Ok == m_pGraphics->FillPath( m_pBrush, (GraphicsPath*)path );	
	}
	return FALSE;
}

BOOL CPainter::MeasureCharacterRects( LPCTSTR lpszText, int nLength, HFONT hFont, UINT uFormat, LPRECT lpRect, int nRectCount )
{
	assert( lpszText );
	if ( NULL == lpRect
		|| nRectCount <= 0 )
	{
		return TRUE;
	}
	if ( NULL == hFont )
	{
		hFont = UIGetDefaultFont();
	}
	CharacterRange* pcRanges = NULL;
	Region* pcRegions = NULL;
	try
	{
		if ( nRectCount < nLength )
		{
			nLength = nRectCount;
		}
		StringFormat strFormat;
 		if ( uFormat & XWS_NOPREFIX )
 		{
 			strFormat.SetHotkeyPrefix( HotkeyPrefixNone );
 		}
 		else if ( uFormat & XWS_PREFIXHIDE )
 		{
 			strFormat.SetHotkeyPrefix( HotkeyPrefixHide );
 		}
 		else
 		{
 			strFormat.SetHotkeyPrefix( HotkeyPrefixShow );
 		}
		int _RcCount = min( 32, nRectCount );
		pcRanges = new CharacterRange[_RcCount];
		for ( int _i = 0; _i < _RcCount; ++_i )
		{
			pcRanges[_i].First = _i;
			pcRanges[_i].Length= 1;
		}
		
		HDC hdc = m_pGraphics->GetHDC();
		Font font( hdc, hFont );
		m_pGraphics->ReleaseHDC( hdc );
		RectF rect( 0, 0, 10000, 10000 );
		pcRegions = new Region[_RcCount];
		m_pGraphics->SetTextRenderingHint( TextRenderingHintAntiAliasGridFit );
		int _pre = 0;
		int _offset = 0;
		for ( int _i = min(32, nLength); _i <= nLength;  )
		{
			strFormat.SetMeasurableCharacterRanges( _i-_pre, pcRanges );
			m_pGraphics->MeasureCharacterRanges(XT2W( lpszText ).c_str()+_pre, _i-_pre, &font, rect, &strFormat, _i-_pre, pcRegions );
			HRGN hrgn;
			hrgn = pcRegions[0].GetHRGN( m_pGraphics );
			GetRgnBox( hrgn, &lpRect[_pre] );
			_offset = lpRect[_pre].left;
			if ( _pre > 0 )
			{
				_offset = lpRect[_pre].left-lpRect[_pre-1].right;
			}
			RECT_TOX(lpRect[_pre],lpRect[_pre].left-_offset);
			DeleteObject( hrgn );
			for ( int _j = 1; _j < _i-_pre; ++_j )
			{
				hrgn = pcRegions[_j].GetHRGN( m_pGraphics );
				GetRgnBox( hrgn, &lpRect[_pre+_j] );
				RECT_TOX(lpRect[_pre+_j],lpRect[_pre+_j].left-_offset);
				DeleteObject( hrgn );
			}
			_pre = _i;
			if ( _pre == nLength )
			{
				break;
			}
			_i += 32;
			if ( _i > nLength )
			{
				_i = nLength;
			}
			rect.X = (REAL)lpRect[_pre-1].right;
		}
		delete[] pcRegions;
		delete[] pcRanges;
		return TRUE;
	}
	catch( ... )
	{
		delete[] pcRegions;
		delete[] pcRanges;
	}
	return FALSE;
}
#ifdef __cplusplus
extern "C"
{
#endif
IMGAPI(IImage*) CreateImageA( LPCSTR pszImageFileName )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreateImage( pszImageFileName );
	}
	return NULL;
}

IMGAPI(IImage*) CreateImageW( LPCWSTR pszImageFileName )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreateImage( pszImageFileName );
	}
	return NULL;
}

IMGAPI(IImage*) CreateImage( LPCVOID lpBuffer, LONG lBufferSize )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreateImage( lpBuffer, lBufferSize );
	}
	return NULL;
}

IMGAPI(IImage*) CreateImageFromIcon( HICON hIcon )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreateImage( hIcon );
	}
	return NULL;
}

IMGAPI(IImage*) CreateImageFromBitmap( HBITMAP hbm, HPALETTE hpal )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreateImage( hbm, hpal );
	}
	return NULL;
}


IMGAPI(IImage*) CloneImage( Bitmap* pBitmap )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CloneImage( pBitmap );
	}
	return NULL;
}

IMGAPI(HRGN)	CreateRgn( const IImage* pImage )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreateRgn( pImage );
	}
	return NULL;
}



IMGAPI(ARGB)GetAvgColor( const IImage* pImage )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->GetAvgColor( pImage );
	}
	return NULL;
}

IMGAPI(void)	DestroyImage( IImage* pImage )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->DestroyImage( pImage );
	}
}

#ifdef SURPORT_EXT
IMGAPI(HRGN)	CreatePartRgn( const IImage* pImage, LPCRECT lpRect )
{
	if ( g_ImageFactory )
	{
		return g_ImageFactory->CreatePartRgn( pImage, lpRect );
	}
	return NULL;
}

IMGAPI(IImage*) GetTileImage( HDC hdc, IImage* pImage, ARGB bkClr, int nBorder /*= -1*/ )
{
	if( NULL == pImage )
	{
		return NULL;
	}
	if ( nBorder < 0 )
	{
		nBorder = 50;
	}
	ImageInfo imageInfo = {0};
	pImage->GetImageInfo( &imageInfo );
	if( imageInfo.Width == 0
		|| imageInfo.Height == 0 )
	{
		return NULL;
	}
 	CPTDC bufferDC;
 	bufferDC.GetDC( hdc );
	//HBITMAP hBmp=::CreateCompatibleBitmap( hdc, imageInfo.Width, imageInfo.Height );
	BYTE * pBits = NULL ;
	BITMAPINFOHEADER bmih;
	ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );
	bmih.biSize = sizeof (BITMAPINFOHEADER) ;
	bmih.biWidth = imageInfo.Width;
	bmih.biHeight = imageInfo.Height ;
	bmih.biPlanes = 1 ;
	bmih.biBitCount = 32; //这里一定要是32
	bmih.biCompression = BI_RGB ;
	bmih.biSizeImage = 0 ;
	bmih.biXPelsPerMeter = 0 ;
	bmih.biYPelsPerMeter = 0 ;
	bmih.biClrUsed = 0 ;
	bmih.biClrImportant = 0 ;
	HBITMAP hBmp = CreateDIBSection (NULL, (BITMAPINFO *) &bmih, DIB_RGB_COLORS, (void**)&pBits, NULL, 0) ;
 	HBITMAP hOldBmp = (HBITMAP)::SelectObject( bufferDC, hBmp );
	CPainter painter( bufferDC, true );
	painter.SetBrushColor( bkClr );
	painter.FillRectangle( 0, 0, imageInfo.Width, imageInfo.Height );
	RECT rcImage={ 0, 0, imageInfo.Width, imageInfo.Height };
	painter.DrawImage( pImage, &rcImage, &rcImage );
 	int nBoder = imageInfo.Width;
 	if ( nBoder > nBorder )
 	{
 		nBoder = nBorder;
 	}
	Color clrStart(0x20, (BYTE)(bkClr>>16), (BYTE)(bkClr>>8), (BYTE)bkClr );
	Color clrEnd( bkClr );
 	HRGN hrgn = CreatePartRgn( pImage, &RECT_C(imageInfo.Width - nBoder , 0, imageInfo.Width, imageInfo.Height ) );
 	if ( hrgn )
 	{
		RECT rcRgn;
 		::GetRgnBox(hrgn, &rcRgn );
		if( RECT_W(rcRgn) == nBoder )
		{
			HBrush hBrush = UICreateLinearBrush( &rcRgn, clrStart.GetValue(), clrEnd.GetValue(), LinearGradientModeHorizontal );
			HBrush hOld = painter.SetBrush( hBrush );
			painter.FillRegion( hrgn );
			painter.SetBrush( hOld );
			UIDestroyBrush( hBrush );
			hBrush = NULL;
		}
		DeleteObject(hrgn);
 	}
	nBoder = imageInfo.Height;
	if ( nBoder > nBorder )
	{
		nBoder = nBorder;
	}
	hrgn = CreatePartRgn( pImage, &RECT_C(0, imageInfo.Height - nBoder, imageInfo.Width, imageInfo.Height ) );
	if ( hrgn )
	{
		RECT rcRgn;
 		::GetRgnBox(hrgn, &rcRgn );
		if( RECT_H(rcRgn) == nBoder )
		{
			HBrush hBrush = UICreateLinearBrush( &rcRgn, clrStart.GetValue(), clrEnd.GetValue(), LinearGradientModeVertical );
			HBrush hOld = painter.SetBrush( hBrush );
			painter.FillRegion( hrgn );
			painter.SetBrush( hOld );
			UIDestroyBrush( hBrush );
			hBrush = NULL;
		}
		DeleteObject( hrgn );
		hrgn = NULL;
	}
	::SelectObject( bufferDC, hOldBmp );
	IImage* pRet = CreateImageFromBitmap( hBmp, NULL );
	DeleteObject( hBmp );
	return pRet;
}
IMGAPI(BOOL) DrawOuterGlow( HDC hDC, LPCRECT lpRect, LPCTSTR lpszString,\
							  HFONT hFont, int align, int format )
{
	return TRUE;
	if ( NULL == hDC 
		|| NULL == lpRect
		|| NULL == hFont
		|| NULL == lpszString)
	{
		return FALSE;
	}
	if ( RectVisible( hDC, lpRect ) )
	{
		Graphics graphics( hDC );
		LOGFONT lg = {0};
		GetObject(hFont, sizeof(LOGFONT), &lg);
		FontFamily fontFamily(XT2W(lg.lfFaceName).c_str());
		StringFormat strformat;
		wstring text = XT2W( lpszString );
		GraphicsPath path;

		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		path.AddString(text.c_str(), (INT)text.length(), &fontFamily, 
			FontStyleRegular, (REAL)abs(lg.lfHeight+2), Gdiplus::Point(lpRect->left-1,lpRect->top+2), &strformat );
		//int alpha = 16;
		for(int i=5; i<15; ++i)
		{
			Pen pen(Color(10, 255, 255, 255), (REAL)i);
			pen.SetLineJoin(LineJoinRound);
			graphics.DrawPath(&pen, &path);
		}
		graphics.FillPath(&SolidBrush( Color( 10, 255, 255, 255 ) ), &path);
	}
	return TRUE;
}

IMGAPI(IImage*) GetGrayImage( const IImage* pImage )
{
	if ( NULL == pImage )
	{
		return NULL;
	}

	ImageInfo info = {0};
	pImage->GetImageInfo( &info );
	if ( info.Width == 0
		|| info.Height == 0 )
	{
		return NULL;
	}
	IImage* pGrayImage = CloneImage( const_cast<Bitmap*>(pImage->GetBitmap()) );
	if ( pGrayImage )
	{
		Color color;
		BYTE  byteMid = 0;
		Bitmap* pbitmap = pGrayImage->GetBitmap();
		for( int i=info.Width; i>=0; i-- )
		{
			for( int j= info.Height; j>=0; j-- )
			{
				pbitmap->GetPixel(i,j, &color);
				if ( color.GetAlpha() != 0 )
				{
					//byteMid=color.GetR()>color.GetG()? color.GetR():color.GetG();
					//byteMid=byteMid>color.GetB()? byteMid:color.GetB();
					//求出平均三个色彩分量的平均值
					byteMid =(color.GetR()+color.GetG()+color.GetB())/3;
					pbitmap->SetPixel(i,j, Color(color.GetAlpha(),byteMid,byteMid,byteMid) );
				}
			}
		}
	}
	return pGrayImage;
}

IMGAPI(IPainter*) UICreateImagePainter( IImage* pImage, bool bClear )
{
	if ( pImage )
	{
		try
		{
			CPainter* painter = new CPainter( pImage, bClear );
			if ( painter )
			{
				if( painter->IsValid() )
				{
					return (IPainter*)painter;
				}
				delete painter;
				painter = NULL;
			}
		}
		catch( ... )
		{

		}	
	}
	return NULL;
}
IMGAPI(IPainter*) UICreateDCPainter( HDC hDC, bool bClear  )
{
	if ( hDC )
	{
		try
		{
			CPainter* painter = new CPainter( hDC, bClear );
			if ( painter )
			{
				if( painter->IsValid() )
				{
					return (IPainter*)painter;
				}
				delete painter;
				painter = NULL;
			}
		}
		catch( ... )
		{
			
		}
	}
	return NULL;
}
IMGAPI(IPainter*) UICreateWndPainter( HWND hWnd, bool bClear  )
{
	if ( hWnd )
	{
		try
		{
			CPainter* painter = new CPainter( hWnd, bClear );
			if ( painter )
			{
				if( painter->IsValid() )
				{
					return (IPainter*)painter;
				}
				delete painter;
				painter = NULL;
			}
		}
		catch( ... )
		{

		}	
	}
	return NULL;
}

IMGAPI(void) UIDestroyPainter( IPainter* painter )
{
	if ( painter )
	{
		CPainter* _delete = (CPainter*)painter;
		delete _delete;
		_delete = NULL;
	}
}

IMGAPI(HPen) UICreateSolidPen( ARGB argb, int width )
{
	try
	{

	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(HPen) UICreatePen( HBrush hBrush )
{
	try
	{

	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(void) UIDestroyPen( HPen hPen )
{
	Pen* pen = (Pen*)hPen;
	delete pen;
	pen = NULL;
}

IMGAPI(HBrush) UICreateSolidBrush( ARGB argb )
{
	try
	{
		SolidBrush* pBrush = new SolidBrush( Color(argb) );
		return (HBrush)(void*)pBrush;
	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(HBrush) UICreateLinearBrush( LPRECT lpRect, ARGB color1, ARGB color2, int mode )
{
	try
	{
		Rect rectRight( lpRect->left, lpRect->top, RECT_W(*lpRect), RECT_H(*lpRect) );
		LinearGradientBrush* pBrush = new LinearGradientBrush( rectRight, color1, color2, (LinearGradientMode)mode );
		return (HBrush)(void*)pBrush;
	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(HBrush) UICreateTextureBrush( IImage* pImage, LPRECT pRect )
{
	try
	{
		TextureBrush* pBrush = NULL;
		if ( NULL == pRect )
		{
			pBrush = new TextureBrush( pImage->GetBitmap() );
		}
		else
		{
			Rect rect(pRect->left, pRect->top, RECT_W(*pRect),RECT_H(*pRect));
			pBrush = new TextureBrush( pImage->GetBitmap(), rect );
		}
		return (HBrush)(void*)pBrush;
	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(void) UIDestroyBrush( HBrush hBrush )
{
	Brush* brush = (Brush*)hBrush;
	delete brush;
	brush = NULL;
}

IMGAPI(HPath) UICreatePolyPath( const Point* points, int nCount )
{
	try
	{
		GraphicsPath* path = new GraphicsPath;
		path->AddPolygon( points, nCount );
		return (HPath)path;
	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(HPath) UICreatePath( const Rect* pRect )
{
	try
	{
		GraphicsPath* path = new GraphicsPath;
		path->AddRectangle( *pRect );
		return (HPath)path;
	}
	catch( ... )
	{

	}
	return NULL;
}

IMGAPI(void) UIDestroyPath( HPath hPath )
{
	GraphicsPath* path = (GraphicsPath*)hPath;
	delete path;
	path = NULL;
}


#endif
#ifdef __cplusplus
}
#endif