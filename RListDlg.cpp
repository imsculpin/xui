// RListDlg.cpp : 实现文件
//

#include "Switch.h"
#include "RList.h"
#include "RListDlg.h"
#include <GdiPlus.h>
#include ".\rlistdlg.h"
#include "strfun.h"
#include "hls.h"
using namespace Gdiplus;
#pragma comment( lib, "gdiplus.lib" )
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum MSGID
{
	ID_TREE = 1000,
	ID_LIST,
	ID_VLLIST,
	ID_HEAD,
	ID_NAME,
	ID_MINI,
	ID_MAX,
	ID_RESTORE,
	ID_CLOSE,
	ID_AUTOLIST,
	ID_SKIN,
	IDC_ATC_CHECKBOX,
	IDC_ALPHA,
	IDC_SHOWTAB,
	IDC_PSD,
};

// CRListDlg 对话框
HFONT CRListDlg::m_normalFont;
HIMAGE	  CRListDlg::m_RLBmpArray[CRListDlg::BMP_END];
CRListDlg::CRListDlg()
{
}

CRListDlg::~CRListDlg()
{
// 	for ( int _i = 0; _i < BMP_END; ++_i )
// 	{
// 		m_RLBmpArray[_i] = NULL;
// 	}
	m_tray.Destroy();
}
// CRListDlg 消息处理程序

BOOL CRListDlg::OnInitWnd()
{
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetAlpha( 0 );
	tstring strImagePath = _T(".\\image\\");
	RECT rcClient;
	RECT_EMPTY( rcClient );
	m_tab.Create(  rcClient, this, 1010 );
	m_list.Create(  rcClient, this, 1005 );
	m_list.EnableScroll( VERT, true );
	m_vlist.Create(  rcClient, this, 1006 );
	m_vlist.EnableScroll( VERT, true );
	m_autolist.Create(  rcClient, this, 1007 );
	m_autolist.EnableScroll( VERT, true );
	RECT view = { 0, 0, 200, 300 };
	m_comboDropView.Create(  view, NULL, 1011);
	m_comboDropView.Show( true );
	m_comboDropView.EnableScroll( VERT, true );
	m_head.Create(  rcClient, this, 1008 );
	m_tree.Create( rcClient, this, 1009 );
	m_tree.EnableScroll( VERT, true );
	m_tree.EnableDbClickExpand( false );
	m_tree.ScrollBarInClient( true );
	m_viewMark.Create( rcClient, this, 1015, VIEW_TOP );
	IMG imgViewBkGnd;
	imgViewBkGnd.imageAvgClr = Color( 50, 0, 0, 0 ).GetValue();
	m_viewMark.SetBkGnd( imgViewBkGnd );
	//m_tree.SetIndentationX( 10 );
	//像素值转换为磅值：pixel*72/96
	long ptSize = MulDiv(12,72,96);
	//磅值转换LOGFONT 的 lfHeight 值
	HDC dcScreen = ::GetDC( 0 );
	long iFontHeight = MulDiv( ptSize, GetDeviceCaps( dcScreen, LOGPIXELSY),72);
	LOGFONT lgFont;
	memset(&lgFont, 0, sizeof(LOGFONT));
	lgFont.lfHeight = -iFontHeight;
	lstrcpy(lgFont.lfFaceName, _T("Arial"));
	lgFont.lfOutPrecision = OUT_TT_PRECIS;
	lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lgFont.lfQuality = PROOF_QUALITY;
	lgFont.lfQuality = ANTIALIASED_QUALITY;
	lgFont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
	lgFont.lfWeight = FW_NORMAL;
	m_normalFont = NULL;//CreateFontIndirect( &lgFont );

	_LoadImages();
	Margin margin;
	margin.left = 3;
	margin.right = 3;
	margin.top = 3;
	margin.bottom =3;
	IMG img;
	if ( m_RLBmpArray[BMP_ROWHOT] )
	{
		img.hImage = m_RLBmpArray[BMP_ROWHOT];
		img.margin.left = img.margin.right=\
			img.margin.top=img.margin.bottom = 3;
		m_tree.SetHotNodeBkGnd( img );
		m_vlist.SetHotRowBkGnd( img );
		m_list.SetHotRowBkGnd( img );
	}
	if ( m_RLBmpArray[BMP_ROWSEL] )
	{
		img.hImage = m_RLBmpArray[BMP_ROWSEL];
		m_tree.SetSelectNodeBkGnd( img );
		m_vlist.SetSelectRowBkGnd( img );
		m_list.SetSelectRowBkGnd( img );
	}
	m_list.SetLineSpace( 0 ) ;
	m_list.SetScrollStep( 80 );
	m_list.SetRowHeight( 48 );

	m_vlist.SetLineSpace( 0 );
	m_vlist.SetScrollStep( 80 );
	m_vlist.SetRowHeight( 48 );
	m_vlist.SetVirtualMode( &m_dbase, CRListDlg::_TranslateFromReocord );
	
	m_autolist.SetLineSpace( 0 );
	m_autolist.SetScrollStep( 80 );
	HIMAGE hBk = CreateImage( XT2W(strImagePath + _T("bkwnd.png")) );
	//ARGB hBk = RGB( 128,128,128 );
// 	m_tree.SetBkGnd(hBk );
// 	m_list.SetBkGnd( hBk);
// 	m_autolist.SetBkGnd( hBk);
// 	m_vlist.SetBkGnd( hBk);
// 	m_tab.SetBkGnd( hBk);
	imgViewBkGnd.imageAvgClr = Color( 150, 255,255,255 ).GetValue();
 	m_tree.SetBkGnd( imgViewBkGnd );
 	m_list.SetBkGnd( imgViewBkGnd );
 	m_autolist.SetBkGnd( imgViewBkGnd );
    m_vlist.SetBkGnd( imgViewBkGnd );
    m_tab.SetBkGnd( imgViewBkGnd );
	margin.top = 2;
	margin.bottom = 2;
	m_list.SetMargin( margin );
	margin.top = 25;
	m_tree.SetMargin( margin );
	hBk = CreateImage( XT2W(strImagePath + _T("bk.png")) );
	ImageInfo imageInfo = {0};
	hBk->GetImageInfo(&imageInfo);
	imgViewBkGnd.hImage = hBk;
	SetBkGnd( imgViewBkGnd );
	m_comboDropView.SetBkGnd( imgViewBkGnd );
	InitHead();
	Initlist();
	InitTree();
	m_head.HookNotify( CRListDlg::XMessageReciever, (DWORD)(DWORD_PTR)this );
	m_tree.HookNotify( CRListDlg::XMessageReciever, (DWORD)(DWORD_PTR)this );
	m_tab.HookNotify( CRListDlg::XMessageReciever, (DWORD)(DWORD_PTR)this );
	HIMAGE image = CreateImage( XT2W( _T(".\\image\\head.png")) );
	if ( image )
	{
		SetIcon( image->GetIcon(), TRUE );
		SetIcon( image->GetIcon(), FALSE);
		m_tray.Create( 1500 );
		m_tray.SetImage( &image, 1 );
		m_tray.Show( true );
		m_tray.SetTipInfo( _T("显示Tab") );
		m_tray.ShowBalloonTip( _T(""), _T("已登录"), 10000 );
		m_tray.SetUICALLBACK( CRListDlg::XMessageReciever, (DWORD)(DWORD_PTR)this );
	}
	SetWindowText( _T("UIDemo") );
	SetStyle( GetStyle()|WS_CAPTION );
	m_MenuBk.hImage = CreateImage( XT2W( _T(".\\image\\menu_bk.png") ) );
	m_MenuBk.margin.left = 25;
	m_MenuBk.margin.bottom = m_MenuBk.margin.right = 10;
	m_MenuBk.margin.top = 15;
 	m_subMenuBk.hImage = CreateImage( _T(".\\image\\scroll_3.png")  );
 	m_subMenuBk.margin.left = 4;
 	m_subMenuBk.margin.bottom = m_MenuBk.margin.right = 10;
 	m_subMenuBk.margin.top = 4;
	m_menuSeparator.imageAvgClr = Color( 254, 150, 150, 150 ).GetValue();
	MENUATTRIBUTE menuAttrib;
	{
		Margin margin = { 3, 15, 6, 5 };
		Margin marginSub = { 3, 3, 3, 3 };
		menuAttrib.uMask = MA_MARGIN|MA_BKGND|MA_SEPARATOR|MA_SUBARROW|MA_ALPHA|MA_SUBBKGND\
			|MA_SUBMARGIN|MA_ITEMHEIGHT|MA_TEXTCLR|MA_GRAYTEXTCLR|MA_ITEMOFFSET;
		menuAttrib.pBkGnd  = &m_MenuBk;
		menuAttrib.pMargin	= &margin;
		menuAttrib.pSubMargin = &marginSub;
		menuAttrib.pSubBkGnd = &m_subMenuBk;
		menuAttrib.pSeparator = &m_menuSeparator;
		menuAttrib.uAlpha = 240;
		menuAttrib.uItemHeight = 20;
		menuAttrib.uItemOffset = 5;
		menuAttrib.clrFont = Color( 254, 255, 0, 0 ).GetValue();
		menuAttrib.clrGrayFont = Color( 254, 100, 100, 100 ).GetValue();
		menuAttrib.pSubArrow = CreateImage( _T(".\\image\\lan_xl_1.png") );
		::UISetMenuAttribute( &menuAttrib );
	}
	m_tab.ClickItem( ID_LIST );
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

BOOL CRListDlg::_LoadImages()
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
		m_RLBmpArray[BMP_ONLINE] = CreateImage( XT2W(strImagePath + _T("head.png")) );
	}
	m_RLBmpArray[BMP_L3] = CreateImage( XT2W(strImagePath + _T("smsqp_l_03.png")) );
	m_RLBmpArray[BMP_L2] = CreateImage( XT2W(strImagePath + _T("smsqp_l_02.png")) );
	m_RLBmpArray[BMP_L1] = CreateImage( XT2W(strImagePath + _T("smsqp_l_01.png")) );
	m_RLBmpArray[BMP_R1] = CreateImage( XT2W(strImagePath + _T("smsqp_r_01.png")) );
	m_RLBmpArray[BMP_R2] = CreateImage( XT2W(strImagePath + _T("smsqp_r_02.png")) );
	m_RLBmpArray[BMP_R3] = CreateImage( XT2W(strImagePath + _T("smsqp_r_03.png")) );
	m_RLBmpArray[BMP_CAR] = CreateImage(_T(".\\image\\car.png") );
	m_RLBmpArray[BMP_PAD] = CreateImage(_T(".\\image\\pad.png") );
	HICON hIcon = m_RLBmpArray[BMP_ONLINE]->GetIcon();
	SetIcon( hIcon, TRUE );
	SetIcon( hIcon, FALSE );
	return TRUE;
}
enum
{
	IDC_ATC_HEAD = 1000,
	IDC_ATC_NAME,
	IDC_ATC_CALL,
	IDC_ATC_SMS,
	IDC_ATC_CONTENT,
	IDC_ATC_SIGN,
};

