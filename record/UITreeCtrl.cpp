#include ".\UITreeCtrl.h"
#include "UIText.h"

inline int Deep( const CUITreeCtrl::NODE& node )
{
	int iDeep = 0;
	CUITreeCtrl::HNODE hNode = node.m_parent;
	while( hNode )
	{
		++iDeep;
		hNode = hNode->m_parent;
	}
	return iDeep;
}

BOOL CUITreeCtrl::Create( IN const RECT& rect, IN IUIWindow* parent, IN UINT32 nID, IUIBase* pInsertBefore /*= NULL*/ )
{
	return __super::Create( rect, parent, nID, pInsertBefore );
}

void CUITreeCtrl::OnDraw(  IPainter* painter )
{
	__super::OnDraw( painter );
	if ( !m_hFirstVisable )
	{
		m_hFirstVisable = FoundFirstVisable();
	}
	HNODE hVisable = m_hFirstVisable;
	while( hVisable )
	{
		RECT rcNode;
		hVisable->m_nodeData.GetRect( rcNode );
		if ( painter->RectVisible( &rcNode ) )
		{
			hVisable->m_nodeData.SetState( RRS_NORMAL );
			if ( hVisable == m_hHotNode )
			{
				hVisable->m_nodeData.SetState( RRS_HOT );
			}
			if ( hVisable == m_hFocusNode )
			{
				hVisable->m_nodeData.SetState( RRS_FOCUSED|hVisable->m_nodeData.GetState() );
			}
			if ( hVisable == m_hSelected )
			{
				hVisable->m_nodeData.SetState( RRS_SELECTED|hVisable->m_nodeData.GetState() );
			}	
			RECT rcClient;
			GetClient(rcClient);
			RECT rcNode;
			hVisable->m_nodeData.GetRect( rcNode );
			if ( rcNode.right != rcClient.right )
			{
				hVisable->m_nodeData.SetSize( rcClient.right-rcNode.left, RECT_H(rcNode) );
			}
			EraseNodeBkGnd( hVisable, painter );
			hVisable->m_nodeData.OnDraw( painter );
		}
		hVisable = FoundNextVisableIn( hVisable );
	}
}

