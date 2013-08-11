#include "Switch.h"
#include ".\UIListCtrl.h"
#include "UIText.h"
#include <map>

#define  CONST_SCROLLBAR  0
void CUIListCtrl::RemoveAll()
{
	m_nHotRow = -1;
	m_nFocusRow = -1;
	m_VScroll.Max() = 1;
	m_list.RemoveAll();
	m_setSelected.clear();
	m_rgShow.Reset();
	SetScrollPos( VERT, 0 );
	_UpdateScroll();
	InvalidRect(NULL);
	return;
}

BOOL CUIListCtrl::HorzScroll(int nSBCode, int nPos )
{
	RECT rcClient;
	GetClient( rcClient );
	if ( m_HScroll.Max() <= RECT_W(rcClient) )
	{
		return TRUE;
	}
	return TRUE;
}

BOOL CUIListCtrl::VertScroll(int nSBCode, int nPos, int iDrect)
{
	nSBCode;
	nPos;
	m_nHotRow = -1;
	if( THUMB == nSBCode )
	{
		(void)_UpdateShow();
	}
	return TRUE;
}


BOOL CUIListCtrl::MouseWheel(int zDelta)
{
	return __super::MouseWheel( zDelta );
}

BOOL CUIListCtrl::MouseMoving(POINT point)
{
	if( __super::MouseMoving( point ) )
	{
		return TRUE;
	}
	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+=m_VScroll.GetPos();
	if ( pRow )
	{
		pRow->MouseMoving( _point );
		if ( !m_rgShow.IsValid() )
		{
			return TRUE;
		}

		if ( m_nHotRow < 0 || m_nHotRow != pRow->m_nId )
		{
			RECT _rcPre;
			RECT_EMPTY( _rcPre );
			if ( XWS_MOUSETRACE & GetStyle()
				&& !m_setSelected.empty() 
				)
			{
				_rcPre = GetRowRect( *m_setSelected.begin() );
			}
			else if ( m_nHotRow >= 0 )
			{
				_rcPre = GetRowRect( m_nHotRow );
			}
			m_nHotRow = pRow->m_nId;
			if ( GetStyle() & XWS_MOUSETRACE )
			{
				m_nFocusRow = pRow->m_nId;
				m_setSelected.clear();
				m_setSelected.insert( pRow->m_nId );
			}
 			RECT rcInvalidate = GetRowRect( pRow->m_nId );
 			if ( !RECT_ISEMPTY(_rcPre) )
 			{
 				::UnionRect( &rcInvalidate, &GetRowRect( pRow->m_nId ), &_rcPre );
 			}
			InvalidRect( &rcInvalidate );
			Update();
		}

		if ( m_nMouseInIndex != pRow->m_nId )
		{
			if ( -1 != m_nMouseInIndex )
			{
				LPROW pRow = _GetRow( m_nMouseInIndex );
				if ( pRow )
				{
					pRow->MouseLeave();
				}
			}
			m_nMouseInIndex = pRow->m_nId;
			pRow->MouseIn();
		}
	}
	else
	{
		if( -1 != m_nMouseInIndex )
		{
			LPROW pRow = _GetRow( m_nMouseInIndex );
			if ( pRow )
			{
				pRow->MouseLeave();
			}
		}
		m_nMouseInIndex = -1;
		if ( m_nHotRow >= 0 )
		{	
			long lFalshRow = m_nHotRow;
			m_nHotRow = -1;
			InvalidRect( &GetRowRect(lFalshRow) );
			Update();
		}
		_Notify( XLAY_MOUSEMOVE, _point );
	}
	return TRUE;
}


BOOL CUIListCtrl::CursorIn( POINT point )
{
	BOOL bRet = FALSE;
	if ( Size() )
	{
		POINT _point = point;
		_point.y+= m_VScroll.GetPos();
		LPROW pRow = GetRowFromPoint( point );
		if ( pRow )
		{
			bRet = pRow->CursorIn( _point );
		}
	}
	if ( !bRet )
	{
		return __super::CursorIn( point );
	}
	return bRet;
}


BOOL CUIListCtrl::MouseLBUp(POINT point )
{
	if( __super::MouseLBUp( point ) )
	{
		return TRUE;
	}

	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	if ( pRow )
	{
		return pRow->MouseLBUp( _point );
	}
	_Notify( XLAY_LBUP, _point );
	return TRUE;
}


