// UIWnd.cpp : 实现文件
#include "UIRichEdit.h"
#include ".\UIRichEdit.h"
#include <WindowsX.h>
#include <algorithm>
#include "UIView.h"
#include "UIMenu.h"
void CUIRichEdit::OnDraw(  IPainter* painter  )
{
	assert( XW_NULL != m_nClass );
	if ( !IsVisable() )
	{
		return;
	}

	IMG* pImageShow = NULL;
	ARGB clrMask = -1;
	RECT rcRichEdit;
	GetRect( rcRichEdit );	
	if ( RECT_ISEMPTY(rcRichEdit)
		|| !painter->RectVisible( &rcRichEdit )
		)
	{
		return;
	}
	if ( m_bEnable )
	{
		if ( ::UIGetFocus() != this 
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
		painter->DrawImage( pImageShow, &rcRichEdit );
	}	
	if ( !m_listContent.empty() )
	{
		RECT_INFLATE( rcRichEdit, -GetMargin().left, -GetMargin().top, -GetMargin().right, -GetMargin().bottom );
		if ( m_bEnableOutGlaw )
		{
			//DrawOuterGlow( _hDC, &rcRichEdit, m_strText.c_str(), m_hFont, 0, 0  );
		}
		ARGB clrOld = painter->SetBrushColor( IsMouseIn()?GetHotTextColor():GetTextColor() );
		list<Line*>::iterator _lab = m_listContent.begin();
		RECT rcDraw;
		GetRect( rcDraw );
		while ( _lab != m_listContent.end() )
		{
			RECT rcLine = (*_lab)->rcLine;
			RECT_TO( rcLine, rcLine.left+rcDraw.left, rcDraw.top+rcLine.top );
			if ( painter->RectVisible( &rcLine ) )
			{
				if ( rcLine.bottom > rcRichEdit.bottom )
				{
					rcLine.bottom = rcRichEdit.bottom;
				}
				if( (*_lab)->type == Line::STRING )
				{
					painter->DrawString( ((StringLine*)(*_lab))->strLineText.c_str(), -1, &rcLine, GetFont(), m_nStyle|XWS_CLIPMODE );
				}
				if ( rcLine.bottom == rcRichEdit.bottom )
				{
					break;
				}
			}
			++_lab;
		}
		painter->SetBrushColor( clrOld );
	}
}

BOOL CUIRichEdit::CursorIn( POINT point  )
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

BOOL CUIRichEdit::MouseLBUp( POINT pt )
{ 
	BOOL bRet = __super::MouseLBUp( pt );
	_Update();
	return bRet;
}

void CUIRichEdit::MouseLeave()
{
	__super::MouseLeave();
	_Update();
}

BOOL CUIRichEdit::MouseLBDown( POINT pt )
{
	BOOL bRet = __super::MouseLBDown( pt );
	_Update();
	return bRet;
}

BOOL CUIRichEdit::MouseRBDown( POINT pt )
{
	BOOL bRet = __super::MouseRBDown( pt );
	if( m_bEnable
		&& !m_bReadOnly )
	{
		UISetFocus( this );
		FocusIn(pt);
		_Update();
// 		if ( WM_RBUTTONDOWN == message
// 			|| WM_CONTEXTMENU == message )
// 		{
// 			if ( WM_CONTEXTMENU == message )
// 			{
// 				GetCaretPos( &point );
// 				ClientToScreen( m_hEdit, &point );
// 			}
// 			else
// 			{
				ClientToScreen( GetUIWindow()->GetHandle(), &pt );
/*			}*/
			NM_PARAM param = {0};
			param.point = pt;
			param.nCtrlId = GetID();
			if ( m_parent )
			{
				param.nParentId = m_parent->GetID();
			}
			//如果第三方已处理, 则直接返回
			if ( Notify( XWM_MENU, 0, (LPARAM)(LONG_PTR)&param ) )
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
			if( OpenClipboard( GetUIWindow()->GetHandle() ) )
			{
				HANDLE handle = GetClipboardData(CF_TEXT);
				bTextCliped = (INVALID_HANDLE_VALUE!=handle)&&(NULL!=handle);
				CloseClipboard();
			}

			menu.InsertMenu( _T("剪切(&T)"), NULL, WM_CUT );
			menu.InsertMenu( _T("复制(&Y)"), NULL, WM_COPY );
			menu.InsertMenu( _T("粘贴(&P)"), NULL, WM_PASTE, bTextCliped );
			menu.InsertMenu( _T("全部选择(&A)"), NULL, WM_CUT-1 );
			UINT nCmd = menu.TrackPopupMenu( pt.x, pt.y, 0 );
			switch( nCmd )
			{
			case WM_CUT-1:
				{
					//SelectAll();
				}
				break;
			case WM_CUT:
				{
					//Cut();
				}
				break;
			case WM_COPY:
				{
					//Copy();
				}
				break;
			case WM_PASTE:
				{
					//Paste();
				}
				break;
			default:
				break;
			}
		//	return S_OK;
/*		}*/
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
			//Copy();
		}
	}
	return bRet;
}