BOOL CRListDlg::_TranslateFromReocord( LPVOID pRecord, CUIListCtrl::ROW& rowData )
{
#define LINE_SPACE	  2
#define V_SPACE       7
#define H_SPACE       2
#define TEXT_HEIGHT   16
#define BUTTON_WIDTH  20
#define BUTTON_HEIGHT 20
#define HEAD_HEIGHT   32
#define IMAGE_HEIGHT  16

	INFO* pRecordInfo = (INFO*)pRecord;
	if ( NULL == pRecordInfo )
	{
		return FALSE;
	}
	//拼装列表　行
	int _X = H_SPACE;
	int _Y = V_SPACE;
	int _Xb = 0;
	int _Yb = 0;

	int nRowHeight = 48;
	int nYOrg = (nRowHeight - HEAD_HEIGHT)/2;
	int nXOrg = 5;
	//插入头像
	CUIImage* pUHead = NULL;
	IMG img;
	if ( pUHead = (CUIImage*)rowData.GetXCtrl( IDC_ATC_HEAD ) )
	{
		img.hImage = m_RLBmpArray[BMP_ONLINE];
		pUHead->SetImage( img );
	}
	else
	{
		pUHead = (CUIImage*)rowData.AddXCtrl( XW_IMAGE, _T(""), RECT_C( nXOrg, nYOrg, nXOrg + HEAD_HEIGHT, nYOrg + HEAD_HEIGHT ), IDC_ATC_HEAD );
		if ( pUHead )
		{
			img.hImage = m_RLBmpArray[BMP_ONLINE];
			pUHead->SetImage( img );
		}
	}
	_X += 41;
	//插入昵称（当没有昵称时，显示帐号）
	IUICtrl* pUnitName = NULL;
	if ( pUnitName = rowData.GetXCtrl( IDC_ATC_NAME ) )
	{
		pUnitName->SetText( pRecordInfo->strSendName );
	}
	else
	{
		pUnitName = rowData.AddXCtrl( XW_TEXT, pRecordInfo->strSendName, RECT_C( _X, _Y, _X + 200, _Y+TEXT_HEIGHT ), IDC_ATC_NAME );
		if ( pUnitName )
		{
			pUnitName->SetTextColor( Color(254,0,0,0).GetValue() );
			pUnitName->SetFont( m_normalFont );
			pUnitName->SetStyle( pUnitName->GetStyle() | XWS_SINGLELINE | XWS_ENDELLIPSE );
		}
	}
	_Xb = _X;

	//定时短信区分标识
	//if ( 0 )
	//{
	//	//插入打电话按钮
	//	CUIImage* pUnitClock = (CUIImage*)rowData.AddXCtrl( XW_IMAGE, _T(""), RECT_C( _X+nNameUnitWidth+H_SPACE, _Y, _X+nNameUnitWidth+BUTTON_WIDTH+H_SPACE, _Y+BUTTON_HEIGHT ) );
	//	if ( pUnitClock )
	//	{
	//		pUnitClock->SetImage( m_RLBmpArray[BMP_CLOCK] );
	//		pUnitClock->SetHotImage( m_RLBmpArray[BMP_CLOCKHOT] );
	//	}
	//}
	_X += 200 + H_SPACE;
	_Y -= 3;
	//插入打电话按钮
	CUIButton* pButton = NULL;
	if ( pButton = (CUIButton*)rowData.GetXCtrl( IDC_ATC_CALL ) )
	{
		img.hImage = m_RLBmpArray[BMP_PHONE_NORMAL];
		pButton->SetNormalImage( img );
		img.hImage = m_RLBmpArray[BMP_PHONE_HOT];
		pButton->SetMouseOverImage( img );
		img.hImage = m_RLBmpArray[BMP_PHONE_PRESSED];
		pButton->SetPressedImage( img );
	}
	else
	{
		pButton = (CUIButton*)rowData.AddXCtrl( XW_BUTTON, _T(""), RECT_C( _X, _Y, _X+BUTTON_WIDTH, _Y+BUTTON_HEIGHT ), IDC_ATC_CALL );
		if ( pButton )
		{
			img.hImage = m_RLBmpArray[BMP_PHONE_NORMAL];
			pButton->SetNormalImage( img );
			img.hImage = m_RLBmpArray[BMP_PHONE_HOT];
			pButton->SetMouseOverImage( img );
			img.hImage = m_RLBmpArray[BMP_PHONE_PRESSED];
			pButton->SetPressedImage( img );
			pButton->SetStyle(XWS_HOTSHOW);
		}
	}

	_X += BUTTON_WIDTH + H_SPACE;
	//插入发短信按钮
	pButton = NULL;
	if ( pButton = (CUIButton*)rowData.GetXCtrl( IDC_ATC_SMS ) )
	{
		img.hImage = m_RLBmpArray[BMP_SMS_NORMAL];
		pButton->SetNormalImage( img );
		img.hImage = m_RLBmpArray[BMP_SMS_HOT];
		pButton->SetMouseOverImage( img );
		img.hImage = m_RLBmpArray[BMP_SMS_PRESSED];
		pButton->SetPressedImage( img );
	}
	else
	{
		pButton = (CUIButton*)rowData.AddXCtrl( XW_BUTTON, _T(""), RECT_C( _X, _Y, _X+BUTTON_WIDTH, _Y+BUTTON_HEIGHT ), IDC_ATC_SMS );
		if ( pButton )
		{
			img.hImage=m_RLBmpArray[BMP_SMS_NORMAL];
			pButton->SetNormalImage( img );
			img.hImage = m_RLBmpArray[BMP_SMS_HOT];
			pButton->SetMouseOverImage( img );
			img.hImage = m_RLBmpArray[BMP_SMS_PRESSED];
			pButton->SetPressedImage( img );
			pButton->SetStyle(XWS_HOTSHOW);
		}
	}

	_X += BUTTON_WIDTH + H_SPACE;
	//插入发短信按钮
	pButton = NULL;
	if ( pButton = (CUIButton*)rowData.GetXCtrl( IDC_ATC_SMS+200 ) )
	{
		img.hImage = m_RLBmpArray[BMP_SMS_NORMAL];
		pButton->SetNormalImage( img );
		img.hImage = m_RLBmpArray[BMP_SMS_HOT];
		pButton->SetMouseOverImage( img );
		img.hImage = m_RLBmpArray[BMP_SMS_PRESSED];
		pButton->SetPressedImage( img );
	}
	else
	{
		pButton = (CUIButton*)rowData.AddXCtrl( XW_BUTTON, _T(""), RECT_C( _X, _Y, _X+BUTTON_WIDTH, _Y+BUTTON_HEIGHT ), IDC_ATC_SMS+200 );
		if ( pButton )
		{
			img.hImage = m_RLBmpArray[BMP_SMS_NORMAL];
			pButton->SetNormalImage( img );
			img.hImage = m_RLBmpArray[BMP_SMS_HOT];
			pButton->SetMouseOverImage( img );
			img.hImage = m_RLBmpArray[BMP_SMS_PRESSED];
			pButton->SetPressedImage( img );
			pButton->SetStyle(XWS_HOTSHOW);
		}
	}

	_X += BUTTON_WIDTH + H_SPACE;
	//插入发短信按钮
	pButton = NULL;
	if ( pButton = (CUIButton*)rowData.GetXCtrl( IDC_ATC_SMS+100 ) )
	{
		img.hImage = m_RLBmpArray[BMP_SMS_NORMAL];
		pButton->SetNormalImage( img );
		img.hImage = m_RLBmpArray[BMP_SMS_HOT];
		pButton->SetMouseOverImage( img );
		img.hImage = m_RLBmpArray[BMP_SMS_PRESSED];
		pButton->SetPressedImage( img );
	}
	else
	{
		pButton = (CUIButton*)rowData.AddXCtrl( XW_BUTTON, _T(""), RECT_C( _X, _Y, _X+BUTTON_WIDTH, _Y+BUTTON_HEIGHT ), IDC_ATC_SMS+100 );
		if ( pButton )
		{
			img.hImage = m_RLBmpArray[BMP_SMS_NORMAL];
			pButton->SetNormalImage( img );
			img.hImage = m_RLBmpArray[BMP_SMS_HOT];
			pButton->SetMouseOverImage( img );
			img.hImage = m_RLBmpArray[BMP_SMS_PRESSED];
			pButton->SetPressedImage( img );
			pButton->SetStyle(XWS_HOTSHOW);
		}
	}

	_X = _Xb;
	_Y += TEXT_HEIGHT + LINE_SPACE + 3;
	_X += H_SPACE;


	//插入短信类型标识(图片)
	CUIImage* pUSign = NULL;
	if ( pUSign = (CUIImage*)rowData.GetXCtrl( IDC_ATC_SIGN ) )
	{
		img.hImage = m_RLBmpArray[BMP_RECIEVED];
		pUSign->SetImage( img );
	}
	else
	{
		pUSign = (CUIImage*)rowData.AddXCtrl( XW_IMAGE, _T(""), RECT_C( _X, _Y, _X + IMAGE_HEIGHT, _Y+IMAGE_HEIGHT ), IDC_ATC_SIGN );
		if ( pUSign )
		{	
			//解析发出/接收短信标识的索引号
			img.hImage = m_RLBmpArray[BMP_RECIEVED];
			pUSign->SetImage( img );
		}
	}

	//插入短信内容
	_Y+=2;
// 	static int test = 20;
// 	_Y+= test+20;
// 	test = -test;
	_X += IMAGE_HEIGHT + H_SPACE;
	//插入短信内容
	IUICtrl* pUnitContent = NULL;
	if ( pUnitContent = rowData.GetXCtrl( IDC_ATC_CONTENT ) )
	{
		pUnitContent->SetText( pRecordInfo->strContent );
	}
	else
	{
		pUnitContent = rowData.AddXCtrl( XW_TEXT, pRecordInfo->strContent, RECT_C( _X, _Y, _X + 122, _Y+TEXT_HEIGHT ) , IDC_ATC_CONTENT );

		if( pUnitContent )
		{
			pUnitContent->SetFont( m_normalFont );
			pUnitContent->SetStyle( pUnitContent->GetStyle() | XWS_SINGLELINE | XWS_ENDELLIPSE );
			if ( 0 == pRecordInfo->strReadFlg.compare( _T("1")) )
			{
				pUnitContent->SetTextColor( Color(254,0,0,0).GetValue() );
			}
			//对未读短信，标粗
			else
			{
				pUnitContent->SetTextColor( Color(254,0,0,0).GetValue() );
			}	
		}
	}

	return TRUE;
}