BOOL CUIListCtrl::MouseLDBClick(POINT point )
{
	if( __super::MouseLDBClick( point ) )
	{
		return TRUE;
	}
	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	if ( pRow )
	{
		pRow->MouseLDBClick( _point );
		return TRUE;
	}
	_Notify( XLAY_LDBCLICK, _point );
	return TRUE;
}


BOOL CUIListCtrl::MouseRBUp(POINT point )
{
	if( __super::MouseRBUp( point ) )
	{
		return TRUE;
	}
	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	if ( pRow )
	{
		return pRow->MouseRBUp( _point );
	}
	_Notify( XLAY_RBUP, _point );
	return TRUE;
}


BOOL CUIListCtrl::MouseRDBClick(POINT point )
{
	if( __super::MouseRDBClick( point ) )
	{
		return TRUE;
	}
	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	if ( pRow )
	{
		return pRow->MouseRDBClick( _point );
	}
	_Notify( XLAY_RDBCLICK, _point );
	return TRUE;
}


void CUIListCtrl::MouseLeave(  )
{
	//如果记录列表为空,做空操作
	__super::MouseLeave(  );
	if( -1 != m_nMouseInIndex )
	{
		LPROW pRow = _GetRow( m_nMouseInIndex );
		if ( pRow )
		{
			pRow->MouseLeave();
		}
	}
	m_nMouseInIndex = -1;
	if ( m_nHotRow >= 0 )
	{
		long lFalshRow = m_nHotRow;
		m_nHotRow = -1;
		InvalidRect( &GetRowRect(lFalshRow) );
	}
	return;
}

void CUIListCtrl::OnDraw(  IPainter* painter )
{
	__super::OnDraw( painter );
	POINT ptOrg = {0};
	for ( INT32 _i = m_rgShow.m_min; _i <= m_rgShow.m_max; ++_i )
	{
		LPROW pRow = NULL;
		try
		{
			pRow = _GetRow( _i );
		}
		catch( ... )
		{
			pRow = NULL;
		}
		if ( NULL != pRow && pRow->m_nId >= 0 )
		{
			pRow->SetState( RRS_NORMAL );
			if ( pRow->m_nId == m_nHotRow )
			{
				pRow->SetState( RRS_HOT );
			}
			if ( pRow->m_nId == m_nFocusRow )
			{
				pRow->SetState( RRS_FOCUSED|pRow->GetState() );
			}
			if ( m_setSelected.find( pRow->m_nId ) != m_setSelected.end() )
			{
				pRow->SetState( RRS_SELECTED|pRow->GetState() );
			}	
			RECT rcClient;
			GetClient(rcClient);
			RECT rcRow;
			pRow->GetRect( rcRow );
			if ( rcRow.right != rcClient.right )
			{
				pRow->SetSize( rcClient.right-rcRow.left, RECT_H(rcRow) );
			}
			if ( !painter->RectVisible( &GetRowRect( _i ) ) )
			{
				continue;
			}
			EraseRowBkGnd( pRow, painter );
			pRow->OnDraw( painter );
		}
	} 
	return;
}

