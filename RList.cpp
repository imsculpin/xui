// RList.cpp : 定义应用程序的类行为。
//

#include "Switch.h"
#include "RList.h"
#include "RListDlg.h"
#include "memcheck.h"

HIMAGE	  CRList::m_RLBmpArray[BMP_END];   //位图缓存
CRList theApp;
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

void __stdcall DefaultSkin( IUIBase*  );
BOOL CRList::InitInstance()
{
 	__super::InitInstance();
// 	MEM_ASSERT( 282 );
// 	MEM_ASSERT( 283 );
	InitUIPlus(DefaultSkin);
	UISetDefalutFont( 13, _T("宋体") );
	_LoadImages();
//  	{
//  		CRListDlg dlgDomodal;
//  		dlgDomodal.SetArc( 3, 3 );
//  		dlgDomodal.SetMin( 300, 400 );
//  		dlgDomodal.DoModal(  RECT_C(200,200,701,601), WS_DEFAULT, WS_EX_DEFAULT|WS_EX_OVERLAPPEDWINDOW );
//  	}
	//dlg = new CRListDlg;
 	dlg.SetArc( 3, 3 );
 	dlg.SetMin( 300, 400 );
 	if( dlg.Create( RECT_C(200,200,701,601), WS_DEFAULT|WS_CAPTION|WS_BORDER, WS_EX_OVERLAPPEDWINDOW, (HWND)NULL ) )
 	{
 	 	GLGetApp()->m_hMainWnd = dlg.GetHandle();
 		//dlg->EnableBlurWindow( true );
 	}
	return TRUE;
}
int CRList::ExitInstance()
{
// 	if ( dlg )
// 	{
// 		delete dlg;
// 		dlg = NULL;
// 	}
	UninitUIPlus();
	return 0;
}

BOOL CRList::_LoadImages()
{
	tstring strImagePath = _T(".\\image\\");
	{
		m_RLBmpArray[BMP_PHONE_NORMAL] = CreateImage( XT2W(strImagePath + _T("phone_1.png")) );
	}

	{
		m_RLBmpArray[BMP_PHONE_HOT] = CreateImage( XT2W(strImagePath + _T("phone_2.png")) );
	}
	{
		m_RLBmpArray[BMP_PHONE_PRESSED] = CreateImage( XT2W(strImagePath + _T("phone_3.png")) );
	}
	{
		m_RLBmpArray[BMP_SMS_NORMAL] = CreateImage(XT2W(strImagePath + _T("sms_1.png")) );
	}
	{
		m_RLBmpArray[BMP_SMS_HOT] = CreateImage(XT2W(strImagePath + _T("sms_2.png")) );
	}
	{
		m_RLBmpArray[BMP_SMS_PRESSED] = CreateImage(XT2W(strImagePath + _T("sms_3.png")) );
	}
	{
		m_RLBmpArray[BMP_SEND] = CreateImage(XT2W(strImagePath + _T("out.png")) );
	}
	{
		m_RLBmpArray[BMP_RECIEVED] = CreateImage(XT2W(strImagePath + _T("in.png")) );
	}
	{
		m_RLBmpArray[BMP_ROWHOT] = CreateImage(XT2W(strImagePath + _T("rbk_hot.png")) );
	}
	{
		m_RLBmpArray[BMP_ROWSEL] = CreateImage(XT2W(strImagePath + _T("rbk_sel.png")) );
	}
	m_RLBmpArray[BMP_OFFLINE] = CreateImage( XT2W(strImagePath + _T("lan_avatar_bg_2.png")) );
	{
		m_RLBmpArray[BMP_ONLINE] = CreateImage( XT2W(strImagePath + _T("13424467930.jpg")) );
	}
	m_RLBmpArray[BMP_L3] = CreateImage( XT2W(strImagePath + _T("smsqp_l_03.png")) );
	m_RLBmpArray[BMP_L2] = CreateImage( XT2W(strImagePath + _T("smsqp_l_02.png")) );
	m_RLBmpArray[BMP_L1] = CreateImage( XT2W(strImagePath + _T("smsqp_l_01.png")) );
	m_RLBmpArray[BMP_R1] = CreateImage( XT2W(strImagePath + _T("smsqp_r_01.png")) );
	m_RLBmpArray[BMP_R2] = CreateImage( XT2W(strImagePath + _T("smsqp_r_02.png")) );
	m_RLBmpArray[BMP_R3] = CreateImage( XT2W(strImagePath + _T("smsqp_r_03.png")) );
	m_RLBmpArray[BMP_CAR] = CreateImage(_T(".\\image\\car.png") );
	m_RLBmpArray[BMP_PAD] = CreateImage(_T(".\\image\\pad.png") );
	return TRUE;
}

void __stdcall DefaultSkin( IUIBase* pUI )
{
	switch( pUI->GetClass())
	{
	case XW_SCROLL:
		{
			tstring strImagePath = _T(".\\image\\");
			CUIScroll* pScroll = (CUIScroll*)pUI;
			Margin margin;
			margin.left = 3;
			margin.right = 3;
			margin.top = 3;
			margin.bottom =3;
			CUIButton null(false);
			null.Create( _T(""), RECT_C(0,0,0,0) );
			CUIImage  nullImag;
			nullImag.Create( _T(""), RECT_C(0,0,0,0) );
			nullImag.SetBkColor( X_TRANSPARENT );
			CUIButton thumb(false);
			HIMAGE hNormal = CreateImage( strImagePath + _T("1.png") );
			ImageInfo info1 = {0};
			hNormal->GetImageInfo( &info1 );
			thumb.Create( _T(""), RECT_C(0,0, 20, 20) );
			IMG img;
			img.hImage = CreateImage( strImagePath + _T("2.png"));
			img.margin.left= img.margin.right = img.margin.top = img.margin.bottom = 3;
			thumb.SetNormalImage( img );
			if ( pScroll )
			{
				pScroll->SetScroll( null, null, nullImag, thumb );
			}
		}
		break;
	default:
		break;
	}
	
}
