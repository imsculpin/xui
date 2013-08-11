// UIWnd.cpp : 实现文件
#include "UIEdit.h"
#include ".\UIEdit.h"
#include <WindowsX.h>
#include <algorithm>
#include "UIView.h"
#include "UIMenu.h"

map<HWND, CUIEdit::EDITHELP> CUIEdit::m_mapUIEdits;
void CUIEdit::OnDraw(  IPainter* painter  )
{
	assert( XW_NULL != m_nClass );
	if ( !IsVisable() )
	{
		return;
	}

	IMG* pImageShow = NULL;
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
		if ( !UIIsFocusIn(this)
			|| m_bReadOnly )
		{
			//普通状态
			if ( !IsMouseIn() )
			{
				pImageShow = &m_imageNormal;
			}
			//鼠标经过
			else
			{
				pImageShow = &m_imageOver;
			}
		}
		else
		{
			pImageShow = &m_imageFocus;
		}

		if( NULL == pImageShow
			&& IsImageValid(&m_imageNormal) )
		{
			pImageShow = &m_imageNormal;
		}
	}
	else
	{
		pImageShow = IsImageValid(&m_imageDisable)?&m_imageDisable:&m_imageNormal;
	}
	if ( pImageShow )
	{ 
		painter->DrawImage( pImageShow, &rcBtn );
	}	
	if ( !m_strText.empty() )
	{
		RECT_INFLATE( rcBtn, -GetMargin().left, -GetMargin().top, -GetMargin().right, -GetMargin().bottom );
		if ( m_bEnableOutGlaw )
		{
			//DrawOuterGlow( _hDC, &rcBtn, m_strText.c_str(), m_hFont, 0, 0  );
		}
		ARGB clrOld = painter->SetBrushColor( IsMouseIn()?GetHotTextColor():GetTextColor() );
		tstring* pString = &m_strText;
		if ( m_bPassword )
		{
			pString = &m_strPassword;
			if ( m_strPassword.length() 
				!= m_strText.length() )
			{
				m_strPassword.assign( m_strText.length(), m_chPassword );
			}
		}
		int nOldTab = painter->SetTabStop( m_nTabStop );
		painter->DrawString( pString->c_str(), -1, &rcBtn, GetFont(), m_nStyle|XWS_CLIPMODE );
		painter->SetTabStop( nOldTab );
		painter->SetBrushColor( clrOld );
	}
}

BOOL CUIEdit::CursorIn( POINT point  )
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

BOOL CUIEdit::MouseLBUp( POINT pt )
{
	BOOL bRet = __super::MouseLBUp( pt );
	_Update();
	return bRet;
}

void CUIEdit::MouseLeave()
{
	__super::MouseLeave();
	_Update();
}

BOOL CUIEdit::MouseLBDown( POINT pt )
{
	BOOL bRet = __super::MouseLBDown( pt );
	_Update();
	return bRet;
}

BOOL CUIEdit::MouseRBDown( POINT pt )
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
		return TRUE;
  	}
	if ( m_bReadOnly )
	{
		ClientToScreen( GetUIWindow()->GetHandle(), &pt );
		NM_PARAM param = {0};
		param.lParam = MAKELPARAM( pt.x, pt.y );
		param.nCtrlId = GetID();
		if ( m_parent )
		{
			param.nParentId = m_parent->GetID();
		}
		//如果第三方已处理, 则直接返回
		if ( Notify( XWM_MENU, 0, (LPARAM)(LONG_PTR)&param ) )
		{
			return TRUE;
		}
		if ( m_bPassword )
		{
			return TRUE;
		}
		//组件内部处理
		CUIMenu menu;
		ACCEL accel;
		accel.fVirt = FVIRTKEY;
		accel.cmd = WM_COPY;
		accel.key = _T('Y');
		menu.AddAccel( accel );
		menu.InsertMenu( _T("复制(&Y)"), NULL, WM_COPY, !m_strText.empty() );
		if( WM_COPY == menu.TrackPopupMenu( pt.x, pt.y, 0 ) )
		{
			Copy();
		}
	}
	return bRet;
}

CUIEdit& CUIEdit::operator=( const CUIEdit& uObj )
{
	if ( this == &uObj )
	{
		return *this;
	}
	CUIText::operator =(uObj);
	m_bEnable = uObj.m_bEnable;
	m_bPassword = uObj.m_bPassword;
	m_chPassword = uObj.m_chPassword;
	m_hCursor = uObj.m_hCursor;
	m_imageNormal = uObj.m_imageNormal;
	m_imageOver = uObj.m_imageOver;
	m_imageFocus = uObj.m_imageFocus;
	m_margin = uObj.m_margin;
	m_nTabStop = uObj.m_nTabStop;
	m_bGetText = false;
	m_bReadOnly = uObj.m_bReadOnly;
	m_bNumberOnly = uObj.m_bNumberOnly;
	m_nMax = uObj.m_nMax;
	m_bDefaultSkin=uObj.m_bDefaultSkin;
	m_hEdit = NULL;
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
	return *this;
}