BOOL CRListDlg::_TranslateFromReocord1( LPVOID pRecord, CUIListCtrl::ROW& rowData )
{
#define LINE_SPACE	  2
#define V_SPACE       7
#define H_SPACE       2
#define TEXT_HEIGHT   16
#define BUTTON_WIDTH  20
#define BUTTON_HEIGHT 20
#define HEAD_HEIGHT   32
#define IMAGE_HEIGHT  16

	INFO* pRecordInfo = (INFO*)pRecord;
	if ( NULL == pRecordInfo )
	{
		return FALSE;
	}
	//拼装列表　行
	int _X = H_SPACE;
	int _Y = V_SPACE;
	int _Xb = 0;
	int _Yb = 0;

	int nRowHeight = 48;
	int nYOrg = (nRowHeight - HEAD_HEIGHT)/2;
	int nXOrg = 5;

	RECT rcClient ={ 0, 0, 300, 200 };
	//插入短信内容
	rcClient.right -= 45;
	
	_X += 15;
	IUICtrl* pUnit = rowData.AddXCtrl( XW_TEXT, pRecordInfo->strTime, RECT_C( _X, _Y, RECT_W(rcClient), _Y+20 ), IDC_ATC_SIGN );
	pUnit->SetTextColor( Color(254,0,0,0).GetValue() );
	pUnit->SetStyle( XWS_SINGLELINE );
	pUnit->SetFont( m_normalFont );
	SIZE size = IUICtrl::TestTextSize( *pUnit );
	if ( pRecordInfo->bRecieved )
	{
		pUnit->SetRect( RECT_C(_X, _Y, _X+size.cx, _Y+size.cy) );
	}
	else
	{
		pUnit->SetRect( RECT_C(rcClient.right-size.cx, _Y, rcClient.right, _Y+size.cy) );
	}
	_Y += size.cy+10;
	IMG img;
	img.margin.left = 13;
	img.margin.top = 20;
	img.margin.right = 10;
	img.margin.bottom = 10;
	pUnit = rowData.AddXCtrl( XW_BUTTON, _T(""), RECT_C( _X, _Y, RECT_W(rcClient), _Y+20 ), IDC_ATC_NAME );
	if ( pRecordInfo->bRecieved )
	{
		img.hImage = m_RLBmpArray[BMP_L1];
		((CUIButton*)pUnit)->SetNormalImage( img );
		img.hImage = m_RLBmpArray[BMP_L2];
		((CUIButton*)pUnit)->SetMouseOverImage( img );
		img.hImage = m_RLBmpArray[BMP_L3];
		((CUIButton*)pUnit)->SetPressedImage( img );
	}
	else
	{
		img.hImage = m_RLBmpArray[BMP_R1];
		((CUIButton*)pUnit)->SetNormalImage( img );
		img.hImage = m_RLBmpArray[BMP_R2];
		((CUIButton*)pUnit)->SetMouseOverImage( img );
		img.hImage = m_RLBmpArray[BMP_R3];
		((CUIButton*)pUnit)->SetPressedImage( img );
	}

	IUICtrl* pUnitContent = NULL;
// 	if ( pUnitContent = rowData.GetXCtrl( IDC_ATC_CONTENT ) )
// 	{
// 		if ( pRecordInfo->bRecieved )
// 		{
// 			rcClient.right -= 50;
// 		}
// 		else
// 		{
// 			_X += 50;
// 			rcClient.left += 50;
// 		}
// 		pUnitContent->SetText( pRecordInfo->strContent );
// 		SIZE size = IUICtrl::TestTextSize( *pUnitContent );
// 		RECT rcText(_X, _Y, _X+size.cx, _Y+size.cy);
// 		pUnitContent->SetRect( rcText );
// 		rcText.InflateRect( 20, 10 );
// 		pUnit->SetRect( rcText );
// 	}
// 	else
// 	{
		if ( pRecordInfo->bRecieved )
		{
			rcClient.right -= 50;
		}
		else
		{
			_X += 50;
			rcClient.left += 50;
		}
		
		pUnitContent = rowData.AddXCtrl( XW_TEXT, pRecordInfo->strContent, RECT_C( _X, _Y, _X + RECT_W(rcClient), _Y+600 ) , IDC_ATC_CONTENT );
		if( pUnitContent )
		{
			pUnitContent->SetFont( m_normalFont );
			pUnitContent->HasNotify( false );
			pUnitContent->SetStyle( pUnitContent->GetStyle() );
			SIZE size = IUICtrl::TestTextSize( *pUnitContent );
			if ( RECT_W(rcClient) > size.cy 
				&& !pRecordInfo->bRecieved )
			{
				_X = rcClient.right - size.cx;
			}
			RECT rcText={_X, _Y, _X+size.cx, _Y+size.cy};
			pUnitContent->SetRect( rcText );
			InflateRect( &rcText, 20, 10 );
			pUnit->SetRect( rcText );
			if ( 0 == pRecordInfo->strReadFlg.compare( _T("1")) ) 
			{
				pUnitContent->SetTextColor( Color(254,0,0,0).GetValue());
			}
			//对未读短信，标粗
			else
			{
				pUnitContent->SetTextColor( Color(254,0,0,0).GetValue() );
			}
		}
	//}

	return TRUE;
}