void CUITreeCtrl::_UpdateScroll( int nMaxVScroll /*= 1*/, int nMaxHScroll /*= 1*/ )
{
	RECT rcThis;
	GetClient( rcThis );
// 	if ( RECT_ISEMPTY(rcThis) )
// 	{
// 		return;
// 	}
	if ( GetStyle() & XWS_HSCROLL )
	{
		m_HScroll.SetRange( 0, nMaxHScroll );	
		if ( m_HScroll.Max() > RECT_W(rcThis) )
		{
			m_HScroll.Show( true );
		}
		else
		{
			m_HScroll.Show( false );
		}
	}

	if( GetStyle() & XWS_VSCROLL )
	{
		m_VScroll.SetRange( 0, nMaxVScroll );		
		if ( m_VScroll.Max() > RECT_H(rcThis) )
		{
			if ( !m_VScroll.IsVisable() )
			{
				m_VScroll.Show( true );
			}
		}
		else
		{
			m_VScroll.SetPos( 0 );
			m_VScroll.Show( false );
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
		m_HScroll.SetPos( m_HScroll.Max()-RECT_W(rcThis) );
	}
}

bool CUITreeCtrl::IsNodeVisable( const NODE& node, DWORD dwParam )
{
	if ( NULL != node.m_parent 
		&& !node.m_parent->m_bExpand )
	{
		return false;
	}
	if ( !node.m_nodeData.IsVisable() )
	{
		return false;
	}
	CUITreeCtrl* pThis = (CUITreeCtrl*)(DWORD_PTR)dwParam;
	if ( pThis )
	{
		RECT rcClient;
		pThis->GetClient( rcClient );
		RECT rcNode;
		node.m_nodeData.GetRect( rcNode );
		if ( rcClient.top > rcNode.bottom
			|| rcClient.bottom < rcNode.top )
		{
			return false;
		}
		return true;
	}
	return false;
}

bool CUITreeCtrl::IsNodeNotVisable( const NODE& node, DWORD dwParam )
{
// 	if ( node.m_nodeData.GetXCtrl( 1001 ) )
// 	{
// 		TRACE( node.m_nodeData.GetXCtrl( 1001 )->GetText().c_str() );
// 		TRACE( "\n" );
// 	}

	if ( NULL != node.m_parent 
		&& !node.m_parent->m_bExpand )
	{
		return false;
	}
	if ( !node.m_nodeData.IsVisable() )
	{
		return false;
	}
	CUITreeCtrl* pThis = (CUITreeCtrl*)(DWORD_PTR)dwParam;
	if ( pThis )
	{
		RECT rcClient;
		pThis->GetClient( rcClient );
		RECT rcNode;
		node.m_nodeData.GetRect( rcNode );
		if ( rcClient.top >= rcNode.bottom
			|| rcClient.bottom <= rcNode.top )
		{
			return true;
		}
		return false;
	}
	return false;
}

bool CUITreeCtrl::PtInNode( const NODE& node, DWORD dwParam )
{
	POINT* point = (POINT*)(DWORD_PTR)dwParam;
	if ( point )
	{
		if( ::PtInRect( &node.m_nodeData.GetRect(  ), *point ) )
		{
			return true;
		}
	}
	return false;
}

void CUITreeCtrl::ChangingRect( NODE& node, DWORD dwParam )
{
	CUITreeCtrl* pThis = (CUITreeCtrl*)(DWORD_PTR)dwParam;
	if ( pThis )
	{
		int IndentationX = 0;
		if ( pThis->m_IndentationX > 0 )
		{
			IndentationX = Deep( node )*pThis->m_IndentationX;
		}
		RECT rcNode;
		node.m_nodeData.GetRect( rcNode );
		RECT_TO(rcNode, pThis->m_margin.left+IndentationX, pThis->m_nHelpY );
		node.m_nodeData.SetRect( rcNode );
		pThis->m_nHelpY += RECT_H(rcNode)+pThis->m_nLineSpace;
	}
}

CUITreeCtrl::HNODE CUITreeCtrl::FoundNextVisableIn( const HNODE& hNode )
{
	if ( !hNode )
	{
		return NULL;
	}
	HNODE hVisable;
	//在子节点中查找
	if( hNode->m_children 
		&& hNode->m_bExpand )
	{
		hVisable = m_tree.FindNode( hNode->m_children, CUITreeCtrl::IsNodeVisable, (DWORD)(DWORD_PTR)this, true );
		if ( hVisable )
		{
			return hVisable;
		}
	}

	//在兄弟节点中查找
	if ( hNode->m_next )
	{
		hVisable = m_tree.FindNode( hNode->m_next, CUITreeCtrl::IsNodeVisable, (DWORD)(DWORD_PTR)this, true ); 
		if ( hVisable )
		{
			return hVisable;
		}
	}

	//在父节点的兄弟节点中查找
	HNODE hParent = hNode->m_parent;
	while ( hParent )
	{
		hVisable = m_tree.FindNode( hParent->m_next, CUITreeCtrl::IsNodeVisable, (DWORD)(DWORD_PTR)this, true ); 
		if ( hVisable )
		{
			return hVisable;
		}
		hParent = hParent->m_parent;
	}
	return NULL;
}

CUITreeCtrl::HNODE CUITreeCtrl::FoundPreNotVisableIn( const HNODE& hNode )
{
	return m_tree.ReverseFindNode( hNode, CUITreeCtrl::IsNodeNotVisable, (DWORD)(DWORD_PTR)this, true );
}

CUITreeCtrl::HNODE CUITreeCtrl::GetNodeFromPoint( POINT point )
{
	if ( !m_hFirstVisable )
	{
		return NULL;
	}
	if ( PtInNode( *m_hFirstVisable, (DWORD)(DWORD_PTR)&point  ) )
	{
		return m_hFirstVisable;
	}
	HNODE hVisable;
	//在子节点中查找
	if( m_hFirstVisable->m_children 
		&& m_hFirstVisable->m_bExpand )
	{
		hVisable = m_tree.FindNode( m_hFirstVisable->m_children, CUITreeCtrl::PtInNode, (DWORD)(DWORD_PTR)&point, true );
		if ( hVisable )
		{
			return hVisable;
		}
	}
	//在兄弟节点中查找
	if ( m_hFirstVisable->m_next )
	{
		hVisable = m_tree.FindNode( m_hFirstVisable->m_next,CUITreeCtrl::PtInNode, (DWORD)(DWORD_PTR)&point, true ); 
		if ( hVisable )
		{
			return hVisable;
		}
	}

	//在父节点的兄弟节点中查找
	HNODE hParent = m_hFirstVisable->m_parent;
	while ( hParent )
	{
		hVisable = m_tree.FindNode( hParent->m_next, CUITreeCtrl::PtInNode, (DWORD)(DWORD_PTR)&point, true ); 
		if ( hVisable )
		{
			return hVisable;
		}
		hParent = hParent->m_parent;
	}
	return NULL;
}

BOOL CUITreeCtrl::HorzScroll(int nSBCode, int nPos )
{
	RECT rcClient;
	GetRect( rcClient );
	if ( m_HScroll.Max() <= RECT_W(rcClient) )
	{
		return TRUE;
	}
	return TRUE;
}

BOOL CUITreeCtrl::VertScroll( int nSBCode, int nPos, int iDrect )
{
	nSBCode;
	nPos;
	RECT rcClient;
	GetRect( rcClient );
	if ( m_VScroll.Max() < RECT_H(rcClient) )
	{
		return TRUE;
	}
	if( THUMB == nSBCode )
	{
		if ( iDrect > 0 )
		{
			if( IsNodeNotVisable( *m_hFirstVisable, (DWORD)(DWORD_PTR)this ) )
			{
				m_hFirstVisable = FoundNextVisableIn( m_hFirstVisable );
			}
		}
		else
		{
			HNODE hUnVisable = FoundPreNotVisableIn( m_hFirstVisable );
			if ( hUnVisable )
			{
				m_hFirstVisable = FoundNextVisableIn( hUnVisable );
			}
			if ( NULL == hUnVisable
				|| NULL == m_hFirstVisable )
			{
				m_hFirstVisable = FoundFirstVisable();
			}
		}
		assert( NULL != m_hFirstVisable );
		InvalidRect();
	}
	return TRUE;
}


BOOL CUITreeCtrl::MouseWheel(int zDelta)
{
	return __super::MouseWheel( zDelta );
}

BOOL CUITreeCtrl::MouseMoving(POINT point)
{
	if( __super::MouseMoving( point ) )
	{
		return TRUE;
	}
	POINT _point = point;
	_point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( _point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{		
			pItem->MouseMoving( _point );
			if ( NULL == m_hHotNode || m_hHotNode != hNode )
			{
				RECT _rcPre;
				RECT_EMPTY( _rcPre );
				if ( m_hHotNode )
				{
					m_hHotNode->m_nodeData.GetRect( _rcPre );
				}
				m_hHotNode = hNode;
				RECT rcInvalidate;
				pItem->GetRect( rcInvalidate );
				if ( !RECT_ISEMPTY(_rcPre) )
				{
					::UnionRect( &rcInvalidate, &pItem->GetRect(), &_rcPre );
				}
				InvalidRect( &rcInvalidate );
			}

			if ( m_hMouseIn != hNode )
			{
				if ( m_hMouseIn )
				{
					m_hMouseIn->m_nodeData.MouseLeave();
				}
				m_hMouseIn = hNode;
				pItem->MouseIn();
			}
		}
	}
	else
	{
		if( m_hMouseIn )
		{
			m_hMouseIn->m_nodeData.MouseLeave();
		}
		m_hMouseIn = NULL;
		if ( m_hHotNode )
		{	
			RECT rcInvalidate;
			m_hHotNode->m_nodeData.GetRect( rcInvalidate );
			m_hHotNode = NULL;
			InvalidRect( &rcInvalidate );
		}
		_Notify( XLAY_MOUSEMOVE, point );
	}
	return TRUE;
}


BOOL CUITreeCtrl::CursorIn( POINT point )
{
	BOOL bRet = FALSE;
	if ( !m_tree.IsEmpty() )
	{
		POINT _point = point;
		_point.y+= m_VScroll.GetPos();
		HNODE hNode = GetNodeFromPoint( point );
		if ( !hNode )
		{
			return FALSE;
		}
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			bRet = pItem->CursorIn( _point );
		}
	}
	if ( !bRet )
	{
		return __super::CursorIn( point );
	}
	return TRUE;
}