void CUIListCtrl::EraseRowBkGnd( LPROW pRowErase, IPainter* painter )
{
	if ( pRowErase && pRowErase->m_nId >= 0 )
	{
		RECT rcRow;
		pRowErase->GetRect( rcRow );
		//绘制活动行的背景色
		if ( GetState(pRowErase->m_nId)&RRS_HOT 
			&& (m_imageHot.hImage|| X_TRANSPARENT != m_imageHot.imageAvgClr ) )
		{
			if( m_imageHot.hImage )
			{
				painter->DrawImage( &m_imageHot, &rcRow );
			}
			else if( X_TRANSPARENT != m_imageHot.imageAvgClr )
			{			
				ARGB clrOld = painter->SetBrushColor( m_imageHot.imageAvgClr );
				HRGN h_rgn = CreateRoundRectRgn(rcRow.left,rcRow.top,rcRow.right,rcRow.bottom,5,5);
				painter->FillRegion( h_rgn );
				painter->SetBrushColor( clrOld );
				DeleteObject( h_rgn );
			}
		}
		//绘制选中行的背景色
		else if( GetState(pRowErase->m_nId)&RRS_SELECTED )
		{
			if ( m_setSelected.count( pRowErase->m_nId )
				&& IsRowVisable( pRowErase->m_nId ) )
			{
				if ( m_imageSelect.hImage )
				{
					painter->DrawImage( &m_imageSelect, &rcRow );
				}
				else if( X_TRANSPARENT != m_imageSelect.imageAvgClr )
				{
					ARGB clrOld = painter->SetBrushColor( m_imageSelect.imageAvgClr );
					HRGN h_rgn = CreateRoundRectRgn(rcRow.left,rcRow.top,rcRow.right,rcRow.bottom,5,5);
					painter->FillRegion( h_rgn );
					painter->SetBrushColor( clrOld );
					DeleteObject( h_rgn );
				}
			}
		}	
		if ( IsImageValid(&m_imageLineSpace) )
		{
			RECT rcLineSpace = {rcRow.left, rcRow.bottom, rcRow.right, rcRow.bottom + m_nLineSpace};
			painter->DrawImage( &m_imageLineSpace, &rcLineSpace );
		}
	}	
}

void CUIListCtrl::_UpdateScroll( int nMaxVScroll /*= 1*/, int nMaxHScroll /*= 1*/ )
{
	RECT rcThis;
	GetClient( rcThis );
	if ( RECT_ISEMPTY(rcThis) )
	{
		return;
	}
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	int nPre = 0;
	if ( GetStyle() & XWS_HSCROLL )
	{
		nPre = m_HScroll.Max();
		m_HScroll.SetRange( 0, nMaxHScroll );	
		if ( m_HScroll.Max() > RECT_W(rcThis) )
		{
			m_HScroll.Show( true );
		}
		else
		{
			m_HScroll.Show(false);
		}
	}

	if( GetStyle() & XWS_VSCROLL )
	{
		nPre = m_VScroll.Max();
		m_VScroll.SetRange( 0, nMaxVScroll );		
		if ( m_VScroll.Max() > RECT_H(rcThis) )
		{
			m_VScroll.Show( true );
		}
		else
		{
			m_VScroll.Show(false);
		}
	}
	if ( m_VScroll.GetPos()+RECT_H(rcThis) > m_VScroll.Max()
		&& m_VScroll.Max() > RECT_H(rcThis) )
	{
		m_VScroll.SetPos( m_VScroll.Max()-RECT_H(rcThis) );
	}
	if ( m_HScroll.GetPos()+RECT_W(rcThis) > m_HScroll.Max()
		&&  m_HScroll.GetPos() > RECT_W(rcThis) )
	{
		m_HScroll.SetPos(m_HScroll.Max()-RECT_W(rcThis));
	}
}

BOOL CUIListCtrl::MouseLBDown(POINT point)
{
	if( __super::MouseLBDown( point ) )
	{
		return TRUE;
	}
	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	if ( NULL != pRow && m_rgShow.IsValid() )
	{	
		if ( GetStyle()&XWS_MOUSETRACE											  //如果设置了鼠标跟踪，直接返回
			|| GetStyle()&XWS_SINGLESEL											  //如果设置了单行选中，直接返回
			)//如果在选中的多行的中间点击了右键，直接返回
		{
			if ( !m_setSelected.empty() )
			{
				if ( m_setSelected.end() == m_setSelected.find( pRow->m_nId ) )
				{
					m_setSelected.clear();	
					m_nFocusRow = pRow->m_nId;
					m_setSelected.insert( pRow->m_nId );
					InvalidRect(NULL);
					Update();
				}				
			}
		}
		else
		{
			//按下了SHIFT键　(SHIFT is pressed)
			if ( GetKeyState( VK_SHIFT ) < 0 )
			{
				if ( m_nFocusRow >= 0 )
				{
					m_setSelected.clear();
					INT32 nMinIndex = min( pRow->m_nId, m_nFocusRow );
					INT32 nMaxRow = max( pRow->m_nId, m_nFocusRow );
					for ( ; nMinIndex <= nMaxRow; nMinIndex++ )
					{
						m_setSelected.insert( nMinIndex );
					}
					m_nFocusRow = pRow->m_nId;
					InvalidRect(NULL);
				}
			}
			//没有按下了CTRL键，清空选中集　(CTRL is not pressed, clear the selected set) 
			if( GetKeyState( VK_CONTROL ) >= 0 )
			{
				m_setSelected.clear();
			}
			//多行选中状态下，如果按下了control键，而且当前点击的行被标记为选中，则取消选中状态
			if ( m_setSelected.size() > 1
				&& (GetState(pRow->m_nId)&RRS_SELECTED)
				&& GetKeyState( VK_CONTROL ) < 0)
			{
				m_setSelected.erase( pRow->m_nId );
			}
			else
			{
				m_setSelected.insert( pRow->m_nId );
			}
			m_nFocusRow = pRow->m_nId;
			InvalidRect(NULL);
			Update();
		}
		pRow->MouseLBDown( _point );
	}
	else
	{
		//如果按下了SHIFT或CTRL键，则不处理
		if ( GetKeyState( VK_CONTROL ) < 0
			|| GetKeyState( VK_SHIFT ) < 0)
		{
			return TRUE;
		}
		m_nHotRow = -1;
		m_setSelected.clear();
		m_nFocusRow = -1;
		InvalidRect(NULL);
		Update();
		_Notify( XLAY_LBDOWN, _point );
	}
	return TRUE;
}