CUIEdit::CUIEdit( const CUIEdit& uObj ) :CUIText( uObj )
,m_bEnable( uObj.m_bEnable )
,m_hCursor( uObj.m_hCursor )
,m_imageNormal(uObj.m_imageNormal)
,m_imageOver(uObj.m_imageOver)
,m_imageFocus( uObj.m_imageFocus )
,m_margin(uObj.m_margin)
,m_bPassword(uObj.m_bPassword)
,m_chPassword(uObj.m_chPassword)
,m_bReadOnly(uObj.m_bReadOnly)
,m_nTabStop(uObj.m_nTabStop)
,m_bGetText(false)
,m_hEdit( NULL )
,m_bNumberOnly(uObj.m_bNumberOnly)
,m_nMax(uObj.m_nMax)
,m_bDefaultSkin(uObj.m_bDefaultSkin)
{
	if ( m_bDefaultSkin 
		&& m_parent
		&& UIGetDefaultSkinFn())
	{
		(*UIGetDefaultSkinFn( ))(this);
	}
}

BOOL CUIEdit::FocusIn( POINT point )
{
	if( __super::FocusIn( point ) 
		&& m_bEnable 
		&& !m_bReadOnly )
	{
		RECT rcThis;
		GetRect( rcThis );
		RECT_INFLATE( rcThis, -m_margin.left, -m_margin.top, -m_margin.right, -m_margin.bottom );
		if( NULL == m_hEdit )
		{	
			WNDCLASS wndcls;
			if (!::GetClassInfo(NULL, _T("X_EDIT"), &wndcls))
			{
				::GetClassInfo(NULL, _T("EDIT"), &wndcls);
				wndcls.lpszClassName    = _T("X_EDIT");
				RegisterClass(&wndcls);
			}
			m_hEdit = CreateWindowEx( 0, _T("X_EDIT"), _T(""), WS_CHILD|(m_bPassword?ES_PASSWORD:0)|\
				(!(GetStyle()&XWS_SINGLELINE)?ES_WANTRETURN|ES_MULTILINE|ES_AUTOVSCROLL:ES_AUTOHSCROLL)\
				|(m_bNumberOnly?ES_NUMBER:0), \
				rcThis.left,rcThis.top,RECT_W(rcThis),\
				RECT_H(rcThis), GetUIWindow()->GetHandle(), NULL, GetModuleHandle(0), NULL );
			SetWindowPos( m_hEdit, HWND_TOP, 0,0,0,0, \
				SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
			assert( m_hEdit );
			EDITHELP edithelp = {
				this,
					(WNDPROC)(LONG_PTR)SetWindowLong( m_hEdit, GWL_WNDPROC, (LONG)(LONG_PTR)CUIEdit::WinProcess )
			};
			m_mapUIEdits.insert( make_pair( m_hEdit, edithelp ) );
			HFONT hFont = GetFont();
			SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)(hFont?hFont:UIGetDefaultFont()), (LPARAM)(BOOL)(FALSE));
			::SendMessage(m_hEdit, EM_SETPASSWORDCHAR, m_bPassword?m_chPassword:0, 0);
			if ( m_nMax > 0 )
			{
				SendMessage(m_hEdit, EM_SETLIMITTEXT, m_nMax, 0);
			}
			int nTab[1] = {0};
			nTab[0] = m_nTabStop;
			::SendMessage(m_hEdit, EM_SETTABSTOPS, 1,(LPARAM)nTab);
		}
		else
		{
			SetWindowPos( m_hEdit, HWND_TOP, rcThis.left,rcThis.top,RECT_W(rcThis),\
				RECT_H(rcThis), SWP_NOACTIVATE );
			if ( !GetFont() )
			{
				SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)UIGetDefaultFont(), (LPARAM)(BOOL)(FALSE));
			}
		}
		ShowWindow( m_hEdit, SW_SHOW );
		SetWindowText( m_hEdit, m_strText.c_str() );
		m_strText.clear();
		m_strTextBase.clear();
		SetFocus( m_hEdit );
		::SendMessage(m_hEdit,WM_KEYDOWN,VK_END,0); 
		InvalidRect();
		return TRUE;
	}
	return FALSE;
}

BOOL CUIEdit::FocusLeave()
{
	if( __super::FocusLeave() )
	{
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
					m_bGetText = true;
					GetWindowText( m_hEdit, pBuffer, nBuffer );
					m_strTextBase = pBuffer;
					delete pBuffer;
				}
				catch( ... )
				{

				}
				m_strText = m_strTextBase;	
			}
			InvalidRect();
		}
		return TRUE;
	}
	return FALSE;
}