BOOL CUITreeCtrl::MouseLBUp(POINT point )
{
	if( __super::MouseLBUp( point ) )
	{
		return TRUE;
	}
	point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			return pItem->MouseLBUp( point );
		}
	}
	_Notify( XLAY_LBUP, point );
	return TRUE;
}


BOOL CUITreeCtrl::MouseLDBClick(POINT point )
{
	if( __super::MouseLDBClick( point ) )
	{
		return TRUE;
	}
	point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			if ( m_bDbClickExpand )
			{
				Expand( hNode, !hNode->m_bExpand );
			}
			else
			{
				pItem->InvalidRect();
			}	
			pItem->MouseLDBClick( point );
			return TRUE;
		}
	}
	_Notify( XLAY_LDBCLICK, point );
	return TRUE;
}


BOOL CUITreeCtrl::MouseRBUp(POINT point )
{
	if( __super::MouseRBUp( point ) )
	{
		return TRUE;
	}
	point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			return pItem->MouseRBUp( point );
		}
	}
	_Notify( XLAY_RBUP, point );
	return TRUE;
}


BOOL CUITreeCtrl::MouseRDBClick(POINT point )
{
	if( __super::MouseRDBClick( point ) )
	{
		return TRUE;
	}
	point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			return pItem->MouseRDBClick( point );
		}	
	}
	_Notify( XLAY_RDBCLICK, point );
	return TRUE;
}

