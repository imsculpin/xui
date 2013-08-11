// UIWnd.cpp : 实现文件
#include "UIComboBox.h"
#include ".\UIComboBox.h"
#include <WindowsX.h>
#include <algorithm>
#include <set>
#include "UIView.h"
#include "UIMenu.h"
#include "UIWindow.h"
#include "UIListCtrl.h"
using std::set;

map<HWND, CUIComboBox::EDITHELP> CUIComboBox::m_mapUIEdits;

CUIComboBox::CUIComboBox( bool bDefaultSkin )
:m_hEdit( NULL )
,m_bEnable( true )
,m_bReadOnly(false)
,m_pListWindow(NULL)
,m_pListView(NULL)
,m_pListViewDefault(NULL)
,m_nDropViewMaxHeight(100)
,m_nDropViewMaxWidth(0)
,m_nOffsetX(0)
,m_nOffsetY(0)
,m_clrLBText(0xFE000000)
,m_bDefaultSkin( bDefaultSkin )
{
	m_sizeArrow.cx = 16;
	m_sizeArrow.cy = 16;
	SetStyle( GetStyle()| XWS_SINGLELINE | XWS_ALIGNVCENTER );
}

CUIComboBox& CUIComboBox::operator=( const CUIComboBox& uObj )
{
	if ( this == &uObj )
	{
		return *this;
	}
	CUIText::operator =(uObj);
	m_bEnable = uObj.m_bEnable;
	m_hCursor = uObj.m_hCursor;
	m_imageNormal = uObj.m_imageNormal;
	m_imageOver = uObj.m_imageOver;
	m_imageFocus = uObj.m_imageFocus;
	m_imageDisable = uObj.m_imageDisable;

	m_imageArrowNormal = uObj.m_imageArrowNormal;
	m_imageArrowOver = uObj.m_imageArrowOver;
	m_imageArrowFocus = uObj.m_imageArrowFocus;
	m_imageArrowDisable = uObj.m_imageArrowDisable;
	m_sizeArrow = uObj.m_sizeArrow;
	m_margin = uObj.m_margin;
	m_nDropViewMaxHeight = uObj.m_nDropViewMaxHeight;
	m_nDropViewMaxWidth = uObj.m_nDropViewMaxWidth;
	m_nOffsetX = uObj.m_nOffsetX;
	m_nOffsetY = uObj.m_nOffsetY;
	m_clrLBText = uObj.m_clrLBText;
	m_hEdit = NULL;
	m_pListView = NULL;
	m_pListViewDefault = NULL;
	m_bDefaultSkin = uObj.m_bDefaultSkin;
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
	return *this;
}

CUIComboBox::CUIComboBox( const CUIComboBox& uObj ) :CUIText( uObj )
,m_bEnable( uObj.m_bEnable )
,m_hCursor( uObj.m_hCursor )
,m_imageNormal(uObj.m_imageNormal)
,m_imageOver(uObj.m_imageOver)
,m_imageFocus( uObj.m_imageFocus )
,m_margin(uObj.m_margin)
,m_bReadOnly(uObj.m_bReadOnly)
,m_hEdit( NULL )
,m_pListView(NULL)
,m_pListWindow(NULL)
,m_pListViewDefault(NULL)
,m_imageArrowNormal(uObj.m_imageArrowNormal)
,m_imageArrowOver(uObj.m_imageArrowOver)
,m_imageArrowFocus(uObj.m_imageArrowFocus)
,m_imageArrowDisable(uObj.m_imageArrowDisable)
,m_sizeArrow(uObj.m_sizeArrow)
,m_nDropViewMaxHeight(uObj.m_nDropViewMaxHeight)
,m_nDropViewMaxWidth(uObj.m_nDropViewMaxWidth)
,m_nOffsetX(uObj.m_nOffsetX)
,m_nOffsetY(uObj.m_nOffsetY)
,m_clrLBText(uObj.m_clrLBText)
,m_bDefaultSkin(uObj.m_bDefaultSkin)
{
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
}

INT32 CUIComboBox::Create( tstring strText, const RECT& rect, \
						  INT32 nCommandId /*= UNIT_TEXT_ID */ )
{
	assert( XW_NULL == m_nClass );
	__super::Create( strText, rect, nCommandId );
	m_nClass = XW_COMBO;
	int yMax = RECT_H(m_rcXCtrl)-m_margin.top-m_margin.bottom;
	if ( m_sizeArrow.cy > yMax )
	{
		m_sizeArrow.cy = yMax;
		m_sizeArrow.cx = yMax;
	}
	return RL_OK;
}