void CUIEdit::EnableMultiline( bool bEnable /*= false */ )
{
	if ( bEnable )
	{
		SetStyle( GetStyle()|XWS_SINGLELINE );
		if( m_hEdit )
		{
			SetWindowLong( m_hEdit, GWL_STYLE, GetWindowLong( m_hEdit, GWL_STYLE )|ES_AUTOHSCROLL&(~ES_MULTILINE)&(~ES_AUTOVSCROLL) );
		}
	}
	else
	{
		SetStyle( GetStyle()&(~XWS_SINGLELINE) );
		if( m_hEdit )
		{
			SetWindowLong( m_hEdit, GWL_STYLE, GetWindowLong( m_hEdit, GWL_STYLE )|ES_MULTILINE|ES_AUTOVSCROLL&(~ES_AUTOHSCROLL) );
		}
	}
}

void CUIEdit::SetParent( IUIBase* parent )
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

void CUIEdit::SetPasswordMode( bool bPassword, TCHAR ch /*= 0 */ )
{
	if ( !m_bEnable )
	{
		return;
	}
	m_bPassword = bPassword;
	if( ch != 0 )
	{
		m_chPassword = ch;
	}
	if ( m_hEdit )
	{
		::SendMessage(m_hEdit, EM_SETPASSWORDCHAR, m_bPassword?m_chPassword:0, 0);
	}
}

LRESULT WINAPI CUIEdit::WinProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( m_mapUIEdits.empty() )
	{
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	map<HWND, EDITHELP>::iterator _lab = m_mapUIEdits.find( hWnd );
	if ( _lab != m_mapUIEdits.end()
		&& _lab->second.orgProc )
	{
		if ( WM_PASTE == message
			&& _lab->second.pEdit->m_bPassword )
		{
			return S_OK;
		}
		//防止非法取密码文本
		if ( WM_GETTEXT == message )
		{
			if ( !_lab->second.pEdit->m_bGetText )
			{
				return S_OK;
			}
			_lab->second.pEdit->m_bGetText = false;
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
			if ( _lab->second.pEdit->m_bPassword )
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
 					_lab->second.pEdit->SelectAll();
 				}
 				break;
 			case WM_CUT:
 				{
 					_lab->second.pEdit->Cut();
 				}
 				break;
 			case WM_COPY:
 				{
 					_lab->second.pEdit->Copy();
 				}
 				break;
 			case WM_PASTE:
 				{
 					_lab->second.pEdit->Paste();
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

void CUIEdit::SetRect( const RECT& rcSet )
{
	__super::SetRect( rcSet );
	if ( m_hEdit )
	{
		RECT rcThis;
		GetRect( rcThis );
		RECT_INFLATE(rcThis, -m_margin.left, -m_margin.top, -m_margin.right, -m_margin.bottom );
		SetWindowPos( m_hEdit, NULL, rcThis.left, rcThis.top,RECT_W(rcThis), RECT_H(rcThis), SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void CUIEdit::SetFont( HFONT pFont )
{
	__super::SetFont( pFont );
	m_hFont = pFont;
	if ( m_hEdit )
	{
		HFONT hFont = GetFont();
		SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)(hFont?hFont:UIGetDefaultFont()), (LPARAM)(BOOL)(FALSE));
	}
}

void CUIEdit::Copy()
{
	if ( m_hEdit )
	{
		SendMessage(m_hEdit, WM_COPY, 0, 0);	
	}
	else
	{
		//将文本放入剪贴板
		if( OpenClipboard( m_hEdit ) )
		{
			HGLOBAL clipbuffer;
			char * buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, (m_strTextBase.length()+1)*sizeof(TCHAR));
			buffer = (char*)GlobalLock(clipbuffer);
			memcpy( buffer, m_strTextBase.c_str(), (m_strTextBase.length()+1)*sizeof(TCHAR) );
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT,clipbuffer);
			CloseClipboard();
		}
	}
}

void CUIEdit::SetTabStop( int nTab )
{
	m_nTabStop = nTab;
	if ( m_hEdit )
	{
		int nTab[1] = {0};
		nTab[0] = m_nTabStop;
		::SendMessage(m_hEdit, EM_SETTABSTOPS, 1,
			(LPARAM)nTab);
	}
}

tstring CUIEdit::GetText() const
{
	tstring strRet;
	if ( m_hEdit && IsWindowVisible( m_hEdit ) )
	{
		int nBuffer = ::GetWindowTextLength( m_hEdit );
		if( nBuffer > 0 )
		{
			++nBuffer;
			try
			{
				TCHAR* pBuffer = new TCHAR[nBuffer];
				memset( pBuffer, 0, (nBuffer)*sizeof(TCHAR) );
				m_bGetText = true;
				GetWindowText( m_hEdit, pBuffer, nBuffer );
				strRet = pBuffer;
				delete pBuffer;
			}
			catch( ... )
			{

			}
		}
	}
	else
	{
		return __super::GetText();
	}
	return strRet;
}