BOOL CUITreeCtrl::MouseLBDown(POINT point)
{
	if( __super::MouseLBDown( point ) )
	{
		return TRUE;
	}
	point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			if ( m_hFocusNode != hNode )
			{
				m_hFocusNode = hNode;
			}
			if ( m_hHotNode != hNode )
			{
				m_hHotNode = hNode;
			}
			if ( m_hSelected != hNode )
			{
				RECT rcPre= { 0,0,0,0 };
				if ( m_hSelected )
				{
					m_hSelected->m_nodeData.GetRect(rcPre);
				}
				m_hSelected = hNode;
				RECT rcInvalidate;
				m_hSelected->m_nodeData.GetRect(rcInvalidate);
				if ( !RECT_ISEMPTY(rcPre) )
				{
					::UnionRect( &rcInvalidate, &rcInvalidate, &rcPre );
				}
				InvalidRect(&rcInvalidate);
			}
			pItem->MouseLBDown( point );
		}
	}
	else
	{
		RECT rcInvalidate = { 0,0,0,0 };
		RECT rc1= { 0,0,0,0 };
		if ( m_hHotNode )
		{
			m_hHotNode->m_nodeData.GetRect( rc1 );
			m_hHotNode   = NULL;
		}
		if ( m_hSelected )
		{
			::UnionRect( &rcInvalidate, &m_hSelected->m_nodeData.GetRect( ), &rc1 );
			rc1 = rcInvalidate;
			m_hSelected   = NULL;
		}
		if ( m_hFocusNode )
		{
			::UnionRect( &rcInvalidate, &m_hFocusNode->m_nodeData.GetRect( ), &rc1 );
			m_hFocusNode   = NULL;
		}
		InvalidRect(&rcInvalidate);
		_Notify( XLAY_LBDOWN, point );
	}
	return TRUE;
}