CUIRichEdit& CUIRichEdit::operator=( const CUIRichEdit& uObj )
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
	m_margin = uObj.m_margin;
	m_bReadOnly = uObj.m_bReadOnly;
	m_pCaretLine = NULL;
	m_nWhoHitCaret = 0;
	m_nLineCount = uObj.m_nLineCount;
	m_bScrollBarInClient = uObj.m_bScrollBarInClient;
	if ( m_painter )
	{
		UIDestroyPainter( m_painter );
		m_painter = NULL;
	}
	if ( GetHandle() )
	{
		m_painter = UICreateWndPainter( GetHandle() );
	}
	CopyContent( m_listContent, uObj.m_listContent );
	m_VScroll.Create( VERT, this, 1 );
	return *this;
}

CUIRichEdit::CUIRichEdit( const CUIRichEdit& uObj ) :CUIText( uObj )
,m_bEnable( uObj.m_bEnable )
,m_hCursor( uObj.m_hCursor )
,m_imageNormal(uObj.m_imageNormal)
,m_imageOver(uObj.m_imageOver)
,m_imageFocus( uObj.m_imageFocus )
,m_margin(uObj.m_margin)
,m_bReadOnly(uObj.m_bReadOnly)
,m_pCaretLine(NULL)
,m_nWhoHitCaret(0)
,m_nLineCount(0)
,m_painter(NULL)
,m_bScrollBarInClient(uObj.m_bScrollBarInClient)
{
	if ( GetHandle() )
	{
		m_painter = UICreateWndPainter( GetHandle() );
	}
	CopyContent( m_listContent, uObj.m_listContent );
	m_VScroll.Create( VERT, this, 1 );
}

CUIRichEdit::~CUIRichEdit()
{
 	while( !m_listContent.empty() )
 	{
 		delete m_listContent.back();
 		m_listContent.pop_back();
 	}
 	if ( m_painter )
 	{
 		UIDestroyPainter( m_painter );
 	}
}

BOOL CUIRichEdit::FocusIn( POINT point )
{
// 	if( __super::FocusIn( point ) 
// 		&& m_bEnable 
// 		&& !m_bReadOnly )
// 	{
// 		RECT rcThis;
// 		GetRect( rcThis );
// 		RECT_INFLATE( rcThis, -m_margin.left, -m_margin.top, -m_margin.right, -m_margin.bottom );
// 		if( NULL == m_hEdit )
// 		{	
// 			m_hEdit = CreateWindowEx( 0, _T("EDIT"), _T(""), WS_CHILD|ES_AUTOHSCROLL|(m_bPassword?ES_PASSWORD:0), \
// 				rcThis.left,rcThis.top,RECT_W(rcThis),\
// 				RECT_H(rcThis), GetUIWindow()->GetHandle(), NULL, GetModuleHandle(0), NULL );
// 			SetWindowPos( m_hEdit, HWND_TOP, 0,0,0,0, \
// 				SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
// 			assert( m_hEdit );
// 			EDITHELP edithelp = {
// 				this,
// 					(WNDPROC)(LONG_PTR)SetWindowLong( m_hEdit, GWL_WNDPROC, (LONG)(LONG_PTR)CUIRichEdit::WinProcess )
// 			};
// 			m_mapUIEditExs.insert( make_pair( m_hEdit, edithelp ) );
// 			HFONT hFont = GetFont();
// 			SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)(hFont?hFont:UIGetDefaultFont()), (LPARAM)(BOOL)(FALSE));
// 			::SendMessage(m_hEdit, EM_SETPASSWORDCHAR, m_bPassword?m_chPassword:0, 0);
// 			int nTab[1] = {0};
// 			nTab[0] = m_nTabStop;
// 			::SendMessage(m_hEdit, EM_SETTABSTOPS, 1,(LPARAM)nTab);
// 		}
// 		else
// 		{
// 			SetWindowPos( m_hEdit, HWND_TOP, rcThis.left,rcThis.top,RECT_W(rcThis),\
// 				RECT_H(rcThis), SWP_NOACTIVATE );
// 			if ( !GetFont() )
// 			{
// 				SendMessage( m_hEdit, WM_SETFONT, (WPARAM)(HFONT)UIGetDefaultFont(), (LPARAM)(BOOL)(FALSE));
// 			}
// 		}
// 		ShowWindow( m_hEdit, SW_SHOW );
// 		SetWindowText( m_hEdit, m_strText.c_str() );
// 		m_strText.clear();
// 		m_strTextBase.clear();
// 		SetFocus( m_hEdit );
// 		::SendMessage(m_hEdit,WM_KEYDOWN,VK_END,0); 
// 		InvalidRect();
// 		return TRUE;
// 	}
	return __super::FocusIn( point );
}