void CUIComboBox::OnDraw(  IPainter* painter  )
{
	assert( XW_NULL != m_nClass );
	if ( !IsVisable() )
	{
		return;
	}

	IMG* pImageShow = NULL;
	IMG* pArrow = NULL;
	ARGB clrMask = -1;
	RECT rcBtn;
	GetRect( rcBtn );	
	if ( RECT_ISEMPTY(rcBtn)
		|| !painter->RectVisible( &rcBtn )
		)
	{
		return;
	}
	if ( m_bEnable )
	{
		if ( !::UIIsFocusIn(this) )
		{
			//普通状态
			if ( !IsMouseIn() )
			{
				pImageShow = &m_imageNormal;
				pArrow = &m_imageArrowNormal;
			}
			//鼠标经过
			else
			{
				pImageShow = &m_imageOver;
				pArrow = &m_imageArrowOver;
			}
		}
		else
		{
			pImageShow = &m_imageFocus;
			pArrow = &m_imageArrowFocus;
		}

		if( NULL == pImageShow
			|| ( NULL != pImageShow
			&& !IsImageValid( pImageShow ) )
			)
		{
			if ( IsImageValid( &m_imageNormal ) )
			{
				pImageShow = &m_imageNormal;
			}
			else
			{
				pImageShow = NULL;
			}
		}
		if( NULL == pArrow
			|| ( NULL != pArrow
			&& !IsImageValid( pArrow ) )
			)
		{
			if ( IsImageValid( &m_imageArrowNormal ) )
			{
				pArrow = &m_imageArrowNormal;
			}
			else
			{
				pArrow = NULL;
			}
		}
	}
	else
	{
		pImageShow = IsImageValid(&m_imageDisable)?&m_imageDisable:&m_imageNormal;
		pArrow = IsImageValid(&m_imageArrowDisable)?&m_imageArrowDisable:&m_imageArrowNormal;
	}
	if ( pImageShow )
	{ 
		painter->DrawImage( pImageShow, &rcBtn );
	}
	if ( pArrow 
		&& m_sizeArrow.cx > 0
		&& m_sizeArrow.cy > 0 )
	{
		RECT rcArrow = rcBtn;
		rcArrow.top += ((RECT_H(rcBtn)-m_sizeArrow.cy)>>2)+((RECT_H(rcBtn)-m_sizeArrow.cy)%2);
		rcArrow.left = rcBtn.right - m_margin.right - m_sizeArrow.cx;
		rcArrow.right = rcArrow.left+m_sizeArrow.cx;
		rcArrow.bottom = rcArrow.top+m_sizeArrow.cy;
		painter->DrawImage( pArrow, &rcArrow );
	}
	if ( !m_strText.empty() )
	{
		RECT_INFLATE( rcBtn, -m_margin.left, -m_margin.top, -(m_margin.right+m_sizeArrow.cx), -m_margin.bottom );
		if ( m_bEnableOutGlaw )
		{
			//DrawOuterGlow( _hDC, &rcBtn, m_strText.c_str(), m_hFont, 0, 0  );
		}
		ARGB clrOld = painter->SetBrushColor( IsMouseIn()?GetHotTextColor():GetTextColor() );
		painter->DrawString( m_strText.c_str(), -1, &rcBtn, GetFont(), m_nStyle );
		painter->SetBrushColor( clrOld );
	}
}

BOOL CUIComboBox::CursorIn( POINT point  )
{
	if ( !IsVisable() )
	{
		return FALSE;
	}
	if( m_bEnable && !m_bReadOnly )
	{
		SetCursor(LoadCursor( NULL, IDC_IBEAM ));
		return TRUE;
	}
	if ( m_hCursor && m_bEnable )
	{
		::SetCursor( m_hCursor );
		return TRUE;
	}
	return FALSE;
}

BOOL CUIComboBox::MouseLBUp( POINT pt )
{
	BOOL bRet = __super::MouseLBUp( pt );
	_Update();
	return bRet;
}

void CUIComboBox::MouseLeave()
{
	__super::MouseLeave();
	_Update();
}

BOOL CUIComboBox::MouseLBDown( POINT pt )
{
	BOOL bRet = __super::MouseLBDown( pt );
	if ( m_bEnable )
	{
		_Update();
		//DropView;
		DropView( m_pListWindow==NULL || (m_pListWindow!=NULL&&!IsWindowVisible( m_pListWindow->GetHandle() )) );
	}
	return bRet;
}

BOOL CUIComboBox::MouseLDBClick( POINT pt )
{
	BOOL bRet = __super::MouseLDBClick( pt );
	if ( m_bEnable )
	{
		_Update();
		//DropView;
		DropView( m_pListWindow==NULL || (m_pListWindow!=NULL&&!IsWindowVisible( m_pListWindow->GetHandle() )) );
	}
	return bRet;
}

BOOL CUIComboBox::MouseRBDown( POINT pt )
{
	BOOL bRet = __super::MouseRBDown( pt );
	if( m_bEnable
		&& !m_bReadOnly
		&& UIGetFocus() != this )
	{
		UISetFocus( this );
		FocusIn(pt);
		_Update();
		ClientToScreen( GetUIWindow()->GetHandle(), &pt );
		ScreenToClient( m_hEdit, &pt );
		SendMessage( m_hEdit, WM_RBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y) );
	}
	return bRet;
}