BOOL CRListDlg::XMessageReciever( UINT message, WPARAM wParam, LPARAM lParam, DWORD dwData )
{
	NM_PARAM* param = (NM_PARAM*)lParam;
	CRListDlg* pThis = (CRListDlg*)(DWORD_PTR)dwData;
	if ( param && pThis )
	{
		if ( wParam == 1500 )
		{
			if ( message == WM_RBUTTONUP )
			{
				CUIMenu menu3;
				ACCEL accel;
				accel.fVirt = FVIRTKEY;
				accel.cmd = 1000;
				accel.key = _T('x');
				menu3.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu3.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu3.AddAccel( accel );
				accel.cmd = 1003;
				menu3.AddAccel( accel );
				menu3.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				menu3.InsertMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_CAR], 1001 );
				menu3.InsertMenu( _T("还原(&R)"), m_RLBmpArray[BMP_CAR], 1003 );
				menu3.InsertSpaceMenu();
				menu3.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_CAR], 1002 );

				CUIMenu menu2;
				menu2.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				accel.fVirt = FVIRTKEY;
				accel.cmd = 1000;
				accel.key = _T('x');
				menu2.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu2.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu2.AddAccel( accel );
				accel.cmd = 1003;
				accel.key = _T('r');
				menu2.AddAccel( accel );

				menu2.InsertMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_CAR], 1001 );
				menu2.InsertSubMenu(  _T("还原(&R)"), m_RLBmpArray[BMP_CAR], &menu3 );
				menu2.InsertSpaceMenu();
				menu2.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_CAR], 1002 );

				CUIMenu menu1;
				menu1.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				menu1.InsertSubMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_CAR], &menu2 );
				menu1.InsertMenu( _T("还原(&R)"), m_RLBmpArray[BMP_CAR], 1003, FALSE );
				menu1.InsertSpaceMenu();
				menu1.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_CAR], 1002 );

				accel.cmd = 1000;
				accel.key = _T('x');
				menu1.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu1.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu1.AddAccel( accel );
				accel.cmd = 1003;
				menu1.AddAccel( accel );

				CUIMenu menu;
				menu.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				menu.InsertMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_PAD],1001, FALSE );
				menu.InsertMenu( _T("还原(&R)"), m_RLBmpArray[BMP_PAD], 1003 );
				menu.InsertSpaceMenu();
				menu.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_PAD], 1002 );
				menu.InsertSubMenu( _T("其它(&O)"), m_RLBmpArray[BMP_PAD], &menu1 );
				accel.cmd = 1000;
				accel.key = _T('x');
				menu.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu.AddAccel( accel );
				accel.cmd = 1003;
				accel.key = _T('r');
				menu.AddAccel( accel );
				accel.cmd = 1004;
				accel.key = _T('o');
				menu.AddAccel( accel );
				menu.InsertMenu( _T("显示面板(&S)"), m_RLBmpArray[BMP_CAR], 1008 );
				menu.InsertSpaceMenu();
				menu.InsertMenu( _T("退出(&C)"), m_RLBmpArray[BMP_PAD], 1009 );
				menu.InsertSpaceMenu();
				accel.cmd = 1008;
				accel.key = _T('s');
				menu.AddAccel( accel );
				accel.cmd = 1009;
				accel.key = _T('C');
				menu.AddAccel( accel );
				UINT nCmd = menu.TrackPopupMenu( param->point.x, param->point.y, MAL_TOP );
				if ( nCmd == 1008 )
				{
					pThis->Show( SW_SHOWNORMAL );
					pThis->BringWindowToTop();
				}
				else if ( nCmd == 1009 )
				{
					pThis->OnCancel();
				}
			}
			else if ( message == WM_LBUTTONDBLCLK )
			{
				UIActiveProcess();
				pThis->Show( SW_SHOWNORMAL );
				pThis->BringWindowToTop();
			}
			return TRUE;
		}
		if ( XWM_CHANGED == message )
		{
			if ( IDC_ALPHA == param->nCtrlId )
			{
				if( param->wParam )
				{
					IMG image;
					image.imageAvgClr = Color(0, 255, 255, 255).GetValue();
					pThis->m_viewMark.SetBkGnd( image );
					pThis->m_viewMark.Show(true);
					//pThis->EnableBlurWindow( true );
 					for ( int _i = 10; _i <= 240; _i += 80 )
 					{
						image.imageAvgClr = Color(_i, 255, 255, 255).GetValue();
 						pThis->m_viewMark.SetBkGnd( image );
 						pThis->InvalidRect(NULL);
 					}
				}
				else
				{
					//pThis->EnableBlurWindow( false );
					pThis->m_viewMark.Show( false );
				}
				pThis->InvalidRect( NULL );
			}
			else if ( IDC_SHOWTAB == param->nCtrlId )
			{
				RECT rcClient;
				pThis->GetClient( &rcClient );
				::InflateRect( &rcClient, -2, -2 );
 				if ( param->wParam != 0 )
 				{
 					rcClient.top = 82;
 				}
 				else
 				{
					rcClient.top = 52;
				}
				rcClient.bottom -= 10;
				pThis->m_vlist.SetRect( rcClient);
				pThis->m_list.SetRect( rcClient);
				pThis->m_autolist.SetRect( rcClient );
				pThis->m_tree.SetRect( rcClient );
				pThis->m_tab.Show( param->wParam != 0 );
				pThis->InvalidRect(NULL);
			}
			else if ( IDC_PSD == param->nCtrlId )
			{
				CUIEdit* pCtrl = (CUIEdit*)pThis->m_head.GetXCtrl(5050);
				RECT rcEdit= { 55, 25, 0, 45 };
				if( pCtrl )
				{
					if ( param->wParam != 0 )
					{
						pCtrl->SetText( _T("abc") );
						pCtrl->SetPasswordMode( true );
					}
					else
					{
						pCtrl->SetText( _T("存在无需理由!") );
						pCtrl->SetPasswordMode( false );
					}
					pCtrl->InvalidRect(  );
				}
			}
			return TRUE;
		}
		
		if ( XLAY_SIZECHANGED == message )
		{
			CUILayPanel* pPanel = (CUILayPanel*)lParam;
			if( pPanel )
			{
				if ( wParam == 1008 )
				{
					IUICtrl* pCtrl = pPanel->GetXCtrl(ID_CLOSE);
					SIZE sizePanel = pPanel->GetSize();
					int _x = sizePanel.cx - 37;
					int _y = 1;
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(_x, 0, _x+37, _y+16) );
					}
					pCtrl = pPanel->GetXCtrl(ID_MAX);
					if( pCtrl )
					{
						_x -= 28;
						pCtrl->SetRect( RECT_C(_x, 0, _x+28, _y+15) );
					}
					pCtrl = pPanel->GetXCtrl(ID_RESTORE);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(_x, 0, _x+28, _y+15) );
					}
					pCtrl = pPanel->GetXCtrl(ID_MINI);
					if( pCtrl )
					{
						_x -= 28;
						pCtrl->SetRect( RECT_C(_x, 0, _x+28, _y+15) );
					}
					pCtrl = pPanel->GetXCtrl(5050);
					RECT rcEdit= { 55, 16, 0, 55 };
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(rcEdit.left, rcEdit.top, sizePanel.cx-307, rcEdit.bottom ) );
					}
					pCtrl = pPanel->GetXCtrl(ID_SKIN);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(sizePanel.cx-305, rcEdit.top, sizePanel.cx-225, rcEdit.bottom ) );
					}
					pCtrl = pPanel->GetXCtrl(IDC_ALPHA);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(sizePanel.cx-70, rcEdit.top+2, sizePanel.cx-8, rcEdit.bottom ) );
					}
					pCtrl = pPanel->GetXCtrl(IDC_SHOWTAB);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(sizePanel.cx-160, rcEdit.top+2, sizePanel.cx-75, rcEdit.bottom ) );
					}
					pCtrl = pPanel->GetXCtrl(IDC_PSD);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C(sizePanel.cx-220, rcEdit.top+2, sizePanel.cx-165, rcEdit.bottom ) );
					}
				}
				else if ( wParam == 1010 )
				{
					IUICtrl* pCtrl = pPanel->GetXCtrl(ID_TREE);
					SIZE sizePanel = pPanel->GetSize();
					int _x = sizePanel.cx - 41;
					int _y = 1;
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C( 0, 0, sizePanel.cx, sizePanel.cy ) );
					}
					
					pCtrl = pPanel->GetXCtrl(ID_VLLIST);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C( 0, 0, sizePanel.cx, sizePanel.cy ) );
					}

					pCtrl = pPanel->GetXCtrl(ID_LIST);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C( 0, 0, sizePanel.cx, sizePanel.cy ) );
					}

					pCtrl = pPanel->GetXCtrl(ID_AUTOLIST);
					if( pCtrl )
					{
						pCtrl->SetRect( RECT_C( 0, 0, sizePanel.cx, sizePanel.cy ) );
					}
				}
			}
			return TRUE;
		}
		else if ( XLAY_LCLICK == message )
		{
			if( wParam == 1010 )
			{
 				pThis->m_autolist.Show( ID_AUTOLIST == param->nCtrlId );
 				pThis->m_list.Show( ID_LIST == param->nCtrlId );
 				pThis->m_vlist.Show( ID_VLLIST == param->nCtrlId );
 				pThis->m_tree.Show( ID_TREE == param->nCtrlId );
 				pThis->InvalidRect( NULL );
			}
			return TRUE;
		}
		else if ( XLAY_RBDOWN == message )
		{
			if ( pThis->m_head.GetID() == wParam )
			{
				pThis->m_head.ClientToScreen( &(param->point) );

				CUIMenu menu3;
				ACCEL accel;
				accel.fVirt = FVIRTKEY;
				accel.cmd = 1000;
				accel.key = _T('x');
				menu3.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu3.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu3.AddAccel( accel );
				accel.cmd = 1003;
				menu3.AddAccel( accel );
				menu3.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				menu3.InsertMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_CAR], 1001 );
				menu3.InsertMenu( _T("还原(&R)"), m_RLBmpArray[BMP_CAR], 1003 );
				menu3.InsertSpaceMenu();
				menu3.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_CAR], 1002 );

				CUIMenu menu2;
				menu2.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				accel.fVirt = FVIRTKEY;
				accel.cmd = 1000;
				accel.key = _T('x');
				menu2.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu2.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu2.AddAccel( accel );
				accel.cmd = 1003;
				accel.key = _T('r');
				menu2.AddAccel( accel );

				menu2.InsertMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_CAR], 1001 );
				menu2.InsertSubMenu(  _T("还原(&R)"), m_RLBmpArray[BMP_CAR], &menu3, FALSE );
				menu2.InsertSpaceMenu();
				menu2.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_CAR], 1002 );
				
				CUIMenu menu1;
				menu1.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				menu1.InsertSubMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_CAR], &menu2 );
				menu1.InsertMenu( _T("还原(&R)"), m_RLBmpArray[BMP_CAR], 1003, FALSE );
				menu1.InsertSpaceMenu();
				menu1.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_CAR], 1002 );

				accel.cmd = 1000;
				accel.key = _T('x');
				menu1.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu1.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu1.AddAccel( accel );
				accel.cmd = 1003;
				menu1.AddAccel( accel );

				CUIMenu menu;
				menu.InsertMenu( _T("最大化(&X)"), m_RLBmpArray[BMP_CAR], 1000 );
				menu.InsertMenu( _T("最小化(&N)"), m_RLBmpArray[BMP_PAD],1001, FALSE );
				menu.InsertMenu( _T("还原(&R)"), m_RLBmpArray[BMP_PAD], 1003 );
				menu.InsertSpaceMenu();
				menu.InsertMenu( _T("关闭(&C)"), m_RLBmpArray[BMP_PAD], 1002 );
				menu.InsertSubMenu( _T("其它(&O)"), m_RLBmpArray[BMP_PAD], &menu1 );
				
				accel.cmd = 1000;
				accel.key = _T('x');
				menu.AddAccel( accel );
				accel.cmd = 1001;
				accel.key = _T('n');
				menu.AddAccel( accel );
				accel.cmd = 1002;
				accel.key = _T('c');
				menu.AddAccel( accel );
				accel.cmd = 1003;
				accel.key = _T('r');
				menu.AddAccel( accel );
				accel.cmd = 1004;
				accel.key = _T('o');
				menu.AddAccel( accel );

				switch( menu.TrackPopupMenu( param->point.x, param->point.y, 0 ) )
				{
				case 1002:
					{
						pThis->OnCancel();
					}
					break;
				case 1000:
					{
						if( !pThis->IsZoomed( ) )
						{
							IUICtrl* pCtrl = pThis->m_head.GetXCtrl(ID_MAX);
							if( pCtrl )
							{
								pCtrl->Show( false );
							}
							pCtrl = pThis->m_head.GetXCtrl(ID_RESTORE);
							if( pCtrl )
							{
								pCtrl->Show(  );
							}
							pThis->Show( SW_SHOWMAXIMIZED );
						}
					}
					break;
				case 1001:
					{
						pThis->Show( SW_SHOWMINIMIZED );
					}
					break;
				case 1003:
					{		
						IUICtrl* pCtrl = pThis->m_head.GetXCtrl(ID_MAX);
						if( pCtrl )
						{
							pCtrl->Show(  );
						}
						pCtrl = pThis->m_head.GetXCtrl(ID_RESTORE);
						if( pCtrl )
						{
							pCtrl->Show( false );
						}
						pThis->Show( SW_RESTORE );
					}
					break;
				default:
					break;
				}
			}
			else if ( pThis->m_tree.GetID() == wParam )
			{
				CUITreeCtrl::HNODE hNode = pThis->m_tree.GetNodeFromPoint( param->point );
				pThis->m_tree.ClientToScreen( &param->point );
				CUIMenu menu;
				menu.InsertMenu( _T("删除"), HIMAGE(), 1000 );
				menu.InsertMenu( _T("掩码窗"), HIMAGE(), 1001 );
				UINT nCmd = menu.TrackPopupMenu( param->point.x, param->point.y, 0 );
				if( 1000 == nCmd )
				{
					pThis->m_tree.Remove( hNode );					
				}
				else if ( nCmd == 1001 )
				{
					IMG image;
					image.imageAvgClr = Color(0, 255, 255, 255).GetValue();
					pThis->m_viewMark.SetBkGnd( image );
					pThis->m_viewMark.Show(true);
					//pThis->EnableBlurWindow( true );
					for ( int _i = 10; _i <= 240; _i += 80 )
					{
						image.imageAvgClr = Color(_i, 255, 255, 255).GetValue();
						pThis->m_viewMark.SetBkGnd( image );
						pThis->InvalidRect(NULL);
					}
				}
			}
		}
		else if( XWM_LCLICK == message )
		{
			if( param->nCtrlId == ID_CLOSE )
			{
				pThis->OnCancel();
			}
			else if( param->nCtrlId == ID_MAX )
			{
				if( !pThis->IsZoomed( ) )
				{
					IUICtrl* pCtrl = pThis->m_head.GetXCtrl(ID_MAX);
					if( pCtrl )
					{
						pCtrl->Show( false );
					}
					pCtrl = pThis->m_head.GetXCtrl(ID_RESTORE);
					if( pCtrl )
					{
						pCtrl->Show( true );
					}
					pThis->Show( SW_SHOWMAXIMIZED );
				}
			}
			else if( param->nCtrlId == ID_MINI )
			{
				pThis->Show( SW_SHOWMINIMIZED );
				pThis->Show( SW_HIDE );
				//::AnimateWindow( pThis->m_hWnd, 1000, AW_HIDE|AW_SLIDE|AW_BLEND );
			}
			else if ( param->nCtrlId == ID_RESTORE )
			{
				IUICtrl* pCtrl = pThis->m_head.GetXCtrl(ID_MAX);
				if( pCtrl )
				{
					pCtrl->Show(  );
				}
				pCtrl = pThis->m_head.GetXCtrl(ID_RESTORE);
				if( pCtrl )
				{
					pCtrl->Show( false );
				}
				pThis->Show( SW_RESTORE );
			}
			else if ( param->nCtrlId == ID_SKIN )
			{
				{
// 					CUIMenu menu;
// 					//menu.Create(  RECT_C( 200,200, 400, 800 ), 1000 );
// 					menu.TrackPopupMenu(  );
				}
				TCHAR szPath[MAX_PATH] = {0};
				GetCurrentDirectory( MAX_PATH, szPath );
				OPENFILENAME ofn;       // common dialog box structure
				TCHAR szFile[260]={0};       // buffer for file name

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = pThis->GetHandle();
				ofn.lpstrFile = szFile;
				//
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				//
				ofn.lpstrFile[0] = _T('\0');
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = _T("png\0*.png\0jpg\0*.jpg\0bmp\0*.bmp\0");
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 

				if (GetOpenFileName(&ofn)) 
				{
					IMG image;
					image.hImage = CreateImage( XT2W(ofn.lpstrFile));
					pThis->SetBkGnd(image);
				}
				SetCurrentDirectory( szPath );
			}
			return TRUE;
		}	
	}
	return FALSE;
}