BOOL CUITreeCtrl::MouseRBDown(POINT point)
{
	if( __super::MouseRBDown( point ))
	{
		return TRUE;
	}
	BOOL bRet = FALSE;
	point.y+= m_VScroll.GetPos();
	HNODE hNode = GetNodeFromPoint( point );
	if ( hNode )
	{
		NODEDATA* pItem = &hNode->m_nodeData;
		if ( pItem )
		{
			if ( m_hFocusNode != hNode )
			{
				m_hFocusNode = hNode;
			}
			if ( m_hHotNode != hNode )
			{
				m_hHotNode = hNode;
			}
			if ( m_hSelected != hNode )
			{
				RECT rcPre = { 0,0,0,0 };
				if ( m_hSelected )
				{
					m_hSelected->m_nodeData.GetRect(rcPre);
				}
				m_hSelected = hNode;
				RECT rcInvalidate;
				m_hSelected->m_nodeData.GetRect(rcInvalidate);
				if ( !RECT_ISEMPTY(rcPre) )
				{
					::UnionRect( &rcInvalidate, &rcInvalidate, &rcPre );
				}
				InvalidRect(&rcInvalidate);
			}
			return pItem->MouseRBDown( point );
		}
	}
	else
	{
		RECT rcInvalidate= { 0,0,0,0 };
		RECT rc1= { 0,0,0,0 };
		if ( m_hHotNode )
		{
			m_hHotNode->m_nodeData.GetRect( rc1 );
			m_hHotNode   = NULL;
		}
		if ( m_hSelected )
		{
			::UnionRect( &rcInvalidate, &m_hSelected->m_nodeData.GetRect( ), &rc1 );
			rc1 = rcInvalidate;
			m_hSelected   = NULL;
		}
		if ( m_hFocusNode )
		{
			::UnionRect( &rcInvalidate, &m_hFocusNode->m_nodeData.GetRect( ), &rc1 );
			m_hFocusNode   = NULL;
		}
		InvalidRect(&rcInvalidate);
		_Notify( XLAY_RBDOWN, point );
	}
	return TRUE;
}


void CUITreeCtrl::MouseLeave(  )
{
	//如果记录列表为空,做空操作
	__super::MouseLeave(  );
	if( m_hMouseIn )
	{
		m_hMouseIn->m_nodeData.MouseLeave();
	}
	m_hMouseIn = NULL;
	if ( m_hHotNode )
	{
		RECT rcItem;
		m_hHotNode->m_nodeData.GetRect( rcItem );
		m_hHotNode = NULL;
		InvalidRect( &rcItem );
	}
	return;
}
void CUITreeCtrl::EraseNodeBkGnd( HNODE& hNode,  IPainter* painter )
{
	NODEDATA* pNode = &hNode->m_nodeData;
	if ( pNode )
	{
		RECT rcRow;
		pNode->GetRect( rcRow );
		IMG* pImage = NULL;
		//绘制活动行的背景色
		if ( m_hHotNode == hNode
			&& (m_imageHot.hImage|| X_TRANSPARENT != m_imageHot.imageAvgClr ) )
		{
			pImage = &m_imageHot;
		}
		//绘制选中行的背景色
		else if( m_hSelected == hNode )
		{
			pImage = &m_imageSelect;
		}
		if( NULL == pImage
			|| ( pImage 
			&& pImage->imageAvgClr == X_TRANSPARENT
			&& NULL == pImage->hImage ) )
		{
			pImage = hNode->m_nodeData.GetBkGnd();
		}
		if ( pImage )
		{
			if ( pImage->hImage )
			{
				painter->DrawImage( pImage, &rcRow );
			}
			else if( X_TRANSPARENT != pImage->imageAvgClr )
			{
				ARGB clrOld = painter->SetBrushColor( pImage->imageAvgClr );
				HRGN h_rgn = CreateRoundRectRgn(rcRow.left,rcRow.top,rcRow.right,rcRow.bottom,5,5);
				painter->FillRegion( h_rgn );
				painter->SetBrushColor( clrOld );
				DeleteObject( h_rgn );
			}
		}
		if ( IsImageValid(&m_imageLineSpace) )
		{
			RECT rcLineSpace = {rcRow.left, rcRow.bottom, rcRow.right, rcRow.bottom + m_nLineSpace};
			painter->DrawImage( &m_imageLineSpace, &rcLineSpace );
		}
	}
}

