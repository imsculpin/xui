// RListDlg.h : 头文件
//

#pragma once
#include "UICtrl.h"
#include <string>
using namespace std;

struct INFO
{
	tstring		strMsgID ;								//消息ID
	tstring		strReadFlg;								//消息的标识，已读未读
	tstring		strSendName;							//发送者的昵称
	tstring		strRecvName;							//接收者的昵称
	tstring		strTime ;								//聊天时间
	tstring		strContent ;							//内容，该内容包括图片、文字、表情等信息。
	bool		bRecieved;
};

// CRListDlg 对话框
class CRListDlg : public CUIWindow
{
// 构造
public:
	CRListDlg();	// 标准构造函数
	~CRListDlg();

#define  CAT( sz, i ) sz#i

	void Initlist()
	{

 		for ( int _j = 0; _j < 100; _j++ )
 		{
 			CUIListCtrl::ROW row;
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
 			m_dbase.Add( -1, chatInfo );
 			m_list.InsertRow(  row );
 		}
 		m_list.InvalidRect(  );
 		m_dbase.Reflash();

		for ( int _j = 0; _j < 5; _j++ )
		{
			CUIListCtrl::ROW row;
			INFO chatInfo;
			if( 1 == _j%2 )
			{
				chatInfo.strContent = _T("在这个世界上有一种东西叫做忍，在这个世界上有一种事物叫做情，你情我愿！在这个世界上有一种东西叫做忍，在这个世界上有一种事物叫做情，你情我愿！在这个世界上有一种东西叫做忍，在这个世界上有一种事物叫做情，你情我愿！在这个世界上有一种东西叫做忍，在这个世界上有一种事物叫做情，你情我愿！在这个世界上有一种东西叫做忍，在这个世界上有一种事物叫做情，你情我愿！");
			}
			else
			{
				chatInfo.strContent = _T("这货又在发牢骚！");
			}

			chatInfo.strTime = _T("2010-10-01 19:20");
			chatInfo.strSendName = _T("发送者");
			chatInfo.strSendName += (char)(48+_j/100);
			chatInfo.strSendName += (char)(48+(_j%100)/10);
			chatInfo.strSendName += (char)(48+_j%10);
			chatInfo.strRecvName = _T("接收者");
			chatInfo.strReadFlg = _T("1");
			chatInfo.bRecieved	= (_j%2 == 0);
			_TranslateFromReocord1( (LPVOID)&chatInfo, row );
			row.SetMargin( 2 );
			m_autolist.InsertRow(  row );
		}
		m_autolist.InvalidRect(NULL);
		return;
	}

	void InitTree();

	void InitHead(  );
// 实现
protected:
	HICON  m_hIcon;
	static void EnumTree( CUITreeCtrl::NODE& node, DWORD dwParam )
	{
// 		TCHAR szNum[10] = {0};
// 		wsprintf( szNum, _T("%d\n"), node.m_nodeData.GetData() );
// 		OutputDebugString( szNum );
	}
	LRESULT WinProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if ( WM_ACTIVATEAPP == message )
		{
			if ( wParam )
			{
				OutputDebugString( _T("ShowWindow\n") );
			}
		}
		
		return __super::WinProc( message, wParam, lParam );
	}

	static BOOL XMessageReciever( UINT message, WPARAM wParam, LPARAM lParam, DWORD dwData );
	// 生成的消息映射函数
	 virtual BOOL OnInitWnd();
	static BOOL _TranslateFromReocord( LPVOID pRecord, CUIListCtrl::ROW& rowData );
	static BOOL _TranslateFromReocord1( LPVOID pRecord, CUIListCtrl::ROW& rowData );

	BOOL _LoadImages();
	void OnSize( UINT nType, int cx, int cy )
	{
		if ( m_hWnd
			&& m_tree.GetUIWindow()
			&& m_tree.GetUIWindow()->GetHandle()
			/*&& pRect*/ )
		{
			m_head.SetRect( RECT_C( 2, 0, cx-2, 52 ) );
			m_tab.SetRect( RECT_C( 2, 52, cx-2, 82 ) );
			int top = 52;
			if ( m_tab.IsVisable() )
			{
				top = 82;
			}
			m_vlist.SetRect( RECT_C( 2, top, cx-2, cy- 10  ));
			m_list.SetRect( RECT_C( 2, top, cx-2, cy- 10  ));
			m_autolist.SetRect( RECT_C( 2, top, cx-2, cy- 10 ) );
			m_tree.SetRect( RECT_C( 2, top, cx-2, cy- 10  ));
			m_viewMark.SetRect( RECT_C( 2, top, cx-2, cy-3 ) );
		}
	}
	CUIListCtrl       m_vlist;
	CUIListCtrl		 m_list;
	CUIListCtrl		 m_autolist;
	CUITreeCtrl		 m_tree;
	CUIView			 m_viewMark;
	CUITab			 m_tab;
	CUIView			 m_head;
	CUIView			 m_comboDropView;
	CDBase<INFO>     m_dbase;
	/*!> 位图在缓存中的位置 */
	enum{ BMP_PHONE_NORMAL = 0, BMP_PHONE_HOT, BMP_PHONE_PRESSED, BMP_SMS_NORMAL, BMP_SMS_HOT, BMP_SMS_PRESSED,\
		BMP_SEND, BMP_RECIEVED, BMP_ROWHOT, BMP_ROWSEL, BMP_OFFLINE, BMP_ONLINE,BMP_L1,BMP_L2,BMP_L3,BMP_R1,BMP_R2,BMP_R3, BMP_PAD, BMP_CAR, BMP_END };
	static HIMAGE	  m_RLBmpArray[BMP_END];   //位图缓存
	static HFONT	  m_normalFont;
	HRGN		      m_hRgn;
	CTray			  m_tray;
	CTray			  m_tray1;
	IMG					  m_MenuBk;
	IMG					  m_subMenuBk;
	IMG					  m_menuSeparator;
};