BOOL CUIListCtrl::MouseRBDown(POINT point)
{
	if( __super::MouseRBDown( point ) )
	{
		return TRUE;
	}
	LPROW pRow = GetRowFromPoint( point );
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	if ( NULL != pRow && m_rgShow.IsValid() )
	{	
		if ( GetStyle()&XWS_MOUSETRACE											  //如果设置了鼠标跟踪，直接返回
			|| GetStyle()&XWS_SINGLESEL											  //如果设置了单行选中，直接返回
			)//如果在选中的多行的中间点击了右键，直接返回
		{
			if ( !m_setSelected.empty() )
			{
				if ( m_setSelected.end() == m_setSelected.find( pRow->m_nId ) )
				{
					m_setSelected.clear();	
					m_nFocusRow = pRow->m_nId;
					m_setSelected.insert( pRow->m_nId );
					InvalidRect(NULL);
					Update();
				}				
			}
		}
		else
		{
			//按下了SHIFT键　(SHIFT is pressed)
			if ( GetKeyState( VK_SHIFT ) < 0 )
			{
				if ( m_nFocusRow >= 0 )
				{
					m_setSelected.clear();
					INT32 nMinIndex = min( pRow->m_nId, m_nFocusRow );
					INT32 nMaxRow = max( pRow->m_nId, m_nFocusRow );
					for ( ; nMinIndex <= nMaxRow; nMinIndex++ )
					{
						m_setSelected.insert( nMinIndex );
					}
					m_nFocusRow = pRow->m_nId;
					InvalidRect(NULL);
					Update();
				}
			}
			//没有按下了CTRL键，清空选中集　(CTRL is not pressed, clear the selected set) 
			if( GetKeyState( VK_CONTROL ) >= 0 )
			{
				m_setSelected.clear();
			}
			//多行选中状态下，如果按下了control键，而且当前点击的行被标记为选中，则取消选中状态
			if ( m_setSelected.size() > 1
				&& (GetState(pRow->m_nId)&RRS_SELECTED)
				&& GetKeyState( VK_CONTROL ) < 0)
			{
				m_setSelected.erase( pRow->m_nId );
			}
			else
			{
				m_setSelected.insert( pRow->m_nId );
			}
			m_nFocusRow = pRow->m_nId;
			InvalidRect(NULL);
			Update();
		}
		return pRow->MouseRBDown( _point );
	}
	else
	{
		//如果按下了SHIFT或CTRL键，则不处理
		if ( GetKeyState( VK_CONTROL ) < 0
			|| GetKeyState( VK_SHIFT ) < 0)
		{
			return TRUE;
		}
		m_nHotRow = -1;
		m_setSelected.clear();
		m_nFocusRow = -1;
		InvalidRect(NULL);
		Update();
		_Notify( XLAY_RBDOWN, _point );
	}
	return TRUE;
}