BOOL CUIComboBox::FocusIn( POINT point )
{
	if( __super::FocusIn( point ) 
		&& m_bEnable 
		&& !m_bReadOnly )
	{
		RECT rcThis;
		GetRect( rcThis );
		RECT_INFLATE( rcThis, -m_margin.left, -m_margin.top, -(m_margin.right+m_sizeArrow.cx), -m_margin.bottom );
		if( NULL == m_hEdit )
		{	
			WNDCLASS wndcls;
			if (!::GetClassInfo(NULL, _T("X_EDIT"), &wndcls))
			{
				::GetClassInfo(NULL, _T("EDIT"), &wndcls);
				wndcls.lpszClassName    = _T("X_EDIT");
				RegisterClass(&wndcls);
			}
			m_hEdit = CreateWindowEx( 0, _T("EDIT"), _T(""), WS_CHILD|ES_AUTOHSCROLL, \
				rcThis.left,rcThis.top,RECT_W(rcThis),\
				RECT_H(rcThis), GetUIWindow()->GetHandle(), NULL, GetModuleHandle(0), NULL );
			SetWindowPos( m_hEdit, HWND_TOP, 0,0,0,0, \
				SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
			assert( m_hEdit );
			EDITHELP edithelp = {
				this,
				(WNDPROC)(LONG_PTR)SetWindowLong( m_hEdit, GWL_WNDPROC, (LONG)(LONG_PTR)CUIComboBox::WinProcess )
			};
			m_mapUIEdits.insert( make_pair( m_hEdit, edithelp ) );
			HFONT hFont = GetFont();
			SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)(hFont?hFont:UIGetDefaultFont()), (LPARAM)(BOOL)(FALSE));
		}
		else
		{
			SetWindowPos( m_hEdit, HWND_TOP, rcThis.left,rcThis.top,RECT_W(rcThis),\
				RECT_H(rcThis), SWP_NOACTIVATE );
		}
		if ( !IsWindowVisible(m_hEdit) )
		{
			SetWindowPos( m_hEdit, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW );
			SetWindowText( m_hEdit, m_strText.c_str() );
			m_strText.clear();
			m_strTextBase.clear();
			SetFocus( m_hEdit );
			::SendMessage(m_hEdit,WM_KEYDOWN,VK_END,0); 
			InvalidRect();
			Update();
		}
	}
	return FALSE;
}

BOOL CUIComboBox::FocusLeave()
{
	if( __super::FocusLeave() )
	{
 		if ( ::GetFocus() == m_pListWindow->GetHandle() )
 		{
			UISetFocus( this );
 			return TRUE;
 		}
		
		if ( !m_bReadOnly
			&& m_bEnable
			&& m_hEdit )
		{
			ShowWindow( m_hEdit, SW_HIDE );
			int nBuffer = ::GetWindowTextLength( m_hEdit );
			if( nBuffer > 0 )
			{
				++nBuffer;
				try
				{
					TCHAR* pBuffer = new TCHAR[nBuffer];
					memset( pBuffer, 0, (nBuffer)*sizeof(TCHAR) );
					GetWindowText( m_hEdit, pBuffer, nBuffer );
					m_strText = pBuffer;
					delete pBuffer;
				}
				catch( ... )
				{

				}
				m_strTextBase = m_strText;	
			}	
		}
		m_strbkup.clear();
		DropView( false );
		InvalidRect();
		return TRUE;
	}
	return FALSE;
}

void CUIComboBox::SetParent( IUIBase* parent )
{
	__super::SetParent( parent );
	if( GetUIWindow() 
		&& GetUIWindow()->GetHandle() )
	{
		if( m_hEdit && IsWindow( m_hEdit ) )
		{
			if ( !m_bReadOnly
				&& m_bEnable )
			{
				::SetParent( m_hEdit, GetUIWindow()->GetHandle() );
			}
			else
			{
				DestroyWindow( m_hEdit );
				m_hEdit = NULL;
			}
		}
	}
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
}