void CRListDlg::InitHead()
{
 	CUIImage image;
 	image.Create( _T(""), RECT_C( 5, 5, 45, 45 ) );
	IMG img;
	img.margin.left = img.margin.right=img.margin.top=img.margin.bottom=3;
	img.hImage = m_RLBmpArray[BMP_ONLINE];
 	image.SetImage( img );
 	image.HasNotify( false );
 	m_head.AddXCtrl( &image );

	CUIButton btn(false);
	btn.Create( _T(""), RECT_C( 0, 0, 50, 50 ),ID_HEAD );
	btn.SetTextColor( Color(254,0,0,0).GetValue() );
	img.hImage = m_RLBmpArray[BMP_OFFLINE];
	img.margin.left = img.margin.right=img.margin.top=img.margin.bottom=0;
	btn.SetMouseOverImage( img );
	m_head.AddXCtrl( &btn );
	CUIText name;
	name.Create( _T("你好！"), RECT_C( 0, 0, 1000, 1000 ), ID_NAME );
	name.SetStyle( XWS_SINGLELINE|XWS_ALIGNVCENTER );
	name.SetFont( m_normalFont );
	name.EnableOutGlaw(true);
	name.SetTextColor( Color(254,255,255,255).GetValue() );
	SIZE size = IUICtrl::TestTextSize( name );
	name.SetRect(  RECT_C( 55, 2, 55+size.cx, 2+size.cy ) );
	m_head.AddXCtrl( &name );
	name.EnableOutGlaw(false);
	m_head.Show( true );
	IMG imageViewBkGnd;
	imageViewBkGnd.imageAvgClr = X_TRANSPARENT;
	m_head.SetBkGnd( imageViewBkGnd );
	tstring strImagePath = _T(".\\image\\");
	HIMAGE hNormal = CreateImage( XT2W( strImagePath+_T("dl_btn_1.png") ) );
	HIMAGE hOver = CreateImage( XT2W( strImagePath+_T("dl_btn_2.png") ) ) ;
	HIMAGE hPressed = CreateImage( XT2W( strImagePath+_T("dl_btn_3.png") ) );
	CUIEdit edit;
	edit.Create( _T(""),RECT_C( 55,25, 500, 65 ), 5050 );
	edit.SetFont( m_normalFont );
	edit.SetStyle( XWS_ALIGNLEFT );
	img.margin.left = img.margin.right = 3;
	img.margin.top = img.margin.bottom = 2;
	img.hImage = CreateImage( XT2W( strImagePath+_T("editBk.png")));
	edit.SetMouseOverImage( img );
	edit.SetFocusImage( *edit.GetMouseOverImage() );
	Margin margin;
	margin.left = margin.right= 2;
	margin.top=margin.bottom= 2;
	edit.SetMargin( margin );
	edit.SetTextColor( Color(254,255,255,255).GetValue() );
	edit.SetParent( &m_head );
	//edit.SetText( _T("不要问我存在的理由！") );
	//edit.SetText( _T("abc") );
	edit.EnableOutGlaw( true );
	//edit.SetNumberOnly( true );
	edit.SetLimitText( 20 );
	//edit.SetReadOnly(  );
	//edit.SetPasswordMode( true, _T('#') );
	//edit.Enable( false );
	m_head.AddXCtrl( &edit );


	RECT rectComb = { 0, 0, 200, 20 };
	CUIComboBox comboBox;
	comboBox.SetNormalImage( img );
	comboBox.Create( _T("测试组合框"), rectComb, 10000 );
	m_tree.AddXCtrl( &comboBox, CUIView::TOP );
	CUIComboBox* pCombBox = (CUIComboBox*)m_tree.GetXCtrl( 10000 );
	if ( pCombBox )
	{
		pCombBox->SetLBTextColor( Color(254, 255, 0, 0).GetValue() );
		pCombBox->SetFont( m_normalFont );
		pCombBox->SetMargin( margin );
		pCombBox->SetTextColor( Color(254,255,255,255).GetValue() );
		//pCombBox->SetDropView( &m_comboDropView );
		margin.left = margin.right=margin.top=margin.bottom = 2;
		pCombBox->SetDropMargin( margin );
		pCombBox->SetDropBkGnd( *m_comboDropView.GetBkGnd() );
		pCombBox->SetReadOnly( false );
		IMG img;
		img.hImage = CreateImage( XT2W( strImagePath+_T("ico_s_cd.png") ) );
		if ( img.hImage )
		{
			pCombBox->SetArrowSize( img.hImage->GetWidth(), img.hImage->GetHeight() );
		}
		pCombBox->SetArrowNormalImage( img );
		img.hImage = CreateImage( XT2W( strImagePath+_T("ico_s_cd_2.png") ) );
		pCombBox->SetArrowMouseOverImage( img );
		img.hImage = CreateImage( XT2W( strImagePath+_T("ico_s_cd_3.png") ) );
		pCombBox->SetArrowFocusImage( img );
		//margin.left = margin.right = margin.top = margin.bottom = 10;
		IMG imageTmp;
		pCombBox->SetDropViewBkGnd( imageTmp );
		pCombBox->SetLBHeight( 13 );
		pCombBox->SetLBSpace( 2 );
		IMG image;
		image.imageAvgClr = Color(254,128,128,255).GetValue();
		pCombBox->SetLBHotBkGnd( image );
		pCombBox->SetReadOnly(  );
		//pCombBox->SetDropViewMargin( margin );
		pCombBox->SetDropViewMaxHeight( 100 );
		pCombBox->AddString(_T("应用的调用频率都很低"));
		pCombBox->AddString(_T("使用GDI/GDI+提供的接口"));
		pCombBox->AddString(_T("游戏的窗口是时时刻刻在重绘的"));
		pCombBox->AddString(_T("当你调用了DrawText或TextOut等方法绘制文字"));
		pCombBox->AddString(_T("ID3DXFont会把生成的图形数据以字为单位保存到若干张纹理中"));
		pCombBox->AddString(_T("用DirectX SDK自带的工具PIX for Windows可以很容易找到性能不够理想的原因"));
		pCombBox->AddString(_T("应用的调用频率都很低"));
		pCombBox->AddString(_T("使用GDI/GDI+提供的接口"));
		pCombBox->AddString(_T("游戏的窗口是时时刻刻在重绘的"));
		pCombBox->AddString(_T("当你调用了DrawText或TextOut等方法绘制文字"));
		pCombBox->AddString(_T("ID3DXFont会把生成的图形数据以字为单位保存到若干张纹理中"));
		pCombBox->AddString(_T("用DirectX SDK自带的工具PIX for Windows可以很容易找到性能不够理想的原因"));
	}
	btn.SetStyle( btn.GetStyle()|XWS_ALIGNCENTER );
	btn.SetRect( RECT_C( 0,0,20,20 ) );
	img.hImage = CreateImage( XT2W( strImagePath+_T("c_n.png") ) );
	btn.SetNormalImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("c_h.png") ) );
	btn.SetMouseOverImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("c_p.png") ) );
	btn.SetPressedImage( img );
	btn.SetID( ID_CLOSE );
	m_head.AddXCtrl( &btn );

	img.hImage = CreateImage( XT2W( strImagePath+_T("mx_n.png") ) );
	btn.SetNormalImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("mx_h.png") ) );
	btn.SetMouseOverImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("mx_p.png") ) );
	btn.SetPressedImage( img );
	btn.SetID( ID_MAX );
	m_head.AddXCtrl( &btn );

	img.hImage = CreateImage( XT2W( strImagePath+_T("restore_n.png") ) );
	btn.SetNormalImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("restore_h.png") ) );
	btn.SetMouseOverImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("restore_p.png") ) );
	btn.SetPressedImage( img );
	btn.SetID( ID_RESTORE );
	btn.Show( false );
	m_head.AddXCtrl( &btn );
	btn.Show( true );

	img.hImage = CreateImage( XT2W( strImagePath+_T("mm_n.png") ) );
	btn.SetNormalImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("mm_h.png") ) );
	btn.SetMouseOverImage( img );
	img.hImage = CreateImage( XT2W( strImagePath+_T("mm_p.png") ) );
	btn.SetPressedImage( img );
	btn.SetID( ID_MINI );
	m_head.AddXCtrl( &btn );

	img.hImage =hNormal;
	btn.SetNormalImage( img );
	img.hImage = hOver;
	btn.SetMouseOverImage( img );
	img.hImage = hPressed;
	btn.SetPressedImage( img );
	btn.SetFont( m_normalFont );
	btn.SetStyle( XWS_SINGLELINE|XWS_ENDELLIPSE|XWS_ALIGNCENTER );
	btn.SetRect( RECT_C( 10, 25, 92, 45 ) );
	btn.SetID( ID_SKIN );
	btn.SetText( _T("换肤") );
	m_head.AddXCtrl( &btn );

	CUICheckBox checkBox;
	checkBox.Create( _T("checkBox"), RECT_C( 20, 2, 200, 18 ), IDC_ATC_CHECKBOX );
	checkBox.SetFont( m_normalFont );
	{
		IMG img1;
		img.margin.left = img.margin.right=img.margin.top=img.margin.bottom = 0;
		img.hImage = CreateImage( ".\\image\\nec_uu_1.png" );
		img1.hImage = CreateImage( _T(".\\image\\nec_u_1.png") );
		checkBox.SetNormalImage( img, img1 );
		img.hImage = CreateImage( ".\\image\\nec_uu_2.png" );
		img1.hImage = CreateImage( _T(".\\image\\nec_u_2.png") );
		checkBox.SetMouseOverImage( img, img1 );
		checkBox.SetTextColor( Color(254,255,255,255).GetValue() );
	}
	checkBox.SetStyle( XWS_ALIGNLEFT|XWS_ALIGNVCENTER );
	checkBox.SetID( IDC_ALPHA );
	checkBox.SetText( _T("掩码窗") );
	m_head.AddXCtrl( &checkBox );
	m_comboDropView.AddXCtrl( &checkBox );
	checkBox.SetStyle( XWS_ALIGNLEFT|XWS_ALIGNVCENTER );
	checkBox.SetID( IDC_SHOWTAB );
	checkBox.SetText( _T("显示TAB") );
	checkBox.SetCheck( true );
	checkBox.SetRect( RECT_C( 20, 20, 200, 38 ) );
	m_head.AddXCtrl( &checkBox );
	m_comboDropView.AddXCtrl( &checkBox );
	checkBox.SetID( IDC_PSD );
	checkBox.SetText( _T("密码") );
	checkBox.SetCheck( false );
	checkBox.SetRect( RECT_C( 20, 40, 200, 58 ) );
	m_head.AddXCtrl( &checkBox );
	m_comboDropView.AddXCtrl( &checkBox );
	//margin.left = margin.top=margin.right=margin.bottom= 10;
	//btn.SetMargin( margin );
	
	img.margin.left = img.margin.right = 10;
	img.margin.top = img.margin.bottom = 5;
	img.hImage =hNormal;
	m_tab.SetNormalImage( img );
	img.hImage = hOver;
	m_tab.SetMouseOverImage( img );
	img.hImage = hPressed;
	m_tab.SetPressedImage( img );
	CUIText text;
	text.Create( _T(""), RECT_C( 0, 0, 1, 1 ), -1 );
	text.SetFont( m_normalFont );
	text.SetTextColor( Color( 254, 0, 0, 0 ).GetValue() );
	text.SetStyle( XWS_SINGLELINE|XWS_ENDELLIPSE|XWS_ALIGNCENTER );
	text.SetRect( RECT_C( 0, 0, 82, 30 ) );
	text.SetID( ID_LIST );
	text.SetText( _T("列表控件") );
	text.HasNotify( false );
	CUITab::LPTABITEM pItem = m_tab.InsertItem( RECT_C( 10, 0, 92, 30 ), ID_LIST );
	pItem->AddXCtrl( &text );
	
	text.SetRect( RECT_C( 0, 0, 179-97, 30 ) );
	text.SetID( ID_AUTOLIST );
	text.SetText( _T("自适应列表控件") );
	pItem = m_tab.InsertItem( RECT_C( 97, 0, 179, 30 ), ID_AUTOLIST );
	pItem->AddXCtrl( &text );

	text.SetRect( RECT_C( 0, 0, 266-184, 30 ) );
	text.SetID( ID_VLLIST);
	text.SetText( _T("虚拟列表控件") );
	pItem = m_tab.InsertItem( RECT_C( 184, 0, 266, 30 ), ID_VLLIST );
	pItem->AddXCtrl( &text );

	text.SetRect( RECT_C( 0, 0, 353-271, 30 ) );
	text.SetID( ID_TREE );
	text.SetText( _T("树形控件") );
	pItem = m_tab.InsertItem( RECT_C( 271, 0, 353, 30 ), ID_TREE );
	pItem->AddXCtrl( &text );
	m_tab.EnableAutoMode( true );
	m_tab.Show(true);
}