BOOL CUIRichEdit::FocusLeave()
{
	if( __super::FocusLeave() )
	{
// 		if ( !m_bReadOnly
// 			&& m_bEnable
// 			&& m_hEdit )
// 		{
// 			ShowWindow( m_hEdit, SW_HIDE );
// 			int nBuffer = ::GetWindowTextLength( m_hEdit );
// 			if( nBuffer > 0 )
// 			{
// 				++nBuffer;
// 				try
// 				{
// 					TCHAR* pBuffer = new TCHAR[nBuffer];
// 					memset( pBuffer, 0, (nBuffer)*sizeof(TCHAR) );
// 					m_bGetText = true;
// 					GetWindowText( m_hEdit, pBuffer, nBuffer );
// 					m_strText = pBuffer;
// 					delete pBuffer;
// 				}
// 				catch( ... )
// 				{
// 
// 				}
// 				m_strTextBase = m_strText;	
// 			}
// 			InvalidRect();
// 		}
		InvalidRect();
		Update();
		return TRUE;
	}
	return FALSE;
}

void CUIRichEdit::EnableMultiline( bool bEnable /*= false */ )
{
	if ( bEnable )
	{
		SetStyle( GetStyle()|XWS_SINGLELINE );
	}
	else
	{
		SetStyle( GetStyle()&(~XWS_SINGLELINE) );
	}
}

// void CUIRichEdit::Copy()
// {
// 	if ( m_hEdit )
// 	{
// 		SendMessage(m_hEdit, WM_COPY, 0, 0);	
// 	}
// 	else
// 	{
// 		//将文本放入剪贴板
// 		if( OpenClipboard( m_hEdit ) )
// 		{
// 			HGLOBAL clipbuffer;
// 			char * buffer;
// 			EmptyClipboard();
// 			clipbuffer = GlobalAlloc(GMEM_DDESHARE, (m_strTextBase.length()+1)*sizeof(TCHAR));
// 			buffer = (char*)GlobalLock(clipbuffer);
// 			memcpy( buffer, m_strTextBase.c_str(), (m_strTextBase.length()+1)*sizeof(TCHAR) );
// 			GlobalUnlock(clipbuffer);
// 			SetClipboardData(CF_TEXT,clipbuffer);
// 			CloseClipboard();
// 		}
// 	}
// }

void CUIRichEdit::SetTabWidth( int nTabWidth )
{
	m_nTabStop = nTabWidth;
}

BOOL CUIRichEdit::OnChar( WPARAM wParam, LPARAM lParam )
{
	if ( m_bReadOnly )
	{
		return FALSE;
	}
	static TCHAR cFirst = 0;
	if( cFirst == 0 )
	{
#if !defined( UNICODE ) && !defined(_UNICODE)
		if ( wParam & 0x80 )
		{
			cFirst = (TCHAR)wParam;
			return TRUE;
		}
		else
#endif
		if( wParam > 0x1F
			|| wParam == VK_TAB )
		{
			tstring strChar;
			strChar += (TCHAR)wParam;
			InputChar( strChar );
			m_strTextBase += (TCHAR)wParam;
		}
		else
		{
			switch( wParam )
			{
			case VK_BACK:
				{
					if ( !m_strTextBase.empty() )
					{
#if !defined( UNICODE ) && !defined(_UNICODE)
						if ( m_strTextBase[m_strTextBase.length()-1]&0x80 )
						{
							m_strTextBase.erase(m_strTextBase.length()-1);
						}
#endif
						m_strTextBase.erase(m_strTextBase.length()-1);
					}
					else
					{
						return TRUE;;
					}
				}
				break;
			case VK_RETURN:
				{
					if ( !(GetStyle() & XWS_SINGLELINE) )
					{
						tstring strChar;
						strChar+=(TCHAR)wParam;
						InputChar( strChar );
						m_strTextBase += (TCHAR)wParam;
					}
				}
				break;
			default:
				{
					return TRUE;
				}
			}
		}
	}
	else
	{		
		m_strTextBase += cFirst;
		m_strTextBase += (CHAR)wParam;
		tstring strChar;
		strChar += cFirst;
		strChar += (TCHAR)wParam;
		InputChar( strChar );
		cFirst = 0;
	}
	if ( cFirst == 0 )
	{
		m_strText = m_strTextBase;
		_Update();
	}
	return TRUE;
}