CUIListCtrl::LPROW CUIListCtrl::GetRowFromPoint( const POINT& point )
{
	POINT pt = point;
	pt.y += m_VScroll.GetPos();
	if ( !m_rgShow.IsValid() )
	{
		return NULL;
	}
	for(  INT32 _i = m_rgShow.m_min; _i <= m_rgShow.m_max;  _i++ )
	{
		if( ::PtInRect( &GetRowRect( _i ), pt ))
		{
			return _GetRow( _i );
		}
	}
	return NULL;
}

BOOL CUIListCtrl::SetVirtualMode( IDataInterface* pDB, FORMAT format, const ROW* pTemplate /*= NULL*/ )
{
	if ( pDB )
	{
		m_pdataBase = pDB;
		m_format = format;
		pDB->SetRelatedUI( this );
		//根据模板
		if ( pTemplate )
		{
			try
			{
				m_pRowTemplate = new CUILayPanel(*pTemplate);
			}
			catch( ... )
			{
				return FALSE;
			}
		}
	}
	InvalidRect(NULL);
	return TRUE;
}

void CUIListCtrl::DBChanged( BOOL bReflashAllData /*= TRUE*/ )
{
	m_nListSize = m_pdataBase->size();
	if ( !Size() )
	{
		_UpdateScroll();
		m_rgShow.Reset();
		InvalidRect(NULL);
		return;
	}
	if ( m_nRowHeight != RL_AUTOPANELHEIGHT )
	{
		_UpdateScroll( (m_nRowHeight+m_nLineSpace)*Size()+m_margin.top);
	}
 	if ( bReflashAllData )
 	{
		GetVirtualModeBufferSize();
		for ( Iter _lab = Begin(); _lab != End(); ++_lab )
		{
			(*_lab)->m_nId = -1;
		}
	}
	_UpdateShow( FALSE );
	return;
}

INT32 CUIListCtrl::_FirstVisable()
{
	if ( !Size() )
	{
		return -1;
	}
	if ( !IsVirtualMode() )
	{
		if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
		{
			if ( m_VScroll.GetPos() - m_margin.top > 0 )
			{
				return (m_VScroll.GetPos() - m_margin.top)/m_nRowHeight;
			}
			return 0;
		}
		else
		{
			if ( m_VScroll.GetPos() - m_margin.top > 0 )
			{
				int nFirst = m_rgShow.m_min;
				LPROW pRow = GetAt( m_rgShow.m_min );
				if ( pRow )
				{
					RECT rcClient;
					GetClient( rcClient );
					RECT rcRow;
					pRow->GetRect( rcRow );
					if ( rcRow.bottom <= rcClient.top  )//向下找第一行可视
					{
						while( ++nFirst < Size() )
						{
							if( GetRowRect(nFirst).bottom > rcClient.top )
							{
								break;
							}
						}
						nFirst = min( Size()-1, nFirst );
					}
					else if ( rcRow.top >= rcClient.top )//向上找第一行可视
					{
						while( --nFirst >= 0 )
						{
							if( GetRowRect(nFirst).bottom < rcClient.top )
							{
								++nFirst;
								break;
							}
						}
						nFirst = max( 0, nFirst );
					}
					return nFirst;
				}
			}
			return 0;
		}
	}
	else
	{
		RECT rcClient;
		GetClient( rcClient );
		int nFirst = 0;
		if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
		{
			if ( m_VScroll.GetPos() - m_margin.top > 0 )
			{
				nFirst = (m_VScroll.GetPos() - m_margin.top)/m_nRowHeight;
			}
			if ( (*Begin())->m_nId != nFirst )
			{
				for ( Iter _lab = Begin(); _lab!=End(); ++_lab )
				{
					if ( (*_lab)->m_nId == nFirst )
					{
						LPROW pItem = *Begin();
						*Begin() = *_lab;
						*_lab = pItem;
						break;
					}
				}
				if ( (*Begin())->m_nId != nFirst
					&& m_format )
				{
					(*Begin())->m_nId = nFirst;
					m_format( m_pdataBase->Get( nFirst), **Begin() );
				}
				(*Begin())->SetOrg( rcClient.left, nFirst*(m_nRowHeight+m_nLineSpace)+m_margin.top );
			}
			return nFirst;
		}
		else
		{

		}
	}
	return 0;
}