LRESULT WINAPI CUIComboBox::WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( m_mapUIEdits.empty() )
	{
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	map<HWND, EDITHELP>::iterator _lab = m_mapUIEdits.find( hWnd );
	if ( _lab != m_mapUIEdits.end()
		&& _lab->second.orgProc )
	{
		if ( message == WM_KEYDOWN
			&& (
			wParam == VK_RETURN
			||wParam == VK_ESCAPE
			||wParam == VK_UP 
			|| wParam == VK_DOWN ))
		{
			_lab->second.pEdit->KeyDown( wParam, lParam );
			return S_OK;
		}
		if ( WM_RBUTTONDOWN == message
			|| WM_CONTEXTMENU == message )
		{
			POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if ( WM_CONTEXTMENU == message )
			{
				GetCaretPos( &point );
				ClientToScreen( _lab->second.pEdit->m_hEdit, &point );
			}
			else
			{
				ClientToScreen( hWnd, &point );
			}
			NM_PARAM param = {0};
			param.point = point;
			param.nCtrlId = _lab->second.pEdit->GetID();
			if ( _lab->second.pEdit->m_parent )
			{
				param.nParentId = _lab->second.pEdit->m_parent->GetID();
			}
			//如果第三方已处理, 则直接返回
			if ( _lab->second.pEdit->Notify( XWM_MENU, 0, (LPARAM)(LONG_PTR)&param ) )
			{
				return S_OK;
			}
			//组件内部处理
			CUIMenu menu;
			ACCEL accel;
			accel.fVirt = FVIRTKEY;
			accel.cmd = WM_CUT;
			accel.key = _T('T');
			menu.AddAccel( accel );
			accel.cmd = WM_COPY;
			accel.key = _T('Y');
			menu.AddAccel( accel );
			accel.cmd = WM_PASTE;
			accel.key = _T('P');
			menu.AddAccel( accel );
			accel.cmd = WM_CUT-1;
			accel.key = _T('A');
			menu.AddAccel( accel );
			int nStartChar = 0;
			int nEndChar = 0;
			BOOL bTextCliped = FALSE;
			if( OpenClipboard( _lab->second.pEdit->m_hEdit ) )
			{
				HANDLE handle = GetClipboardData(CF_TEXT);
				bTextCliped = (INVALID_HANDLE_VALUE!=handle)&&(NULL!=handle);
				CloseClipboard();
			}

			::SendMessage( _lab->second.pEdit->m_hEdit, EM_GETSEL, (WPARAM)&nStartChar,(LPARAM)&nEndChar );
			menu.InsertMenu( _T("剪切(&T)"), NULL, WM_CUT, nStartChar != nEndChar );
			menu.InsertMenu( _T("复制(&Y)"), NULL, WM_COPY, nStartChar != nEndChar );
			menu.InsertMenu( _T("粘贴(&P)"), NULL, WM_PASTE, bTextCliped );
			menu.InsertMenu( _T("全部选择(&A)"), NULL, WM_CUT-1 );
			UINT nCmd = menu.TrackPopupMenu( point.x, point.y, 0 );
			switch( nCmd )
			{
			case WM_CUT-1:
				{
					SendMessage( _lab->second.pEdit->m_hEdit, EM_SETSEL, 0, -1 );
				}
				break;
			case WM_CUT:
				{
					SendMessage(_lab->second.pEdit->m_hEdit, WM_CUT, 0, 0);
				}
				break;
			case WM_COPY:
				{
					SendMessage(_lab->second.pEdit->m_hEdit, WM_COPY, 0, 0);
				}
				break;
			case WM_PASTE:
				{
					SendMessage(_lab->second.pEdit->m_hEdit, WM_PASTE, 0, 0);
				}
				break;
			default:
				break;
			}
			return S_OK;
		}
		return CallWindowProc( _lab->second.orgProc, hWnd, message, wParam, lParam );
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

void CUIComboBox::SetRect( const RECT& rcSet )
{
	__super::SetRect( rcSet );
	if ( m_hEdit )
	{
		RECT rcThis;
		GetRect( rcThis );
		RECT_INFLATE(rcThis, -m_margin.left, -m_margin.top, -(m_margin.right+m_sizeArrow.cx), -m_margin.bottom );
		SetWindowPos( m_hEdit, NULL, rcThis.left, rcThis.top,RECT_W(rcThis), RECT_H(rcThis), SWP_NOACTIVATE|SWP_NOZORDER);
	}
	int yMax = RECT_H(m_rcXCtrl)-m_margin.top-m_margin.bottom;
	if ( m_sizeArrow.cy > yMax )
	{
		m_sizeArrow.cy = yMax;
	}
}

void CUIComboBox::SetFont( HFONT pFont )
{
	__super::SetFont( pFont );
	m_hFont = pFont;
	if ( m_hEdit )
	{
		HFONT hFont = GetFont();
		SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)(hFont?hFont:UIGetDefaultFont()), (LPARAM)(BOOL)(FALSE));
	}
}

BOOL CUIComboBox::DropView( bool bDrop /*= true */)
{
	if ( !GetUIWindow()
		|| (GetUIWindow()
		&& !GetUIWindow()->GetHandle())
		)
	{
		return FALSE;
	}
	if ( !bDrop )
	{
		if ( m_pListWindow&&m_pListWindow->GetHandle() )
		{
			SetWindowPos( m_pListWindow->GetHandle(), NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_HIDEWINDOW);
		}
		if ( m_hEdit
			&& IsWindowVisible(m_hEdit))
		{
			SetFocus( m_hEdit );
			::SendMessage(m_hEdit,WM_KEYDOWN,VK_END,0); 
		}
		InvalidRect(  );
		Update();
		return TRUE;
	}
	CheckAndGetView();
	SIZE sizeView = {0};
	if ( m_pListView )
	{
		sizeView = m_pListView->GetSize( );
	}
	if ( sizeView.cx <= 0
		|| sizeView.cy <= 0 )
	{
		return TRUE;
	}
	if ( NULL == m_pListWindow )
	{
		try
		{
			m_pListWindow = new CUIWindow;
		}
		catch( ... )
		{
			return FALSE;
		}
		if ( m_pListWindow )
		{
			RECT rect = {0,0,0,0};
			if( m_pListWindow->Create( rect, WS_POPUP, WS_EX_TOOLWINDOW, GetUIWindow()->GetHandle() ) )
			{
				m_pListWindow->SetBkGnd( m_imageDropBkGnd, false );
				m_pListView->Attach( m_pListWindow );
				EDITHELP edithelp = {
					this,
					(WNDPROC)(LONG_PTR)SetWindowLong( m_pListWindow->GetHandle(), GWL_WNDPROC, (LONG)(LONG_PTR)CUIComboBox::DropViewWindowProcess )
				};
				m_mapUIEdits.insert( make_pair( m_pListWindow->GetHandle(), edithelp ) );
			}
		}
	}

	RECT rcThis = {0};
	if ( NULL == m_pListWindow
		|| ( m_pListWindow
		&& !m_pListWindow->GetHandle() )
		)
	{
		return FALSE;
	}	
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		((CUIListCtrl*)m_pListView)->SetCurSel( -1 );
	}

	GetRect( rcThis );
	rcThis.top = rcThis.bottom;
	::ClientToScreen( GetUIWindow()->GetHandle(), (LPPOINT)&rcThis );
	rcThis.right = rcThis.left+sizeView.cx+m_marginDrop.left+m_marginDrop.right;
	rcThis.bottom = rcThis.top + sizeView.cy+m_marginDrop.top+m_marginDrop.bottom;
	SetWindowPos( m_pListWindow->GetHandle(), HWND_TOP, rcThis.left, rcThis.top,\
		RECT_W(rcThis), RECT_H(rcThis), SWP_NOACTIVATE|SWP_SHOWWINDOW);

	return TRUE;
}