void CRListDlg::InitTree()
{
	SetRedraw( FALSE );
	IMG img;
	img.margin.left = img.margin.top=img.margin.right=img.margin.bottom= 3;
	m_tree.SetExpandAttrib( 3, 4 );
	CUITreeCtrl::NODEDATA row;
	CUITreeCtrl::HNODE hEnum;
	row.Show( true );
	CUITreeCtrl::HNODE hNode = NULL;
	for ( int _j = 0; _j < 300; _j++ )
	{
		INFO chatInfo;
		chatInfo.strContent = _T("SKSFDLKJ看三色堇反对党搜索速度非凡方法诉讼似懂非懂萨电力网。");
		chatInfo.strTime = _T("2010-10-01 19:20");
		chatInfo.strSendName = _T("发送者");
		chatInfo.strSendName += (char)(48+_j/100);
		chatInfo.strSendName += (char)(48+(_j%100)/10);
		chatInfo.strSendName += (char)(48+_j%10);
		chatInfo.strSendName += _T("1234567890");
		chatInfo.strRecvName = _T("接收者");
		chatInfo.strReadFlg = _T("1");
		chatInfo.bRecieved	= (_j%2 == 0);
		_TranslateFromReocord( (LPVOID)&chatInfo, row );
		row.SetMargin( 2 );
 		if ( NULL != hNode && _j%50 )
 		{
 			row.SetRect( RECT_C( 0,0,500,50 ) );
			row.SetData( _j );
			m_tree.Insert( row, NULL, hNode );
 		}
 		else
		{
			CUITreeCtrl::NODEDATA row;
			row.Show(true);
			CUIText text;
			tstring strName = _T("组节点");
			strName += (char)(48+_j/100);
			text.Create( strName, RECT_C( 0,0,100,100 ),IDC_ATC_NAME );
			text.SetFont( m_normalFont );
			text.SetTextColor( Color(254,0,0,0).GetValue() );
			text.HasNotify( false );
			text.SetStyle( XWS_SINGLELINE );
			SIZE size = IUICtrl::TestTextSize( text );
			text.SetRect( RECT_C( 24, 10, 24+size.cx, 10+size.cy ) );
			row.SetRect( RECT_C( 0, 0, 500, size.cy+20 ) );
			row.AddXCtrl( &text );
			CUIImage image;
			image.Create( _T(""), RECT_C(  10, (size.cy+15)/2, 19, (size.cy+15)/2+5  ), 3 );
			tstring strImagePath = _T(".\\image\\");
			img.hImage = CreateImage( XT2W( strImagePath+_T("lan_xl_2.png") ) );
			image.SetImage( img );
			row.AddXCtrl( &image );
			image.SetID( 4 );
			image.SetRect( RECT_C(  12, (size.cy+11)/2, 17, (size.cy+11)/2+9  ) );
			img.hImage = CreateImage( XT2W( strImagePath+_T("lan_xl_1.png") ) );
			image.SetImage( img );
			image.Show(false);
			row.AddXCtrl( &image );
			row.SetData( _j );
			hNode = m_tree.Insert( row, NULL, NULL );
			hNode->m_bExpand = true;
		}
	}
	hNode = m_tree.GetRoot();
	while( hNode )
	{
		m_tree.Enum( m_tree.GetFirstChild( hNode ), CRListDlg::EnumTree, 0 );
		hNode = m_tree.GetNextBrother( hNode );
	}
	SetRedraw( TRUE );
	InvalidateRect( GetHandle(), NULL, FALSE );
}