// UIPlus.cpp : 定义 DLL 应用程序的导出函数。
//
#include "Switch.h"
#include "ImagingFactory.h"
#include "UIPlus.h"
#include "CPTDC.h"
#include "UICtrlBase.h"
#include <map>
#include <vector>
#include <WindowsX.h>
#include <CommCtrl.h>
#include "UIPopMenu.h"
#include "FontMgr.h"
#include <Imm.h>
#pragma comment( lib, "Imm32.lib" )
#ifdef SURPORT_DWM
#include "Dwm.h"
#endif
#pragma comment( lib, "Msimg32.lib")
#pragma comment( lib, "Comctl32.lib" )
using std::map;
using std::vector;
#ifdef __cplusplus
CUIPopMenu* pSingleMenu = NULL;
extern "C"
{
#endif

	struct WNDDATA
	{
		IUIBase*	pUI;
		IImage*     pImageBuffer;
		IImage*		pImageBkGnd;
		HBITMAP		m_hBkGnd;
		HBITMAP		m_hOldGnd;
	};

	struct VIEWHOOK
	{
		WNDPROC OrgProc;//原始窗口处理过程
		IImage* pImageBuffer;
		IImage* pImageBkGnd;
		RECT   m_rcPre;
		IUIWindow* m_pWin;
		vector<WNDDATA> m_XWnds;
	};

#ifdef SURPORT_DWM
	static  CDwmProxy		 g_DwmProxy;
#endif
	static  IUIBase*		 g_pFocus = NULL;
	static  IUIBase*	     g_pCapture = NULL;
	static  IUIBase*         g_pMouseIn = NULL;
	static	map<HWND, VIEWHOOK*> *g_pMapXWnds;
	static  CFontMgr*        g_pFontMgr = NULL;
	static  bool			 g_Inputing = false;
	UIDEFAULTSKINFN			 g_defaultSkinFn = NULL;
	DLLAPI(BOOL) InitUIPlus( UIDEFAULTSKINFN defaultSkin /*= NULL*/, DWORD dwReserved /*= 0 */)
	{
		g_defaultSkinFn = defaultSkin;
		if ( g_pMapXWnds )
		{
			return TRUE;
		}
		try
		{
			g_pMapXWnds = new map<HWND, VIEWHOOK*>;
			g_pFontMgr = new CFontMgr;
		}
		catch( ... )
		{
			return FALSE;
		}
		return TRUE;
	}

	DLLAPI(void) UninitUIPlus(  )
	{
		if ( g_pMapXWnds )
		{
			while( g_pMapXWnds->size() )
			{
				UnregisterUIWnd( g_pMapXWnds->begin()->second->m_XWnds.begin()->pUI );
			}
			delete g_pMapXWnds;
			g_pMapXWnds = NULL;
		}
		if ( g_pFontMgr )
		{
			delete g_pFontMgr;
			g_pFontMgr = NULL;
		}
	}

	void Paint( HDC hDC, VIEWHOOK* pXwnds, IUIWindow* pWin )
	{
		RECT rcWnd = {0};
		if ( hDC && pWin )
		{
			RECT rcClip;
			GetClipBox( hDC, &rcClip );
			if ( RECT_ISEMPTY(rcClip) )
			{
				return;
			}
			pWin->GetClient( &rcWnd );
			CPTDC bufferDC;
			bufferDC.GetDC( hDC );
			HBITMAP hBufferBmp = ::CreateCompatibleBitmap( hDC, RECT_W( rcWnd ), RECT_H( rcWnd ) );
			HBITMAP hOldBmp = (HBITMAP)SelectObject( bufferDC, hBufferBmp );

			POINT ptOrg = {0};
			IPainter* painter = UICreateDCPainter( bufferDC );
			painter->DrawImage( pXwnds->pImageBkGnd, &rcClip, &rcClip );
			UIDestroyPainter( painter );
			HRGN hClip = NULL;
			for ( vector<WNDDATA>::reverse_iterator _lab = pXwnds->m_XWnds.rbegin();
				_lab != pXwnds->m_XWnds.rend(); ++_lab )
			{					
				(*_lab).pUI->GetRect( rcWnd );
				if( (*_lab).pUI->IsVisable()
					&& RectVisible( hDC, &rcWnd) )
				{
					RECT rcClipTmp;
					::IntersectRect( &rcClipTmp, &rcClip, &rcWnd );
					if ( RECT_ISEMPTY(rcClipTmp) )
					{
						continue;
					}
					hClip = CreateRectRgn( rcClipTmp.left, rcClipTmp.top, rcClipTmp.right, rcClipTmp.bottom );
					SelectClipRgn( bufferDC, hClip );
					DeleteObject( hClip );

 					painter = UICreateDCPainter( bufferDC );
 					RECT rcBk = rcClipTmp;
 					RECT_TO( rcBk, rcBk.left-rcWnd.left, rcBk.top-rcWnd.top );
 					painter->DrawImage( _lab->pImageBkGnd, &rcClipTmp, &rcBk );
 					UIDestroyPainter(painter);

					POINT viewOrg;
					SetWindowOrgEx( bufferDC, rcWnd.left, rcWnd.top+(*_lab).pUI->GetScrollPos(VERT), &ptOrg );
					SetViewportOrgEx( bufferDC, rcWnd.left, rcWnd.top, &viewOrg );
					IPainter* painter = UICreateDCPainter( bufferDC );
					if ( painter )
					{
						(*_lab).pUI->OnDraw( painter );	
					}
					UIDestroyPainter( painter );
					SetViewportOrgEx( bufferDC, viewOrg.x, viewOrg.y, &viewOrg );
					SetWindowOrgEx( bufferDC, ptOrg.x, ptOrg.y, NULL );				
				
					painter = UICreateDCPainter( bufferDC );
					if ( painter )
					{
						(*_lab).pUI->DrawScroll( painter );
					}
					UIDestroyPainter( painter );
					SelectClipRgn( bufferDC, NULL );
				}
			}
			StretchBlt( hDC, rcClip.left, rcClip.top, RECT_W( rcClip ), RECT_H( rcClip ),  \
				bufferDC, rcClip.left, rcClip.top, RECT_W( rcClip ), RECT_H( rcClip ), SRCCOPY );
			SelectObject( hDC, hOldBmp );
			DeleteObject( hBufferBmp );
		}
	}

	void DrawBorder( VIEWHOOK* pXwnds, IUIWindow* pWin, RECT& rcClient, IPainter* painter )
	{
		HPath path;
		HPath pathInner;
		do 
		{
			if ( pWin->GetRgn() )
			{
				DWORD dwCount = GetRegionData( pWin->GetRgn() , 0 , NULL );
				if ( dwCount > 0 )
				{
					LPRGNDATA pRgnData =(LPRGNDATA) malloc( dwCount );
					GetRegionData( pWin->GetRgn(), dwCount, pRgnData );
					if ( pRgnData->rdh.nCount )
					{
						int nPoints = 4*pRgnData->rdh.nCount;
						Point* pPoints = (Point*)malloc( nPoints*sizeof(Point) );
						Point* pInnerPoints = (Point*)malloc( nPoints*sizeof(Point) );
						RECT* pRgnRects = (RECT*)pRgnData->Buffer;
						int nStart = 0;
						for ( DWORD _i = 0; _i < pRgnData->rdh.nCount; ++_i )
						{
							nStart = _i*2;
							pPoints[nStart].X = pRgnRects[_i].left;
							pPoints[nStart].Y = pRgnRects[_i].top;
							pPoints[nPoints-nStart-1].X = pRgnRects[_i].right-1;
							pPoints[nPoints-nStart-1].Y = pRgnRects[_i].top;

							pInnerPoints[nStart].X = pRgnRects[_i].left+1;
							pInnerPoints[nStart].Y = pRgnRects[_i].top+1;
							pInnerPoints[nPoints-nStart-1].X = pRgnRects[_i].right-2;
							pInnerPoints[nPoints-nStart-1].Y = pRgnRects[_i].top+1;

							++nStart;
							pPoints[nStart].X = pRgnRects[_i].left;
							pPoints[nStart].Y = pRgnRects[_i].bottom-1;
							pPoints[nPoints-nStart-1].X = pRgnRects[_i].right-1;
							pPoints[nPoints-nStart-1].Y = pRgnRects[_i].bottom-1;

							pInnerPoints[nStart].X = pRgnRects[_i].left+1;
							pInnerPoints[nStart].Y = pRgnRects[_i].bottom-2;
							pInnerPoints[nPoints-nStart-1].X = pRgnRects[_i].right-2;
							pInnerPoints[nPoints-nStart-1].Y = pRgnRects[_i].bottom-2;
						}
						path = UICreatePolyPath( pPoints, nPoints );
						pathInner = UICreatePolyPath( pInnerPoints, nPoints );
						free(pRgnData);		
						free( pPoints );
						free( pInnerPoints );
						break;
					}
					free(pRgnData);	
				}
			}
			Rect rect;
			rect.X = rcClient.left;
			rect.Y = rcClient.top;
			rect.Width = RECT_W(rcClient)-1;
			rect.Height = RECT_H(rcClient)-1;
			path = UICreatePath( &rect );
			++rect.X;
			++rect.Y;
			rect.Width-=2;
			rect.Height-=2;
			pathInner = UICreatePath( &rect );

		} while ( 0 );
		ARGB clrOld = painter->SetPenColor( Color(150, 0, 0, 0).GetValue() );
 		painter->DrawPath( path );
		painter->SetPenColor( Color(60, 255, 255, 255).GetValue() );
		painter->DrawPath( pathInner );
		painter->SetPenColor( clrOld );
		UIDestroyPath( path );
		UIDestroyPath( pathInner );
	}

// 	void UpdateUIBkGnd( HDC hDC, RECT& rcClient, VIEWHOOK* pXwnds, IUIBase* pUI  )
// 	{
// 		CPTDC bkGndDC;
// 		bkGndDC.GetDC( hDC );
// 		HBITMAP hBkGnd = CreateCompatibleBitmap( hDC, RECT_W( rcClient ), RECT_H( rcClient )  );
// 		HBITMAP hOldGnd = (HBITMAP)SelectObject( bkGndDC, hBkGnd );
// 		BitBlt( bkGndDC, 0, 0, RECT_W( rcClient ), RECT_H( rcClient ),pXwnds->m_bkGndDC, 0, 0, SRCCOPY  );
// 		RECT rect;
// 		RECT rcUI;
// 		RECT rcTmp;
// 		for ( vector<WNDDATA>::reverse_iterator _lab = pXwnds->m_XWnds.rbegin();
// 			_lab != pXwnds->m_XWnds.rend(); ++_lab )
// 		{	
// 			(*_lab).pUI->GetRect(rect);
// 			if( (*_lab).pUI->IsVisable() )
// 			{
// 				if( NULL != pUI
// 					&& pUI != (*_lab).pUI )
// 				{
// 					pUI->GetRect(rcUI);
// 					IntersectRect( &rcTmp, &rcUI, &rect );
// 					if( RECT_ISEMPTY( rcTmp ) )
// 					{
// 						continue;
// 					}
// 				}
// 				IMG* pBkGnd = (*_lab).pUI->GetBkGnd();
// 				if( !pBkGnd )
// 				{
// 					if( NULL != pUI
// 						&& (*_lab).pUI == pUI )
// 					{
// 						break;
// 					}
// 					continue;
// 				}
// 				if ( ((*_lab).m_bkGndDC 
// 					&& NULL == pUI )
// 					|| ( NULL != pUI
// 					&& (*_lab).pUI == pUI))
// 				{
// 					::SelectObject( (*_lab).m_bkGndDC, (*_lab).m_hOldGnd );
// 					DeleteObject( (*_lab).m_hBkGnd );
// 					(*_lab).m_hBkGnd  = NULL;
// 				}
// 				if( NULL != pBkGnd->hImage
// 					|| X_TRANSPARENT != pBkGnd->imageAvgClr )
// 				{
// 					if ( !(*_lab).m_bkGndDC )
// 					{
// 						(*_lab).m_bkGndDC.GetDC( bkGndDC );
// 					}
// 					if( NULL == (*_lab).m_hBkGnd )
// 					{
// 						(*_lab).m_hBkGnd = CreateCompatibleBitmap( bkGndDC, RECT_W( rect ), RECT_H( rect ) );
// 						(*_lab).m_hOldGnd = (HBITMAP)SelectObject( (*_lab).m_bkGndDC, (*_lab).m_hBkGnd );
// 					}
// 					IPainter* painter = UICreateDCPainter( bkGndDC );
// 					if ( !painter )
// 					{
// 						return;
// 					}
// 					if( pBkGnd->hImage )
// 					{
// 						RECT rcImage;
// 						rcImage.left = 0;
// 						rcImage.top  = 0;
// 						rcImage.right = rcImage.left+pBkGnd->hImage->GetWidth();
// 						rcImage.bottom = rcImage.top + pBkGnd->hImage->GetHeight();
// 						painter->DrawImage( pBkGnd->hImage, &rect, &rcImage );
// 					}
// 					else
// 					{
// 						HBrush hBrush = UICreateSolidBrush( pBkGnd->imageAvgClr );
// 						if ( hBrush )
// 						{
// 							HBrush hOldBrush = painter->SetBrush( hBrush );
// 							painter->FillRectangle( rect.left, rect.top, RECT_W(rect), RECT_H(rect) );
// 							painter->SetBrush( hOldBrush );
// 							UIDestroyBrush( hBrush );
// 						}
// 					} 
// 					StretchBlt( (*_lab).m_bkGndDC, 0, 0,RECT_W( rect ), RECT_H( rect ),\
// 						bkGndDC, rect.left, rect.top, RECT_W( rect ), RECT_H( rect ), SRCCOPY );
// 					if( _lab + 1 != pXwnds->m_XWnds.rend() )
// 					{
// 						POINT ptOrg = {0, 0};
// 						SetWindowOrgEx( bkGndDC, rect.left, (*_lab).pUI->GetScrollPos(VERT)+ rect.top, &ptOrg );
// // 						if ( painter )
// // 						{
// 							(*_lab).pUI->OnDraw( painter );	
// /*						}*/
// 						//UIDestroyPainter( painter );
// 						SetWindowOrgEx( bkGndDC, ptOrg.x, ptOrg.y, NULL );
// 						SetWindowOrgEx( bkGndDC, rect.left,rect.top, &ptOrg );
// 						//painter = UICreateDCPainter( bkGndDC );
// // 						if ( painter )
// // 						{
// 							(*_lab).pUI->DrawScroll( painter );
// // 						}
// // 						UIDestroyPainter( painter );
// 						SetWindowOrgEx( bkGndDC, ptOrg.x, ptOrg.y, NULL );
// 					}
// 					UIDestroyPainter( painter );
// 				}
// 			}	
// // 			if( NULL != pUI
// // 				&& (*_lab).pUI == pUI )
// // 			{
// // 				break;
// // 			}
// 		}
// 		::SelectObject( bkGndDC, hOldGnd );
// 		::DeleteObject( hBkGnd );
// 	}

	//******************************************************************************
	// 方    法: UIDefWindowProc
	// 访问权限: export
	// 参    数: HWND    窗口句柄
	// 参    数: message 消息ID
	// 参    数: wParam 消息参数
	// 参    数: lParam 消息参数
	// 返 回 值: HRESULT S_OK 初始化成功，S_FALSE 初始化失败
	// 作    者: superlive/wangshuhe
	// 描    述: 初始化插件程序环境
	//******************************************************************************
	LRESULT UIDefWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		if ( NULL == g_pMapXWnds  )
		{
			return ::DefWindowProc( hWnd, message, wParam, lParam );
		}
		static long time1 = 0;
		static long time2 = 0;
		static RECT rcWnd;
		VIEWHOOK* pXwnds = NULL;
		map<HWND, VIEWHOOK*>::iterator _itFound = g_pMapXWnds->find( hWnd );
		if( _itFound != g_pMapXWnds->end() )
		{
			pXwnds = (VIEWHOOK*)_itFound->second;
		}
		if( NULL == pXwnds )
		{
			return ::DefWindowProc( hWnd, message, wParam, lParam );
		}
		IUIWindow* pWin = pXwnds->m_pWin;
		if ( pWin && pXwnds && !pXwnds->m_XWnds.empty() )
		{
			switch( message )
			{
			case WM_ERASEBKGND:
				{
					for ( vector<WNDDATA>::reverse_iterator _lab = pXwnds->m_XWnds.rbegin();
						_lab != pXwnds->m_XWnds.rend(); ++_lab )
					{					
						if( (*_lab).pUI->IsVisable() )
						{
							return TRUE;
						}
					}
				}
				break;
			case WM_PAINT:
				{
					PAINTSTRUCT ps;
					HDC hDC = ::BeginPaint( hWnd, &ps );
					Paint( hDC, pXwnds, pWin );
					::EndPaint( hWnd, &ps );
					return TRUE;
				}
				break;
			case WM_NCPAINT:
				{
					return S_OK;
				}
				break;
			case WM_SIZE:
				{
					if ( SIZE_RESTORED == wParam
						|| SIZE_MAXIMIZED == wParam
						|| SIZE_MAXSHOW == wParam )
					{
						LRESULT lret = CallWindowProc( pXwnds->OrgProc, hWnd, message, wParam, lParam );
						UpdateBkGnd( hWnd );
						return lret;
					}
					break;
				}
			case WM_MOUSEMOVE:
				{
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof( tme );
					tme.hwndTrack = hWnd;
					tme.dwFlags = TME_LEAVE;
					tme.dwHoverTime = 2;
					_TrackMouseEvent( &tme );
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					if ( g_pCapture )
					{
						point.x += g_pCapture->GetScrollPos( HORZ );
						point.y += g_pCapture->GetScrollPos( VERT );
						g_pCapture->MouseMoving( point );
						break;
					}

					IUIBase* pMouseIn = NULL;
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							(*_lab).pUI->MouseMoving( point );
							pMouseIn = (*_lab).pUI;
							break;
						}
					}
					if ( g_pMouseIn != pMouseIn )
					{
						if ( g_pMouseIn )
						{
							g_pMouseIn->MouseLeave();
						}
						g_pMouseIn = pMouseIn;
						if ( g_pMouseIn )
						{
							g_pMouseIn->MouseIn();
						}
					}
				}
				break;
			case WM_SETCURSOR:
				{
					if ( HTCLIENT == GET_X_LPARAM(lParam) )
					{
						POINT point;
						GetCursorPos( &point );
						::ScreenToClient( hWnd, &point );
						for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
							_lab != pXwnds->m_XWnds.end(); ++_lab )
						{
							(*_lab).pUI->GetRect( rcWnd );
							if( (*_lab).pUI->IsVisable()
								&& ::PtInRect( &rcWnd, point ))
							{
								if( (*_lab).pUI->CursorIn( point ) )
								{
									return TRUE;
								}
							}
						}
					}
				}
				break;
			case WM_KEYDOWN:
				{
					if ( g_pFocus )
					{
						g_pFocus->KeyDown( wParam, lParam );
					}
				}
				break;
			case WM_KEYUP:
				{
					if ( g_pFocus )
					{
						g_pFocus->KeyUp( wParam, lParam );
					}
				}
				break;
			case WM_CHAR:
				{
					if ( g_pFocus )
					{
						if( g_pFocus->OnChar( wParam, lParam ) )
						{
							return S_OK;
						}
					}
				}
				break;
			case WM_LBUTTONDOWN:
				{
					if ( WS_CAPTION && pWin->GetStyle() )
					{
						SetFocus( hWnd );
					}
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					if ( g_pFocus )
					{
						RECT rcFocus;
						g_pFocus->GetRect( rcFocus );
						if( !::PtInRect( &rcFocus, point ) )
						{
							UISetFocus(NULL);
						}
					}
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseLBDown( point ) )
							{
								if ( NULL == g_pFocus )
								{
									UISetFocus((*_lab).pUI);
								}
								return TRUE;
							}
							if ( NULL == g_pFocus )
							{
								UISetFocus((*_lab).pUI);
							}
							break;
						}
					}
				}
				break;
			case WM_NCLBUTTONDOWN:
				{
					if ( WS_CAPTION && pWin->GetStyle() )
					{
						SetFocus( hWnd );
					}
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					if ( g_pFocus )
					{
						RECT rcFocus;
						g_pFocus->GetRect( rcFocus );
						if( !::PtInRect( &rcFocus, point ) )
						{
							UISetFocus(NULL);
						}
					}
				}
				break;
			case WM_RBUTTONDOWN:
				{
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					if ( g_pFocus )
					{
						RECT rcFocus;
						g_pFocus->GetRect( rcFocus );
						if( !::PtInRect( &rcFocus, point ) )
						{
							UISetFocus(NULL);
						}
					}				
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseRBDown( point ) )
							{
								return TRUE;
							}
							break;
						}
					}
				}
				break;
			case WM_RBUTTONUP:
				{
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseRBUp( point ) )
							{
								return TRUE;
							}
							break;
						}
					}
				}
				break;
			case WM_LBUTTONUP:
				{
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					if ( g_pCapture )
					{
						point.x += g_pCapture->GetScrollPos( HORZ );
						point.y += g_pCapture->GetScrollPos( VERT );
						g_pCapture->MouseLBUp( point );
						g_pCapture = NULL;
					}
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseLBUp( point ) )
							{
								return TRUE;
							}
							break;
						}
					}
				}
				break;
			case WM_LBUTTONDBLCLK:
				{
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseLDBClick( point ) )
							{
								return TRUE;
							}
							break;
						}
					}
				}
				break;
			case WM_RBUTTONDBLCLK:
				{
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseRDBClick( point ) )
							{
								return TRUE;
							}
							break;
						}
					}
				}
				break;
			case WM_MOUSEWHEEL:
				{
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					ScreenToClient( hWnd, &point );
					if ( g_pFocus )
					{
						if ( g_pFocus->MouseWheel( GET_WHEEL_DELTA_WPARAM(wParam) ) )
						{
							return TRUE;
						}
					}
					for ( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
						_lab != pXwnds->m_XWnds.end(); ++_lab )
					{
						(*_lab).pUI->GetRect( rcWnd );
						if( (*_lab).pUI->IsVisable()
							&& ::PtInRect( &rcWnd, point ))
						{
							if( (*_lab).pUI->MouseWheel( GET_WHEEL_DELTA_WPARAM(wParam) ) )
							{
								return TRUE;
							}
							break;
						}
					}
				}
				break;
			case WM_MOUSELEAVE:
				{
					UISetMouseIn( NULL );
				}
				break;
			case WM_KILLFOCUS:
				{
					HWND hNew = (HWND)wParam;
					if( IsWindow( hNew )
						&& ::GetParent(hNew) == hWnd )
					{
						break;
					}
					if ( pSingleMenu )
					{
						pSingleMenu->EndMenu(  );
					}
					pSingleMenu = NULL;
					UISetFocus( NULL );
				}
				break;
			case WM_ACTIVATE:
				{
					if( WA_INACTIVE == wParam )
					{
						if ( GetParent((HWND)lParam) != hWnd )
						{
							UISetFocus( NULL );
						}
					}
				}
				break;
			case WM_ACTIVATEAPP:
				{
					if ( pSingleMenu && FALSE == wParam )
					{
						pSingleMenu->EndMenu(  );
					}
					pSingleMenu = NULL;
				}
			default:
				;		
			}
			return CallWindowProc( pXwnds->OrgProc, hWnd, message, wParam, lParam );
		}
		return S_OK;
	}


	//******************************************************************************
	// 方    法: UpdateBkGnd
	// 访问权限: export
	// 参    数: N/A
	// 返 回 值: N/A
	// 作    者: superlive/wangshuhe
	// 描    述: 反初始化插件程序环境
	//******************************************************************************