CUIComboBox::~CUIComboBox()
{
	if( m_hEdit )
	{
		if ( m_mapUIEdits.size() 
			&& m_hEdit )
		{
			m_mapUIEdits.erase( m_hEdit );
		}
		DestroyWindow( m_hEdit );
		m_hEdit = NULL;
		
	}
	if ( m_pListView
		&& m_pListView != m_pListViewDefault )
	{
		m_pListView->UnhookNotify( CUIComboBox::DropViewMessage );
		m_pListView->Detach();
	}
	m_pListView = NULL;
	if ( m_pListViewDefault )
	{
		m_pListViewDefault->Detach();
		delete m_pListViewDefault;
		m_pListViewDefault = NULL;
	}
	if ( m_pListWindow )
	{
		if ( m_mapUIEdits.size() 
			&& m_pListWindow->GetHandle() )
		{
			m_mapUIEdits.erase(m_pListWindow->GetHandle());
		}
		delete m_pListWindow;
		m_pListWindow = NULL;
	}
}

void CUIComboBox::SetDropView( CUIView* pView )
{
	if ( !pView )
	{
		return;
	}
	if ( m_pListView )
	{
		m_pListView->Detach();
	}
	m_pListView = pView;
	if ( m_pListWindow )
	{
		m_pListView->Attach( m_pListWindow );
		m_pListView->HookNotify( CUIComboBox::DropViewMessage, (DWORD)(DWORD_PTR)this );
	}
}

int CUIComboBox::GetLBCount() const
{
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		return ((CUIListCtrl*)m_pListView)->Size();
	}
	return 0;
}

int CUIComboBox::GetLBCurSel() const
{
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		return ((CUIListCtrl*)m_pListView)->GetCurSel();
	}
	return 0;
}

BOOL CUIComboBox::SetLBCurSel( int nSelect )
{
	CheckAndGetView();
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		return ((CUIListCtrl*)m_pListView)->SetCurSel( nSelect );
	}
	assert( false );
	return FALSE;
}

DWORD CUIComboBox::GetLBItemData( int nIndex ) const
{
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		return ((CUIListCtrl*)m_pListView)->GetRowData( nIndex );
	}
	assert( false );
	return 0;
}

void CUIComboBox::SetLBItemData( int nIndex, DWORD dwItemData )
{
	CheckAndGetView();
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		((CUIListCtrl*)m_pListView)->SetRowData( nIndex, dwItemData );
	}
	assert( false );
	return;
}

int CUIComboBox::GetLBText( int nIndex, tstring& strText ) const
{
	// 		if ( m_pListView
	// 			&& m_pListView == m_pListViewDefault )
	// 		{
	// 			return ((CUIListCtrl*)m_pListView)->SetRowData( nIndex, dwItemData );
	// 		}
	// 		assert( false );
	return 0;
}

int CUIComboBox::AddString( const tstring& strText )
{
	CheckAndGetView();
	if ( m_pListView
	 	&& m_pListView == m_pListViewDefault )
	{
		CUIListCtrl::ROW row;
		CUIText text;
		text.Create( strText, RECT_C( 0, 0, 10,\
			((CUIListCtrl*)m_pListView)->GetRowHeight() ), 1 );
		text.HasNotify( false );
		text.SetStyle( XWS_SINGLELINE|XWS_ENDELLIPSE );
		text.SetTextColor( m_clrLBText );
		text.SetFont( GetFont() );
		row.AddXCtrl( &text );
		CUIListCtrl::LPROW pRow = ((CUIListCtrl*)m_pListView)->InsertRow(row);
		int nHeight = ((CUIListCtrl*)m_pListView)->GetVirturlHeight();
		if ( m_nDropViewMaxHeight> 0
			&& nHeight > m_nDropViewMaxHeight )
		{
			nHeight = m_nDropViewMaxHeight;
		}
		RECT rcView = {m_marginDrop.left, m_marginDrop.top, \
			(m_nDropViewMaxWidth>0?m_nDropViewMaxWidth:RECT_W(m_rcXCtrl))-m_marginDrop.right, m_marginDrop.top+nHeight};
		m_pListView->SetRect( rcView, false );
		m_pListView->GetClient( rcView );
		IUICtrl* pText = pRow->GetXCtrl( 1 );
		if ( pText )
		{
			pText->SetRect( RECT_C( 0, 0, RECT_W(rcView), ((CUIListCtrl*)m_pListView)->GetRowHeight() ) );
		}
		if ( pRow )
		{
			return pRow->GetID();
		}
		return -1;
	}
	assert( false );
	return -1;
}

BOOL CUIComboBox::DeleteString( int nIndex )
{
	CheckAndGetView();
	if ( m_pListView
	 	&& m_pListView == m_pListViewDefault )
	{
		if ( nIndex >=0
			&& nIndex < ((CUIListCtrl*)m_pListView)->Size() )
		{
			((CUIListCtrl*)m_pListView)->RemoveRow( ((CUIListCtrl*)m_pListView)->Begin()+nIndex );
			int nHeight = ((CUIListCtrl*)m_pListView)->GetVirturlHeight();
			if ( m_nDropViewMaxHeight> 0
				&& nHeight > m_nDropViewMaxHeight )
			{
				nHeight = m_nDropViewMaxHeight;
			}
			RECT rcView = {m_marginDrop.left, m_marginDrop.top, \
				(m_nDropViewMaxWidth>0?m_nDropViewMaxWidth:RECT_W(m_rcXCtrl))-m_marginDrop.right, m_marginDrop.top+nHeight};
			m_pListView->SetRect( rcView, false );
		}
		return TRUE;
	}
	assert( false );
	return FALSE;
}