CUIRichEdit::Line* CUIRichEdit::GetBackLine( CUIRichEdit::Line::LINETYPE lineType, bool bnew /*= false*/ )
{
	try
	{
		if ( m_listContent.size() )
		{
			if ( !bnew )
			{
				if ( lineType == m_listContent.back()->type )
				{
					return m_listContent.back();
				}
				else if ( Line::_NULL == m_listContent.back()->type )
				{
					if ( lineType == Line::STRING )
					{
						StringLine* line = new StringLine;
						RECT_EMPTY(line->rcLine);
						line->index = m_listContent.back()->index;
						line->type = Line::STRING;
						line->rcLine = m_listContent.back()->rcLine;
						delete m_listContent.back();
						m_listContent.back() = line;
					}
					else if( lineType == Line::OBJECT )
					{
						ObjectLine* line = new ObjectLine;
						line->index = m_listContent.back()->index;
						line->type = Line::OBJECT;
						line->rcLine = m_listContent.back()->rcLine;
						delete m_listContent.back(); 
						m_listContent.back() = line;
					}
				}
			}
			else
			{
				if ( lineType == Line::STRING )
				{
					StringLine* line = new StringLine;
					RECT_EMPTY(line->rcLine);
					line->index = (int)m_listContent.size();
					line->type = Line::STRING;

					m_listContent.push_back( line );
				}
				else if( lineType == Line::OBJECT )
				{
					ObjectLine* line = new ObjectLine;
					line->index = (int)m_listContent.size();
					line->type = Line::OBJECT;
					RECT_EMPTY(line->rcLine);
					m_listContent.push_back( line );
				}
				else
				{
					NullLine* line = new NullLine;
					line->index = (int)m_listContent.size();
					line->type = Line::_NULL;
					RECT_EMPTY(line->rcLine);
					m_listContent.push_back( line );
				}
			}
		}
		else
		{
			if ( lineType == Line::STRING )
			{
				StringLine* line = new StringLine;
				line->index = (int)m_listContent.size();
				line->type = Line::STRING;
				RECT_EMPTY(line->rcLine);
				//RECT_S( line->rcLine, 0, 0, 0, 0 );
				m_listContent.push_back( line );
			}
			else if( lineType == Line::OBJECT )
			{
				ObjectLine* line = new ObjectLine;
				line->index = (int)m_listContent.size();
				line->type = Line::OBJECT;
				RECT_EMPTY(line->rcLine);
				m_listContent.push_back( line );
			}
			else
			{
				NullLine* line = new NullLine;
				line->index = (int)m_listContent.size();
				line->type = Line::_NULL;
				RECT_EMPTY(line->rcLine);
				m_listContent.push_back( line );
			}
		}
	}
	catch( ... )
	{
		return NULL;
	}
	m_nLineCount = (int)m_listContent.size();
	RECT rcClient;
	GetClient( rcClient );
	if ( m_listContent.size() > 1 )
	{
		m_listContent.back()->rcLine.left = m_margin.left;
		m_listContent.back()->rcLine.right = m_margin.left + RECT_W( rcClient );
		m_listContent.back()->rcLine.top = m_listContent.back()->rcLine.bottom = (*(++m_listContent.rbegin()))->rcLine.bottom;
	}
	else
	{
		m_listContent.back()->rcLine.left = m_margin.left;
		m_listContent.back()->rcLine.right = m_margin.left + RECT_W( rcClient );
		m_listContent.back()->rcLine.top = m_listContent.back()->rcLine.bottom = m_margin.top;
	}
	return m_listContent.back();
}

CUIRichEdit::Line* CUIRichEdit::GetCaretLine( POINT point )
{
	return *m_listContent.begin();
}