#ifdef SURPORT_EXT
	DLLAPI( LRESULT ) UpdateBkGnd( HWND hWnd, IUIBase* pUI )
#else
	DLLAPI( LRESULT ) UpdateBkGnd( HWND hWnd )
#endif
	{
		if ( NULL == g_pMapXWnds  )
		{
			return S_OK;
		}
		VIEWHOOK* pXwnds = NULL;
		map<HWND, VIEWHOOK*>::iterator _itFound = g_pMapXWnds->find( hWnd );
		if( _itFound != g_pMapXWnds->end() )
		{
			pXwnds = (VIEWHOOK*)_itFound->second;
		}
		if( NULL == pXwnds )
		{
			return S_OK;
		}
		bool bnewed = false;
		IUIWindow* pWin = pXwnds->m_pWin;
		if ( pXwnds && pWin )
		{
			RECT rcClient;
			pWin->GetRect( &rcClient );
			if( RECT_ISEMPTY(rcClient) )
			{
				return S_OK;
			}
			if ( pXwnds->pImageBkGnd 
				&& ( pXwnds->pImageBkGnd->GetWidth()!= RECT_W( rcClient )
				|| pXwnds->pImageBkGnd->GetHeight() != RECT_H( rcClient )
				)
				)
			{
				delete ((ImageImpl*)pXwnds->pImageBkGnd);
				pXwnds->pImageBkGnd = NULL;
			}
			if ( !pXwnds->pImageBkGnd  )
			{
				try
				{
					pXwnds->pImageBkGnd = new ImageImpl;
					pXwnds->pImageBkGnd->SetBitmap( new Bitmap( RECT_W(rcClient), RECT_H(rcClient) ) );
				}
				catch( ... )
				{
					delete ((ImageImpl*)pXwnds->pImageBuffer);
					pXwnds->pImageBuffer = NULL;
					return S_FALSE;
				}
				bnewed = true;
			}
			 
			IPainter* painter = UICreateImagePainter( pXwnds->pImageBkGnd, true );
			if( !painter )
			{
				return S_FALSE;
			}
			RECT_TO( rcClient, 0, 0  );
			//刷新视窗背景
			IMG* pWndBkGnd = pWin->GetBkGnd();
			if( pWndBkGnd )
			{
				if ( X_TRANSPARENT != pWndBkGnd->imageAvgClr )
				{
					ARGB clrOld = painter->SetBrushColor( pWndBkGnd->imageAvgClr );
					painter->FillRectangle( 0, 0, RECT_W( rcClient ), RECT_H( rcClient ) );
					painter->SetBrushColor( clrOld );
				}
				if( NULL != pWndBkGnd->hImage )
				{
					ImageInfo imageInfo = {0};
					pWndBkGnd->hImage->GetImageInfo( &imageInfo );
					if ( !(pWin->GetBkGnd()->rcValid.right < pWin->GetBkGnd()->rcValid.left
						|| pWin->GetBkGnd()->rcValid.bottom < pWin->GetBkGnd()->rcValid.top) )
					{
						RECT rcTmp;
						if ( RECT_ISEMPTY(pWin->GetBkGnd()->rcValid) )
						{
							RECT_S( rcTmp, 0, 0, imageInfo.Width, imageInfo.Height );
							painter->DrawImage( pWndBkGnd->hImage, &rcTmp, &rcTmp );
						}
						else
						{
							RECT _client = rcClient;
							RECT rcImage = pWin->GetBkGnd()->rcValid;
							if( RECT_W( rcImage ) > RECT_W( rcClient ) )
							{
								rcImage.right = rcImage.left + RECT_W( rcClient );
							}
							else
							{
								_client.right = RECT_W( rcImage );
							}
							if( RECT_H( rcImage ) > RECT_H( rcClient ) )
							{
								rcImage.bottom =rcImage.top + RECT_H( rcClient );
							}
							else
							{
								_client.bottom = RECT_H( rcImage );
							}
							painter->DrawImage( pWndBkGnd->hImage, &_client, &rcImage );
						}
					}
				}
				if ( pWin->GetStyle() & WS_BORDER )
				{
					DrawBorder( pXwnds, pWin, rcClient, painter );
				}
			}
			UIDestroyPainter( painter );
			painter = NULL;
			if ( bnewed )
			{
				if ( pXwnds->pImageBuffer )
				{
					delete ((ImageImpl*)pXwnds->pImageBuffer);
					pXwnds->pImageBuffer = NULL;
				}
				pXwnds->pImageBuffer = CloneImage( pXwnds->pImageBkGnd->GetBitmap() );
			}
			
			//刷新子视窗背景
			RECT rect;
			for ( vector<WNDDATA>::reverse_iterator _lab = pXwnds->m_XWnds.rbegin();
				_lab != pXwnds->m_XWnds.rend(); ++_lab )
			{	
				if( (*_lab).pUI->IsVisable() )
				{
					IMG* pBkGnd = (*_lab).pUI->GetBkGnd();
					if( !pBkGnd )
					{
						if ( _lab->pImageBkGnd )
						{
							delete (ImageImpl*)_lab->pImageBkGnd;
							_lab->pImageBkGnd = NULL;
						}
						if ( _lab->pImageBuffer )
						{
							delete (ImageImpl*)_lab->pImageBuffer;
							_lab->pImageBuffer = NULL;
						}
						continue;
					}
					(*_lab).pUI->GetRect(rect);
					if( NULL != pBkGnd->hImage
						|| X_TRANSPARENT != pBkGnd->imageAvgClr )
					{
						if ( _lab->pImageBkGnd
							&& ( _lab->pImageBkGnd->GetWidth()!=RECT_W( rect )
							|| _lab->pImageBkGnd->GetHeight() != RECT_H( rect )
							))
						{
							delete (ImageImpl*)_lab->pImageBkGnd;
							_lab->pImageBkGnd = NULL;
						}
						try
						{
							if ( !_lab->pImageBkGnd )
							{
								_lab->pImageBkGnd = new ImageImpl;
								_lab->pImageBkGnd->SetBitmap( new Bitmap( RECT_W(rect), RECT_H(rect) ) );
							}
						}
						catch( ... )
						{
							delete (ImageImpl*)_lab->pImageBkGnd;
							_lab->pImageBkGnd = NULL;
							return S_FALSE;
						}
						painter = UICreateImagePainter( _lab->pImageBkGnd, true );
						if( !painter )
						{
							return S_FALSE;
						}
						RECT_TO( rect, 0, 0 );
						if( pBkGnd->hImage )
						{
							ImageInfo imageInfo = {0};
							pBkGnd->hImage->GetImageInfo( &imageInfo );
							RECT rcImage;
							rcImage.left = 0;
							rcImage.top  = 0;
							rcImage.right = rcImage.left+imageInfo.Width;
							rcImage.bottom = rcImage.top + imageInfo.Height;
							painter->DrawImage( pBkGnd->hImage, &rect, &rcImage );
						}
						else
						{
							ARGB clrOld = painter->SetBrushColor( pBkGnd->imageAvgClr );
							painter->FillRectangle( rect.left, rect.top, RECT_W( rect ), RECT_H( rect ) );
							painter->SetBrushColor( clrOld );
						}
						UIDestroyPainter( painter );
						painter = NULL;
						_lab->pImageBuffer = CloneImage( _lab->pImageBkGnd->GetBitmap() );
					}
				}	
			}
		}
		return S_OK;
	}

	//******************************************************************************
	// 方    法: RegisterUIWnd
	// 访问权限: export
	// 参    数: nMessageId 订阅的消息ID
	// 参    数: msgProc	消息回调处理过程函数指针(参考MessageProc))
	// 返 回 值: HRESULT S_OK 订阅成功 S_FALSE 订阅失败
	// 作    者: superlive/wangshuhe
	// 描    述: 订阅消息
	//******************************************************************************
	DLLAPI(HRESULT) RegisterUIWnd( IUIBase* pXwnd, IUIBase* pInsertBefore/*=NULL*/ )
	{
		if( NULL == g_pMapXWnds
			||NULL == pXwnd 
			||(NULL!=pXwnd
			&& (NULL == pXwnd->GetUIWindow()
			||(NULL != pXwnd->GetUIWindow()
			&& NULL == pXwnd->GetUIWindow()->GetHandle()))))
		{
			return S_FALSE;
		}
		VIEWHOOK* pXWnds = NULL;
		map<HWND, VIEWHOOK*>::iterator _itFound = g_pMapXWnds->find( pXwnd->GetUIWindow()->GetHandle() );
		if( _itFound != g_pMapXWnds->end() )
		{
			pXWnds = (VIEWHOOK*)_itFound->second;
		}
		if ( NULL == pXWnds )
		{
			try
			{
				pXWnds = new VIEWHOOK;
				pXWnds->OrgProc = (WNDPROC)(LONG_PTR)SetWindowLong( pXwnd->GetUIWindow()->GetHandle(), GWL_WNDPROC, (LONG)(LONG_PTR)UIDefWindowProc );
				(*g_pMapXWnds)[pXwnd->GetUIWindow()->GetHandle()] = pXWnds;
				pXWnds->m_pWin = pXwnd->GetUIWindow();
				pXWnds->pImageBuffer = NULL;
				pXWnds->pImageBkGnd = NULL;
			}
			catch( ... )
			{
				return S_FALSE;
			}
		}

		bool bInserted = false;
		WNDDATA wndData;
		wndData.pUI = pXwnd;
		wndData.pImageBuffer = NULL;
		wndData.pImageBkGnd = NULL;
		if ( pInsertBefore 
			&& pInsertBefore != VIEW_BOTTOM )
		{
			if ( pInsertBefore == VIEW_TOP )
			{
				pXWnds->m_XWnds.insert( pXWnds->m_XWnds.begin(), wndData );
				bInserted = true;
			}
			else
			{
				if( pInsertBefore->GetUIWindow()
					&& pInsertBefore->GetUIWindow()->GetHandle()
					&& pInsertBefore->GetUIWindow()->GetHandle() == pXwnd->GetUIWindow()->GetHandle() )
				{
					for( vector<WNDDATA>::iterator _lab = pXWnds->m_XWnds.begin();
						_lab != pXWnds->m_XWnds.end(); ++_lab )
					{
						if( _lab->pUI == pInsertBefore )
						{
							pXWnds->m_XWnds.insert( _lab, wndData );
							bInserted = true;
							break;
						}
					}
				}
			}
		}
		if ( !bInserted )
		{
			pXWnds->m_XWnds.push_back( wndData );
		}
		return S_OK;
	}

	//******************************************************************************
	// 方    法: UnregisterUIWnd
	// 访问权限: export
	// 参    数: nMessageId 退订的消息ID
	// 参    数: msgProc	消息回调处理过程函数指针(参考MessageProc))
	// 返 回 值: HRESULT S_OK 退订成功 S_FALSE 退订失败
	// 作    者: superlive/wangshuhe
	// 描    述: 退订消息
	//******************************************************************************
	DLLAPI(HRESULT) UnregisterUIWnd( IUIBase* pXwnd  )
	{
		if ( NULL == g_pMapXWnds  )
		{
			return S_OK;
		}
		if( NULL == pXwnd 
			||(NULL!=pXwnd
			&& (NULL == pXwnd->GetUIWindow()
			||(NULL != pXwnd->GetUIWindow()
			&& NULL == pXwnd->GetUIWindow()->GetHandle()))))
		{
			return S_OK;
		}
		VIEWHOOK* pXwnds = NULL;
		map<HWND, VIEWHOOK*>::iterator _itFound = g_pMapXWnds->find( pXwnd->GetUIWindow()->GetHandle() );
		if( _itFound != g_pMapXWnds->end() )
		{
			pXwnds = (VIEWHOOK*)_itFound->second;
		}
		if( NULL == pXwnds )
		{
			return FALSE;
		}
		if ( pXwnds )
		{
			for( vector<WNDDATA>::iterator _lab = pXwnds->m_XWnds.begin();
				_lab != pXwnds->m_XWnds.end(); ++_lab )
			{
				if( _lab->pUI == pXwnd )
				{
					DestroyImage( _lab->pImageBuffer );
					DestroyImage( _lab->pImageBkGnd );
					pXwnds->m_XWnds.erase( _lab );
					break;
				}
			}
		}
		if ( pXwnds->m_XWnds.empty() )
		{
			g_pMapXWnds->erase( pXwnd->GetUIWindow()->GetHandle() );
			DestroyImage( pXwnds->pImageBuffer );
			DestroyImage( pXwnds->pImageBkGnd );
			delete pXwnds;
			pXwnds = NULL;
		}
		return S_OK;
	}

	DLLAPI(BOOL)  InvalidRect( HWND hWnd, LPCRECT lpRect, int nMode /*= 0*/  )
	{
		nMode;
		if ( IsWindow(hWnd) )
		{
			return InvalidateRect( hWnd, lpRect, FALSE  );
		}
		return FALSE;
	}

	DLLAPI(IUIBase*) UISetFocus( IUIBase* pUI )
	{
		if ( pUI == g_pFocus )
		{
			return g_pFocus;
		}
		IUIBase* pOld = g_pFocus;
		g_pFocus = NULL;
		g_pFocus = pUI;
		if( pOld 
			&& !UIIsFocusIn( pOld ) )
		{	
			pOld->FocusLeave();
		}
		return pOld;
	}

	DLLAPI(bool) UIIsFocusIn( IUIBase* pUI )
	{
		if ( pUI )
		{
			IUIBase* pFocus = g_pFocus;
			while( pFocus )
			{
				if ( pUI == pFocus )
				{
					return true;
				}
				pFocus = pFocus->GetParent();
			}
		}
		return false;
	}

	DLLAPI(IUIBase*) UIGetFocus( )
	{
		return g_pFocus;
	}

	DLLAPI(IUIBase*) UISetMouseIn( IUIBase* pUI )
	{
		IUIBase* pOld = g_pMouseIn;
		g_pMouseIn = pUI;
		if( g_pMouseIn )
		{
			g_pMouseIn->MouseIn();
		}
		if( pOld )
		{
			pOld->MouseLeave();
		}
		return pOld;
	}

	DLLAPI(IUIBase*) UIGetMouseIn(  )
	{
		return g_pMouseIn;
	}

	DLLAPI(IUIBase*) UIGetWndMouseIn( )
	{
		return g_pMouseIn;
	}

	DLLAPI(void) UISetCapture( IUIBase* pCapture )
	{
		if( pCapture
			&& pCapture->GetUIWindow() 
			&& pCapture->GetUIWindow()->GetHandle() )
		{
			SetCapture( pCapture->GetUIWindow()->GetHandle() );
			g_pCapture = pCapture;
		}
		return ;
	}

	DLLAPI(void) UIReleaseCapture(  )
	{
		ReleaseCapture(  );
		g_pCapture = NULL;
		return ;
	}

	DLLAPI(IUIBase*) UIGetCapture()
	{
		return g_pCapture;
	}

	DLLAPI(void)	 UIActiveProcess(  )
	{
		POINT point;
		GetCursorPos( &point );
		HWND hWndFromPoint = ::WindowFromPoint( point );
		if ( hWndFromPoint )
		{
			DWORD dwMouseInProcessId;
			::GetWindowThreadProcessId( hWndFromPoint, &dwMouseInProcessId );
			if( GetCurrentProcessId() == dwMouseInProcessId )
			{
				return;
			}
		}
		WNDCLASS wndcls;
		if (!(::GetClassInfo((HINSTANCE)::GetModuleHandle(NULL), _T("UI_ACTIVE"), &wndcls)))//| CS_HREDRAW | CS_DROPSHADOW |CS_VREDRAW
		{
			// otherwise we need to register a new class
			wndcls.style            = CS_DBLCLKS  |  CS_SAVEBITS;
			wndcls.lpfnWndProc      = DefWindowProc;
			wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
			wndcls.hInstance        = (HINSTANCE)::GetModuleHandle(NULL);
			wndcls.hIcon            = NULL;
			wndcls.hCursor          = LoadCursor( NULL, IDC_ARROW );
			wndcls.hbrBackground    = (HBRUSH)GetStockObject( WHITE_BRUSH );
			wndcls.lpszMenuName     = NULL;
			wndcls.lpszClassName    = _T("UI_ACTIVE");
			RegisterClass(&wndcls);
		}

		HWND hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, _T("UI_ACTIVE"), _T(""),  \
			WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, 0, 0, hWndFromPoint, NULL,\
			(HINSTANCE)::GetModuleHandle(NULL), NULL );
		if ( NULL != hWnd )
		{
			DestroyWindow( hWnd );
		}
		return;
	}