INT32 CUIListCtrl::_LastVisable( INT32 nFirst )
{
	if ( !Size()
		|| nFirst < 0)
	{
		return -1;
	}
	INT32 nLast = nFirst;
	if ( !IsVirtualMode() )
	{
		if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
		{
			if ( m_VScroll.GetPos() - m_margin.top > 0 )
			{
				nLast = nFirst+RECT_H(m_rect)/m_nRowHeight;
				++nLast;
			}
			else
			{
				nLast = (RECT_H(m_rect)-m_VScroll.GetPos())/m_nRowHeight;
				++nLast;
			}
			if ( Size() <= nLast )
			{
				nLast = Size()-1;
			}
			return nLast;
		}
		else
		{
			RECT rcClient;
			GetClient( rcClient );
			while( ++nLast < Size() )
			{
				if( GetRowRect( nLast ).top >= rcClient.bottom )
				{
					--nLast;
					break;
				}
			}
			++nLast;
			nLast = min( Size()-1, nLast );
			return nLast;
		}
	}
	else
	{		
		if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
		{
			RECT rcClient;
			GetClient( rcClient );
			if ( m_VScroll.GetPos() - m_margin.top > 0 )
			{
				nLast = nFirst+RECT_H(m_rect)/m_nRowHeight;
			}
			else
			{
				nLast = (RECT_H(m_rect)-m_VScroll.GetPos())/m_nRowHeight;
			}
			++nLast;
			if ( Size() <= nLast )
			{
				nLast = Size()-1;
			}
			for ( INT32 _i = nFirst+1; _i <= nLast; ++_i )
			{
				if ( (*(Begin()+(_i-nFirst)))->m_nId != _i )
				{
					for ( Iter _lab = (Begin()+(_i-nFirst)); _lab!=End(); ++_lab )
					{
						if ( (*_lab)->m_nId == _i )
						{
							LPROW pItem = *(Begin()+(_i-nFirst));
							*(Begin()+(_i-nFirst)) = *_lab;
							*_lab = pItem;
							break;
						}
					}
					if ( (*(Begin()+(_i-nFirst)))->m_nId != _i
						&& m_format )
					{
						(*(Begin()+(_i-nFirst)))->m_nId = _i;
						m_format( m_pdataBase->Get( _i ), **(Begin()+(_i-nFirst)) );
					}
					(*(Begin()+(_i-nFirst)))->SetOrg( rcClient.left, (*(Begin()+(_i-nFirst-1)))->GetRect().bottom-m_rect.top+m_nLineSpace );
				}
			}
			return nLast;
		}
	}
	return nFirst;
}

void CUIListCtrl::SetRect( const RECT& rcSet )
{
	RECT rcPre;
	GetRect(rcPre);
	__super::SetRect( rcSet );
	if( !Size() )
	{
		return;
	}
	RECT rcNow;
	GetRect( rcNow );
	RECT rcClient;
	GetClient( rcClient );
	if ( RECT_H(rcNow) != RECT_H(rcPre)
		|| RECT_W(rcPre) != RECT_W(rcNow) )
	{
		if ( RECT_W(rcPre) != RECT_W(rcNow) )
		{
			m_VScroll.OnSize();
		}
		if ( RECT_H(rcPre) != RECT_H(rcNow) )
		{
			m_HScroll.OnSize();
		}
		if ( IsVirtualMode( ) )
		{
			GetVirtualModeBufferSize();
			if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
			{
				for ( INT32 _i = m_rgShow.m_min; m_rgShow.m_min>= 0 &&_i <= m_rgShow.m_max; ++_i )
				{
					LPROW pRow = NULL;
					try
					{
						pRow = _GetRow( _i );
						pRow->SetSize( RECT_W(rcClient), m_nRowHeight );
					}
					catch( ... )
					{
					}
				}
			}
			DBChanged( TRUE );
			return;
		}
		_UpdateScroll( m_VScroll.Max(), m_HScroll.Max() );
		_UpdateShow( FALSE ); 
	}
}