void CUIRichEdit::CopyContent( list<Line*>& contentDest, const list<Line*>& contentSrc )
{
	try
	{
		while( !contentDest.empty() )
		{
			delete contentDest.back();
			contentDest.pop_back();
		}
		Line* pLine = NULL;
		for ( list<Line*>::const_iterator _lab = contentSrc.begin();
			_lab !=contentSrc.end(); ++_lab )
		{
			if( (*_lab)->type == Line::STRING )
			{
				pLine = (Line*)new StringLine;
				*(StringLine*)pLine = *(StringLine*)(*_lab);
			}
			else
			{
				pLine = (Line*)new ObjectLine;
				*(ObjectLine*)pLine = *(ObjectLine*)(*_lab);
			}
			m_listContent.push_back( pLine );
		}
		if ( !m_listContent.empty() )
		{
			_UpdateScroll( m_listContent.back()->rcLine.bottom );
		}
	}
	catch( ... )
	{

	}
	return;
}

void CUIRichEdit::InputChar( const tstring& strChar )
{
	StringLine* pLine = (StringLine*)GetBackLine( Line::STRING );
	if ( pLine )
	{
		RECT rcPos = {0};
		pLine->vecCharPos.push_back( (int)pLine->strLineText.length() );
		pLine->veclineUnitRect.push_back( rcPos );
		pLine->strLineText += strChar;
		if ( strChar[0] == VK_RETURN )
		{
			GetBackLine( Line::_NULL, true );
			return;
		}
		if ( m_painter )
		{			
			if( m_painter->MeasureCharacterRects( pLine->strLineText.c_str(),\
				(int)pLine->veclineUnitRect.size(), GetFont(), m_nStyle, &pLine->veclineUnitRect[0], (int)pLine->veclineUnitRect.size()  ) )
			{
				RECT rcTest = { 0, 0, 10000, 10000 };
				m_painter->MeasureString( pLine->strLineText.c_str(), -1, &rcTest,GetFont(), m_nStyle );
 				pLine->rcLine.bottom = pLine->rcLine.top+rcTest.bottom;
				if ( !(m_nStyle&XWS_SINGLELINE) )
				{
					int _i = pLine->veclineUnitRect.back().right;
					if ( rcTest.right > RECT_W(pLine->rcLine)
						&& pLine->veclineUnitRect.size() > 1 )
					{
						StringLine* pLine1 = (StringLine*)GetBackLine( Line::STRING, true );
						if ( pLine1 )
						{
							pLine->vecCharPos.push_back( 0 );
							pLine1->veclineUnitRect.push_back( pLine->veclineUnitRect.back() );
							pLine1->strLineText+=strChar;
							RECT_TO( pLine1->veclineUnitRect.back(), pLine1->rcLine.right, pLine->veclineUnitRect.back().bottom );
							pLine->veclineUnitRect.pop_back();
							pLine->strLineText.erase( pLine->strLineText.length()-strChar.size(), strChar.size() );
							pLine->vecCharPos.pop_back();
							pLine1->rcLine.bottom = pLine1->rcLine.top + rcTest.bottom;
							_UpdateScroll( pLine1->rcLine.bottom );
							return ;
						}
					}
				}
				_UpdateScroll( pLine->rcLine.bottom );
			}
		}
	}
}

void CUIRichEdit::SetParent( IUIBase* parent )
{
	if ( parent != m_parent )
	{
		__super::SetParent( parent );
		if ( m_painter )
		{
			UIDestroyPainter( m_painter );
		}
		if ( GetHandle() )
		{
			m_painter = UICreateWndPainter( GetHandle() );
		}
	}
}

void CUIRichEdit::_UpdateScroll( int nMaxVScroll /*= 1*/, int nMaxHScroll /*= 1 */ )
{
	int nPre = 0;
	RECT rcThis;
	GetRect( rcThis );
	if( GetStyle() & XWS_VSCROLL )
	{
		if ( nMaxVScroll < RECT_H(rcThis) )
		{
			m_VScroll.SetPos(0);
			m_VScroll.Show( false );
		}
		else
		{
			m_VScroll.Show( true );
			m_VScroll.SetRange( 0, nMaxVScroll );
			if ( m_VScroll.GetPos() > nMaxVScroll - RECT_W( rcThis ) )
			{
				m_VScroll.SetPos(nMaxVScroll - RECT_W( rcThis ) );
			}
		}
	}
}