#ifdef SURPORT_DWM
	DLLAPI( LRESULT ) EnableBlurWindow( HWND hWnd, bool bEnable )
	{
		return g_DwmProxy.EnableBlurWindow( hWnd, bEnable );
	}

	DLLAPI( LRESULT ) EnableComposition( bool bEnable )
	{
		return g_DwmProxy.EnableComposition( bEnable );
	}
#endif

	DLLAPI(UINT) UITrackPopupMenu( MENUDATA* pMenuData, int nCount, int x, int y, int fuFlags, ACCEL* pAccels /*= NULL*/, int nAccelCount /*= 0*/, bool bLayered /*= false*/ )
	{
		CUIPopMenu::m_bLayered = bLayered;
		CUIPopMenu menu;
		UINT nRet = menu.TrackPopupMenu( pMenuData, nCount, x, y, fuFlags, pAccels, nAccelCount );
		return nRet;
	}

	DLLAPI(BOOL) UISetMenuAttribute( const MENUATTRIBUTE* pAttrib )
	{
		if ( pAttrib )
		{
			if ( pAttrib->uMask&MA_BKGND )
			{
				CUIPopMenu::m_pBkGnd = pAttrib->pBkGnd;
			}
			if ( pAttrib->uMask&MA_BKITEM )
			{
				CUIPopMenu::m_pBkItem = pAttrib->pBkItem;
			}
			if ( pAttrib->uMask&MA_MARGIN )
			{
				CUIPopMenu::m_margin = *pAttrib->pMargin;
			}
			if ( pAttrib->uMask&MA_DRAWMASK )
			{	
				CUIPopMenu::m_uDrawMask = pAttrib->uDrawMask;
			}
			if ( pAttrib->uMask&MA_ALPHA )
			{	
				CUIPopMenu::m_uAlpha = pAttrib->uAlpha;
			}
			if ( pAttrib->uMask&MA_ICONWIDTH )
			{	
				CUIPopMenu::m_uIconWidth = pAttrib->uIconWidth;
			}
			if ( pAttrib->uMask&MA_SUBARROW )
			{	
				CUIPopMenu::m_pSubArrow = pAttrib->pSubArrow;
			}
			if ( pAttrib->uMask&MA_SEPARATOR )
			{	
				CUIPopMenu::m_pSeparator = pAttrib->pSeparator;
			}
			if ( pAttrib->uMask&MA_ITEMHEIGHT )
			{	
				CUIPopMenu::m_uItemHeight = pAttrib->uItemHeight;
			}
			if ( pAttrib->uMask&MA_SUBBKGND )
			{	
				CUIPopMenu::m_pSubBkGnd = pAttrib->pSubBkGnd;
			}
			if ( pAttrib->uMask&MA_SUBMARGIN )
			{	
				CUIPopMenu::m_subMargin = *pAttrib->pSubMargin;
			}
			if ( pAttrib->uMask&MA_GRAYTEXTCLR )
			{
				CUIPopMenu::m_clrGray = pAttrib->clrGrayFont;
			}
			if ( pAttrib->uMask&MA_TEXTCLR )
			{
				CUIPopMenu::m_clrFont = pAttrib->clrFont;
			}
			if ( pAttrib->uMask&MA_FONT )
			{
				CUIPopMenu::m_font = pAttrib->hFont;
			}
			if( pAttrib->uMask&MA_ITEMOFFSET )
			{
				CUIPopMenu::m_itemOffset = pAttrib->uItemOffset;
			}

			return TRUE;
		}
		return FALSE;
	}

	//******************************************************************************
	// 方    法: UIGetDefaultFont
	// 访问权限: export
	// 参    数: NA
	// 返 回 值: 成功 返回字体句柄,否则返回NULL
	// 作    者: superlive/wangshuhe
	// 描    述: 取默认字体
	//******************************************************************************
	DLLAPI(HFONT) UIGetDefaultFont(  )
	{
		if ( g_pFontMgr )
		{
			return g_pFontMgr->GetDefault();
		}
		return NULL;
	}

	DLLAPI(void)  UISetDefalutFont( int iHeight, LPCTSTR lpszFace /*= NULL*/, int style/*=F_NORMAL */)
	{
		if ( g_pFontMgr )
		{
			g_pFontMgr->SetDefault( iHeight, lpszFace, style );
		}
	}

	//******************************************************************************
	// 方    法: UIGetFont
	// 访问权限: export
	// 参    数: iHeight 字体大小
	// 参    数: lpszFace 字体类型 为NULL值时,使用默认字体
	// 参    数: style  字体样式 参见FONT_STYLE
	// 返 回 值: 成功 返回字体句柄,否则返回NULL
	// 作    者: superlive/wangshuhe
	// 描    述: 刷新窗口区域
	//******************************************************************************
	DLLAPI(HFONT)  UIGetFont( int iHeight, LPCTSTR lpszFace /*= NULL*/, int style/*=F_NORMAL */)
	{
		if ( g_pFontMgr )
		{
			return g_pFontMgr->GetFont( iHeight, lpszFace, style );
		}
		return NULL;
	}

	DLLAPI(UIDEFAULTSKINFN) UIGetDefaultSkinFn(  )
	{
		return g_defaultSkinFn;
	}

#ifdef __cplusplus
}
#endif