int CUIComboBox::InsertString( int nIndex, LPCTSTR lpszString )
{
	CheckAndGetView();
	if ( m_pListView
	 	&& m_pListView == m_pListViewDefault )
	{
		CUIListCtrl::ROW row;
		CUIText text;
		text.Create( lpszString, RECT_C( 0, 0, 10,\
			((CUIListCtrl*)m_pListView)->GetRowHeight() ), 1 );
		text.HasNotify( false );
		text.SetStyle( XWS_SINGLELINE|XWS_ENDELLIPSE );
		text.SetTextColor( m_clrLBText );
		text.SetFont( GetFont() );
		row.AddXCtrl( &text );
		CUIListCtrl::Iter _where = ((CUIListCtrl*)m_pListView)->End();
		if ( nIndex >=0
			&& nIndex < ((CUIListCtrl*)m_pListView)->Size() )
		{
			_where = ((CUIListCtrl*)m_pListView)->Begin()+nIndex;
		}
		_where = ((CUIListCtrl*)m_pListView)->InsertRow(_where, row);
		int nHeight = ((CUIListCtrl*)m_pListView)->GetVirturlHeight();
		if ( m_nDropViewMaxHeight> 0
			&& nHeight > m_nDropViewMaxHeight )
		{
			nHeight = m_nDropViewMaxHeight;
		}
		RECT rcView = {m_marginDrop.left, m_marginDrop.top, \
			(m_nDropViewMaxWidth>0?m_nDropViewMaxWidth:RECT_W(m_rcXCtrl))-m_marginDrop.right, m_marginDrop.top+nHeight };
		m_pListView->SetRect( rcView, false );
		if ( _where!= ((CUIListCtrl*)m_pListView)->End() )
		{
			m_pListView->GetClient( rcView );
			IUICtrl* pText = (*_where)->GetXCtrl( 1 );
			if ( pText )
			{
				pText->SetRect( RECT_C( 0, 0, rcView.right, ((CUIListCtrl*)m_pListView)->GetRowHeight() ) );
			}
			return (int)(_where-((CUIListCtrl*)m_pListView)->Begin());
		}
		return -1;
	}
	assert( false );
	return -1;
}

BOOL CUIComboBox::DropViewMessage( UINT message, WPARAM wParam, LPARAM lParam, DWORD dwParam )
{
	if ( XLAY_SIZECHANGED == message )
	{
		CUILayPanel* pPanel = (CUILayPanel*)lParam;
		if( pPanel )
		{
			IUICtrl* pCtrl = pPanel->GetXCtrl(1);
			SIZE sizePanel = pPanel->GetSize();
			if ( pCtrl )
			{
				pCtrl->SetRect( RECT_C( 0, 0, sizePanel.cx, pCtrl->GetSize().cy ) );
			}
		}
	}
	else if ( XLAY_LBDOWN == message )
	{
		NM_PARAM* param = (NM_PARAM*)(LONG_PTR)lParam;
		CUIComboBox* pCommbo = (CUIComboBox*)(DWORD_PTR)dwParam;
		if ( param && pCommbo && pCommbo->m_pListViewDefault )
		{
			CUIListCtrl::LPCROW pRow = ((CUIListCtrl*)pCommbo->m_pListViewDefault)->GetAt((INT32)param->nIndex);
			if ( pRow )
			{
				IUICtrl* pCtrl = pRow->GetXCtrl(1);
				if ( pCtrl )
				{
					pCommbo->SetText( pCtrl->GetText() );
					pCommbo->InvalidRect();
					pCommbo->GetUIWindow()->UpdateWindow();
					pCommbo->DropView( false );
				}
			}
		}
	}
	else if ( XWM_FOCUSLEAVE ==  message )
	{
		CUIComboBox* pCommbo = (CUIComboBox*)(DWORD_PTR)dwParam;
		if ( pCommbo
			&& pCommbo != UIGetFocus()
			&& pCommbo->m_hEdit != ::GetFocus()
			&& pCommbo->m_pListWindow->GetHandle() != ::GetFocus()
			)
		{
			pCommbo->DropView( false );
		}
	}
	return FALSE;
}

void CUIComboBox::CheckAndGetView()
{
	if ( NULL == m_pListView )
	{
		if ( NULL == m_pListViewDefault )
		{
			try
			{
				m_pListViewDefault = new CUIListCtrl;
			}
			catch( ... )
			{
				return;
			}
			if( !m_pListViewDefault->Create( RECT_C( 0,0,0,0 ), NULL, 1, NULL ) )
			{
				delete m_pListViewDefault;
				m_pListViewDefault = NULL;
				return;
			}
			m_pListViewDefault->EnableScroll( VERT, true );
			((CUIListCtrl*)m_pListViewDefault)->SetRowHeight( 20 );
			m_pListViewDefault->HookNotify( CUIComboBox::DropViewMessage, (DWORD)(DWORD_PTR)this );
			m_pListView = m_pListViewDefault;
			m_pListViewDefault->SetStyle( m_pListViewDefault->GetStyle()|XWS_MOUSETRACE );
			if ( m_bDefaultSkin 
				&& m_parent
				&& UIGetDefaultSkinFn())
			{
				(*UIGetDefaultSkinFn( ))(this);
			}
			m_pListViewDefault->Show( true );
		}
	}
}