void CUIListCtrl::CalcSize( INT32 nIndex )
{
	SIZE size = { 0, 0 };
	Iter _itRow = m_list.Offset( m_list.Begin(), nIndex-(*Begin())->m_nId );
	if ( !IsValid( _itRow ) )
	{
		return;
	}
	RECT rcClient;
	GetClient(rcClient);
	if ( (*_itRow)->IsEmpty() )
	{
		if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
		{
			size.cx = RECT_H(rcClient);
			size.cy = m_nRowHeight;
		}
		SIZE sizePre = (*_itRow)->GetSize();
		if ( sizePre.cx != size.cx
			|| sizePre.cy != size.cy )
		{
			m_VScroll.Max() += (size.cy - sizePre.cy);
			_UpdateScroll( m_VScroll.Max(), m_HScroll.Max() );
			(*_itRow)->SetSize( size );
		}
		return ;
	}
	int nHeight = m_nRowHeight;
	if ( nHeight <=  0 )
	{
		(*_itRow)->CalcSize();
	}
	else
	{
		size.cx = RECT_W(rcClient);
		size.cy = nHeight;
		(*_itRow)->SetSize( size );
	}
	return;
}

void CUIListCtrl::RemoveRow( Iter& _remove )
{
	assert( !IsVirtualMode() );
	if ( IsVirtualMode() )
	{
		return ;
	}
	if ( m_list.IsValid(_remove) )
	{
		RECT rcRow;
		int _height = (*_remove)->GetSize().cy+m_nLineSpace;
		HXITEM _hNext = NULL;
		Iter _itTmp = _remove+1;
		if ( IsValid( _itTmp ) )
		{
			_hNext = *_itTmp;
		}
		m_list.RemoveItem( _remove );
		if ( _hNext )
		{
			Iter _Next = m_list.Hxitem2Iter( _hNext );
			if ( IsValid( _Next ) )
			{
				RECT rcClient;
				GetClient( rcClient );
				POINT ptOrg = { 0, 0 };
				if ( _Next != Begin() )
				{
					(*(_Next-1))->GetRect( rcRow );
					ptOrg.x = rcClient.left;
					ptOrg.y = rcRow.bottom+m_nLineSpace;
				}
				(*_Next)->SetOrg( ptOrg.x, ptOrg.y );
				for ( Iter _lab = _Next+1; _lab != End(); ++_lab )
				{
					(*(_lab-1))->GetRect( rcRow );
					(*_lab)->SetOrg( rcClient.left, rcRow.bottom+m_nLineSpace );
				}
			}
		}
		m_VScroll.Max() -= _height;
		_UpdateScroll( m_VScroll.Max(), m_HScroll.Max() );
		_UpdateShow();
	}
}

CUIListCtrl::Iter CUIListCtrl::InsertRow( Iter& _before, ROW& _row )
{
	assert( !IsVirtualMode() );
	if ( IsVirtualMode() )
	{
		return End();
	}
	RECT rcRow;
	Iter _pInsert = m_list.InsertItem( _before, _row );
	if ( _pInsert != End() )
	{
		LPROW pRow = *_pInsert;
		if ( pRow )
		{
			if ( _pInsert != Begin() )
			{
				Iter _pre = --_pInsert;
				(*_pre)->GetRect( rcRow );
				pRow->SetOrg(0, rcRow.bottom-m_rect.top+m_nLineSpace);
			}
			else
			{
				pRow->SetOrg(0,m_margin.top);
			}
			if ( pRow->m_nId >= 0 )
			{
				CalcSize( pRow->m_nId );
			}
			for ( Iter _lab = _pInsert+1; _lab != End(); ++_lab )
			{
				(*(_lab-1))->GetRect( rcRow );
				(*_lab)->SetOrg( m_margin.left, rcRow.bottom-rcRow.bottom+m_nLineSpace );
			}
			_UpdateScroll( m_VScroll.Max()+pRow->GetSize().cy + m_nLineSpace, m_HScroll.Max() );
			_UpdateShow();
		}
	}

	return _pInsert;
}