void CUITreeCtrl::Show( bool bShow )
{
	__super::Show( bShow );
	if ( bShow 
		&& !IsEmpty()
		&& NULL == m_hFirstVisable )
	{
		ChangeRect();
	}
	/*else
	{
		if( IsVisable() )
		{
			RECT rcTmp = m_rect;
			RECT rcTo = m_rect;
			bool bScroll = m_VScroll.IsVisable();
			DWORD style = GetStyle();
			SetStyle( style&(~XWS_VSCROLL));
			m_VScroll.Show( false );
			int nWidth = rcTmp.Width() < 30?rcTmp.Width():30;
			rcTmp.MoveToX( rcTmp.left+nWidth );
			for( int nStep = 0; ;nStep += nWidth )
			{
				rcTmp.MoveToX( rcTmp.left-nStep );
				if( rcTmp.right <= rcTo.right )
				{
					rcTmp.MoveToX( rcTo.left );
				}
				if( rcTmp.right != rcTo.right )
				{
					SetRect( rcTmp );
				}
				else
				{
					SetStyle( style );
					m_VScroll.Show( true );
					SetRect( rcTmp );
					InvalidRect();
					break;
				}
				InvalidRect();
			}
		}
	}*/
}

BOOL CUITreeCtrl::Notify( UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( XLAY_LCLICK == message
		&& !m_bDbClickExpand )
	{
		NM_PARAM* param = (NM_PARAM*)lParam;
		if ( param )
		{
			//展开节点
			HNODE hNode = GetNodeFromPoint( param->point );
			if ( hNode )
			{
				Expand( hNode, !hNode->m_bExpand );
			}
		}
	}
	return __super::Notify( message, wParam, lParam );
}

void CUITreeCtrl::Expand( HNODE& hNode, bool bExpand /*= true */ )
{
	if ( hNode )
	{
		if( hNode->m_bExpand == bExpand )
		{
			return;
		}
		hNode->m_bExpand = bExpand;
		IUICtrl* pCtrl = NULL;
		if ( hNode->m_bExpand )
		{	
			if ( m_nXCtrlIdExpandShow > 0 )
			{
				pCtrl = hNode->m_nodeData.GetXCtrl( m_nXCtrlIdExpandShow );
				if ( pCtrl )
				{
					pCtrl->Show();
				}
			}
			if ( m_nXCtrlIdUnexpandShow > 0 )
			{
				pCtrl = hNode->m_nodeData.GetXCtrl( m_nXCtrlIdUnexpandShow );
				if ( pCtrl )
				{
					pCtrl->Show( false );
				}
			}
		}
		else
		{
			if ( m_nXCtrlIdExpandShow > 0 )
			{
				pCtrl = hNode->m_nodeData.GetXCtrl( m_nXCtrlIdExpandShow );
				if ( pCtrl )
				{
					pCtrl->Show( false );
				}
			}
			if ( m_nXCtrlIdUnexpandShow > 0 )
			{
				pCtrl = hNode->m_nodeData.GetXCtrl( m_nXCtrlIdUnexpandShow );
				if ( pCtrl )
				{
					pCtrl->Show( true );
				}
			}
		}
		if ( hNode->m_children )
		{
			ChangeRect( );
		}
		else
		{
			hNode->m_nodeData.InvalidRect();
		}
	}
	return ;
}

void CUITreeCtrl::ShowNode( HNODE& hNode, bool bShow /*= true */ )
{
	if( bShow )
	{
		if ( !hNode->m_nodeData.IsVisable() )
		{
			bool bIsVisable = hNode->IsVisable();
			hNode->m_nodeData.Show( true );
			if( hNode->IsVisable() )
			{
				ChangeRect();
			}
		}
	}
	else
	{
		if ( hNode->m_nodeData.IsVisable() )
		{
			bool bIsVisable = hNode->IsVisable();
			hNode->m_nodeData.Show( false );
			if( bIsVisable )
			{
				ChangeRect();
			}
		}
	}
}

CUITreeCtrl::HNODE CUITreeCtrl::Remove( HNODE& hRemove, bool bFreeChildren /*= TRUE */ )
{
	if ( NULL == hRemove  )
	{
		return NULL;
	}
	if ( m_hSelected == hRemove )
	{
		m_hSelected = NULL;
	}
	if ( m_hHotNode == hRemove )
	{
		m_hHotNode = NULL;
	}
	if ( m_hFocusNode == hRemove )
	{
		m_hFocusNode = NULL;
	}
	bool bIsVisable = hRemove->IsVisable();
	HNODE hChildren = m_tree.RemoveNode( hRemove, bFreeChildren );
	if ( bIsVisable )
	{
		ChangeRect();
	}
	return hChildren;
}