void CUIComboBox::SetArrowSize( const SIZE& sizeArrow )
{
	if ( sizeArrow.cx > 0 )
	{
		m_sizeArrow.cx = sizeArrow.cx;
	}
	if ( sizeArrow.cy > 0 )
	{
		m_sizeArrow.cy = sizeArrow.cy;
	}
	
	if ( !RECT_ISEMPTY(m_rcXCtrl))
	{
		int yMax = RECT_H(m_rcXCtrl)-m_margin.top-m_margin.bottom;
		if ( m_sizeArrow.cy > yMax )
		{
			m_sizeArrow.cy = yMax;
		}
	}
}

void CUIComboBox::SetArrowSize( int cx, int cy )
{
	if ( cx > 0 )
	{
		m_sizeArrow.cx = cx;
	}
	if ( cy > 0 )
	{
		m_sizeArrow.cy = cy;
	}
	if ( !RECT_ISEMPTY(m_rcXCtrl))
	{
		int yMax = RECT_H(m_rcXCtrl)-m_margin.top-m_margin.bottom;
		if ( m_sizeArrow.cy > yMax )
		{
			m_sizeArrow.cy = yMax;
		}
	}
}

void CUIComboBox::SetLBHeight( int nHeight )
{
	if ( nHeight > 0 )
	{
		CheckAndGetView();
		if ( m_pListView
			&& m_pListView == m_pListViewDefault )
		{
			((CUIListCtrl*)m_pListView)->SetRowHeight( nHeight );
			return;
		}
		//未使用默认的DropView，调用不会产生任何效果
		assert( false );
	}
}


void CUIComboBox::SetLBSpace( int nSpace )
{
	if ( nSpace > 0 )
	{
		CheckAndGetView();
		if ( m_pListView
			&& m_pListView == m_pListViewDefault )
		{
			((CUIListCtrl*)m_pListView)->SetLineSpace( nSpace );
			return;
		}
		//未使用默认的DropView，调用不会产生任何效果
		assert( false );
	}
}

void CUIComboBox::SetLBHotBkGnd( const IMG& img )
{
	CheckAndGetView();
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		((CUIListCtrl*)m_pListView)->SetSelectRowBkGnd( img );
		return;
	}
	//未使用默认的DropView，调用不会产生任何效果
	assert( false );
}

void CUIComboBox::SetDropViewBkGnd( const IMG& img )
{
	CheckAndGetView();
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		((CUIListCtrl*)m_pListView)->SetBkGnd( img );
		return;
	}
	//未使用默认的DropView，调用不会产生任何效果
	assert( false );
}

void CUIComboBox::SetDropViewMargin( const Margin& margin )
{
	CheckAndGetView();
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		((CUIListCtrl*)m_pListView)->SetMargin( margin );
		return;
	}
	//未使用默认的DropView，调用不会产生任何效果
	assert( false );
}


void CUIComboBox::DeleteAll()
{
	CheckAndGetView();
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		((CUIListCtrl*)m_pListView)->RemoveAll();
		return;
	}
	//未使用默认的DropView，调用不会产生任何效果
	assert( false );
}

BOOL CUIComboBox::MouseWheel( int zDelta )
{
	if ( m_pListView )
	{
		m_pListView->MouseWheel( zDelta );
	}
	return TRUE;
}