CUIListCtrl::LPROW CUIListCtrl::InsertRow( const ROW& _row )
{
	assert( !IsVirtualMode() );
	if ( IsVirtualMode() )
	{
		return NULL;
	}
	POINT ptOrg = { m_margin.left, m_margin.top };
	RECT rcRow;
	if ( Size() )
	{
		m_list.Last()->GetRect(rcRow);
		ptOrg.y = rcRow.bottom-m_rect.top+m_nLineSpace;
	}
	else
	{
		m_VScroll.Max() = m_margin.top+m_margin.bottom;
	}
	LPROW _pInsert = m_list.InsertItem( _row );
	if ( _pInsert )
	{
		if ( _pInsert->m_nId >= 0 )
		{
			CalcSize( _pInsert->m_nId );
		}
		_pInsert->SetOrg(ptOrg.x, ptOrg.y);
		m_VScroll.Max() += _pInsert->GetSize().cy + m_nLineSpace;
		_UpdateScroll( m_VScroll.Max(), m_HScroll.Max() );
		_UpdateShow();
	}
	return _pInsert;
}
INT32 CUIListCtrl::_GetMouseScrollLines()
{
	INT32 nScrollLines = 3;            //默认为三行
#ifndef _WIN32_WCE
	//windows 95　之前，需要从注册表中读取
	OSVERSIONINFO VersionInfo;
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&VersionInfo) || 
		(VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && VersionInfo.dwMinorVersion == 0))
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Row\\Desktop"),
			0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			TCHAR szData[128];
			DWORD dwKeyDataType;
			DWORD dwDataBufSize = sizeof(szData);

			if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
				(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
			{
				nScrollLines = _tcstoul(szData, NULL, 10);
			}
			RegCloseKey(hKey);
		}
	}
	// win98及之后版本
	else
	{
		SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
	}
#endif
	return nScrollLines;
}

void CUIListCtrl::ScrollTo( IN INT32 nIndex )
{
	if ( nIndex < 0 || nIndex >= Size() )
	{
		return ;
	}

	RECT rcClient;
	GetClient( rcClient );
	INT32 nShowBeg = 0;
	INT32 nShowPos = 0;

	if ( m_nRowHeight == RL_AUTOPANELHEIGHT )
	{
		LPROW pRow = GetAt( nIndex );
		if ( pRow )
		{
			RECT rcRow;
			pRow->GetRect( rcRow );
			nShowPos = rcRow.top;
		}
	}
	else
	{
		nShowPos = nIndex*(m_nRowHeight+m_nLineSpace);
	}
	if( nShowPos + RECT_H(rcClient) > m_VScroll.Max() )
	{
		nShowPos = m_VScroll.Max() - RECT_H(rcClient);
	}
	if ( nShowPos < 0 )
	{
		nShowPos = 0;
	}
	if ( nShowPos == GetScrollPos( VERT ) )
	{
		return;
	}
	SetScrollPos( VERT, nShowPos );
	_UpdateShow();
	return;
}

INT32 CUIListCtrl::GetVirtualModeBufferSize()
{
	int nBufferSize = (int)m_list.Size();
	INT32 nSize = 0;
	RECT rcClient;
	GetClient( rcClient );
	if ( m_nRowHeight > 0
		&& IsVirtualMode() )
	{
		nSize = (RECT_H(rcClient))/(m_nRowHeight+m_nLineSpace);
		if ( RECT_H(rcClient)%(m_nRowHeight+m_nLineSpace))
		{
			++nSize;
		}
	}
	//预留两个存储空间
	nSize += 2;
	if ( nBufferSize < nSize )
	{
		LPROW pTemplate = m_pRowTemplate;
		ROW rowTmp;
		if ( !pTemplate )
		{
			pTemplate = new ROW;
		}
		//根据模板，生成Buffer
		try
		{
			SIZE size = {RECT_W(rcClient),m_nRowHeight};
			for ( int i = 0; i < nSize - nBufferSize; ++i )
			{
				LPROW pRow = new ROW( *pTemplate );
				if ( RL_AUTOPANELHEIGHT != m_nRowHeight )
				{
					pRow->SetSize( size );
				}
				pRow->m_nId = -1;
				m_list.InsertItem(pRow);
			}
			nBufferSize = nSize;
		}
		catch( ... )
		{
			return FALSE;
		}
	}
	else if( nSize > nBufferSize )
	{
		for ( int i = 0; i < nSize - nBufferSize; ++i )
		{
			m_list.RemoveItem( m_list.End() );
		}
		nBufferSize = nSize;
		//OnDBChange(0, 0);
	}
	return nSize;
}