BOOL CUIComboBox::KeyDown( WPARAM wParam, LPARAM lParam )
{
	if ( wParam == VK_ESCAPE )
	{
		if ( !m_strbkup.empty() )
		{
			SetText( m_strbkup );
		}

		if ( m_pListWindow
			&& IsWindowVisible(m_pListWindow->GetHandle()))
		{
			DropView( false );
			m_strbkup.clear();
		}
		else
		{
			UISetFocus( NULL );
		}
		return TRUE;
	}
	else if ( wParam == VK_RETURN )
	{
		if ( m_strbkup.empty() )
		{
			m_strbkup = m_strTextBase;
		}
		if ( m_pListViewDefault 
			&& m_pListViewDefault == m_pListView )
		{
			if ( m_pListWindow 
				&& IsWindowVisible(m_pListWindow->GetHandle()))
			{
				CUIListCtrl::LPCROW pRow = ((CUIListCtrl*)m_pListViewDefault)->GetAt(((CUIListCtrl*)m_pListViewDefault)->GetCurSel());
				if ( pRow )
				{
					IUICtrl* pCtrl = pRow->GetXCtrl(1);
					if ( pCtrl )
					{
						SetText( pCtrl->GetText() );
						InvalidRect();
						Update();
					}
				}	
			}
			DropView( false );
			m_strbkup.clear();
		}
	}
	else if ( wParam == VK_UP )
	{
		if ( m_strbkup.empty() )
		{
			if ( m_hEdit 
				&& IsWindowVisible( m_hEdit ) )
			{
				int nBuffer = ::GetWindowTextLength( m_hEdit );
				if( nBuffer > 0 )
				{
					++nBuffer;
					try
					{
						TCHAR* pBuffer = new TCHAR[nBuffer];
						memset( pBuffer, 0, (nBuffer)*sizeof(TCHAR) );
						GetWindowText( m_hEdit, pBuffer, nBuffer );
						m_strbkup = pBuffer;
						delete pBuffer;
					}
					catch( ... )
					{

					}
				}	
			}
			else
			{
				m_strbkup = m_strTextBase;
			}
		}
		if ( m_pListViewDefault 
			&& m_pListViewDefault == m_pListView 
			&& !m_pListView->IsEmpty() )
		{
			int nPos = (((CUIListCtrl*)m_pListViewDefault)->GetCurSel())-1;
			nPos = nPos>=0?nPos:((CUIListCtrl*)m_pListViewDefault)->Size()-1;

			((CUIListCtrl*)m_pListViewDefault)->SetCurSel( nPos );
			CUIListCtrl::LPCROW pRow = ((CUIListCtrl*)m_pListViewDefault)->GetAt(nPos);
			if ( pRow )
			{
				IUICtrl* pCtrl = pRow->GetXCtrl(1);
				if ( pCtrl )
				{
					SetText( pCtrl->GetText() );
					InvalidRect();
					Update();
				}
			}	

			((CUIListCtrl*)m_pListViewDefault)->SetCurSel( nPos );
			((CUIListCtrl*)m_pListViewDefault)->InvalidRect();
			((CUIListCtrl*)m_pListViewDefault)->Update();
			return TRUE;
		}
	}
	else if ( wParam == VK_DOWN )
	{
		if ( m_strbkup.empty() )
		{
			if ( m_hEdit 
				&& IsWindowVisible( m_hEdit ) )
			{
				int nBuffer = ::GetWindowTextLength( m_hEdit );
				if( nBuffer > 0 )
				{
					++nBuffer;
					try
					{
						TCHAR* pBuffer = new TCHAR[nBuffer];
						memset( pBuffer, 0, (nBuffer)*sizeof(TCHAR) );
						GetWindowText( m_hEdit, pBuffer, nBuffer );
						m_strbkup = pBuffer;
						delete pBuffer;
					}
					catch( ... )
					{

					}
				}	
			}
			else
			{
				m_strbkup = m_strTextBase;
			}
			assert( !m_strbkup.empty() );
		}
		if ( m_pListViewDefault 
			&& m_pListViewDefault == m_pListView 
			&& !m_pListView->IsEmpty() )
		{
			int nPos = (((CUIListCtrl*)m_pListViewDefault)->GetCurSel())+1;
			nPos = nPos%((CUIListCtrl*)m_pListViewDefault)->Size();

			((CUIListCtrl*)m_pListViewDefault)->SetCurSel( nPos );
			CUIListCtrl::LPCROW pRow = ((CUIListCtrl*)m_pListViewDefault)->GetAt(nPos);
			if ( pRow )
			{
				IUICtrl* pCtrl = pRow->GetXCtrl(1);
				if ( pCtrl )
				{
					SetText( pCtrl->GetText() );
					InvalidRect();
					Update();
				}
			}	

			((CUIListCtrl*)m_pListViewDefault)->SetCurSel( nPos );
			((CUIListCtrl*)m_pListViewDefault)->InvalidRect();
			((CUIListCtrl*)m_pListViewDefault)->Update();
			return TRUE;
		}
	}
	return __super::KeyDown( wParam, lParam );
}

void CUIComboBox::SetDropMargin( const Margin& margin )
{
	m_marginDrop = margin;
	if ( m_pListView
		&& m_pListView == m_pListViewDefault )
	{
		RECT rcView;
		m_pListView->GetRect( rcView );
		::RECT_TO( rcView, m_marginDrop.left, m_marginDrop.top );
		m_pListView->SetRect( rcView );
	}
}

void CUIComboBox::SetDropBkGnd( const IMG& img )
{
	m_imageDropBkGnd = img;
	if ( m_pListWindow )
	{
		m_pListWindow->SetBkGnd( img );
	}
}

LRESULT WINAPI CUIComboBox::DropViewWindowProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( m_mapUIEdits.empty() )
	{
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	map<HWND, EDITHELP>::iterator _lab = m_mapUIEdits.find( hWnd );
	if ( _lab != m_mapUIEdits.end()
		&& _lab->second.orgProc )
	{
		if ( message == WM_KEYDOWN
			&& (
			wParam == VK_RETURN
			||wParam == VK_ESCAPE
			||wParam == VK_UP 
			|| wParam == VK_DOWN ))
		{
			_lab->second.pEdit->KeyDown( wParam, lParam );
			return S_OK;
		}
	
		if( WM_ACTIVATE == message
			&&WA_INACTIVE == wParam )
		{
			POINT point;
			GetCursorPos( &point );
			HWND hwnd = WindowFromPoint( point );
			if ( hwnd != _lab->second.pEdit->GetHandle() )
			{
				if( _lab->second.pEdit->m_hEdit !=  hwnd )
				{
					::SetFocus( _lab->second.pEdit->GetHandle() );
					_lab->second.pEdit->DropView(false);
					_lab->second.pEdit->FocusLeave();
				}
			}
			else if ( UIGetFocus() != _lab->second.pEdit )
			{
				::SetFocus( _lab->second.pEdit->GetHandle() );
				_lab->second.pEdit->DropView(false);
				_lab->second.pEdit->FocusLeave();
			}
			return S_OK;
		}
		if ( message == WM_MOUSEWHEEL )
		{
			_lab->second.pEdit->MouseWheel( GET_WHEEL_DELTA_WPARAM(wParam) );
			return S_OK;
		}
		if ( message == WM_KILLFOCUS )
		{
			return S_OK;
		}
		return CallWindowProc( _lab->second.orgProc, hWnd, message, wParam, lParam );
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}