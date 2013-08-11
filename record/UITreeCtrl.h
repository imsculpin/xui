#ifndef __UI_TREECTRL_H__
#define __UI_TREECTRL_H__
#include "UIView.h"
#include "UITree.h"
#include "UIText.h"
#include <WindowsX.h>

class CUITreeCtrl: public CUIView
{
public:
	typedef XNODEDATA  NODEDATA;
	typedef CUITreeNode NODE;
	typedef HXTREENODE HNODE;

public:
	CUITreeCtrl(void)
		:m_nLineSpace(0)
		,m_IndentationX(0)
		,m_bDbClickExpand(false)
		,m_nXCtrlIdUnexpandShow(-1)
		,m_nXCtrlIdExpandShow(-1)
	{
		
	}

	~CUITreeCtrl(void)
	{

	}
	
	void SetRect( const RECT& rcSet )
	{
		__super::SetRect( rcSet );
		_UpdateScroll( m_VScroll.Max(), m_HScroll.Max() );
	}

	HNODE Insert( const NODEDATA& _data, HNODE _before = NULL, HNODE _parent = NULL )
	{
		HNODE hNode =  m_tree.InsertNode( _data, _before, _parent );
		if ( NULL != hNode )
		{
			hNode->m_nodeData.m_nId = 0;
			hNode->m_nodeData.SetParent( this );
			if ( NULL == hNode->m_parent
				|| (NULL!=hNode->m_parent
				&& hNode->m_parent->m_bExpand )
				)
			{
				ChangeRect();
			}
		}
		return hNode;
	}

	HNODE Insert( const HNODE& _node, HNODE _before = NULL, HNODE _parent = NULL )
	{
		return Insert( _node->m_nodeData, _before, _parent );
	}

	HNODE InsertTree( const HNODE& hRoot, HNODE _before = NULL, HNODE _parent = NULL )
	{
		HNODE hNode = m_tree.InsertTree( hRoot, _before, _parent );
		if ( NULL == hNode->m_parent
			|| (NULL!=hNode->m_parent
			&& hNode->m_parent->m_bExpand )
			)
		{
			ChangeRect();
		}
	}

	HNODE Remove( HNODE& hRemove, bool bFreeChildren = TRUE );

	void RemoveAll(  )
	{
		if ( !m_tree.IsEmpty())
		{
			m_tree.RemoveAll();
			m_hHotNode   = NULL;
			m_hSelected  = NULL;
			m_hFocusNode = NULL;
			m_hMouseIn   = NULL;
			ChangeRect();
		}
	}

	void ShowNode( HNODE& hNode, bool bShow = true );

	/*Fun原型:bool (*)(NODE&,DWORD)*/
	template <class Fun>
	HNODE Find( Fun funcation, DWORD dwParam, bool bOnlyInVisable = false )
	{
		return m_tree.FindNode( funcation, dwParam, bOnlyInVisable );
	}

	/*Fun原型:bool (*)(NODE&,DWORD)*/
	template <class Fun>
	HNODE Find( HNODE hNodeStart, Fun funcation, DWORD dwParam, bool bOnlyInVisable = false )
	{
		return m_tree.FindNode( hNodeStart, funcation, dwParam, bOnlyInVisable );
	}

	/*Fun原型:T (*)(NODE&,DWORD)*/
	template <class Fun>
	void Enum( Fun funcation, DWORD dwParam, bool bOnlyInVisable = false )
	{
		m_tree.EnumNode( funcation, dwParam, bOnlyInVisable );
	}

	/*Fun原型:T (*)(NODE&,DWORD)*/
	template <class Fun>
	void Enum( HNODE hNodeStart, Fun funcation, DWORD dwParam, bool bOnlyInVisable = false )
	{
		m_tree.EnumNode( hNodeStart, funcation, dwParam, bOnlyInVisable );
	}
	
	/*!< 创建List */
	BOOL Create( IN const RECT& rect, IUIWindow* parent, IN UINT32 nID, IUIBase* pInsertBefore = NULL );

	void SetIndentationX( UINT32 IndentationX )
	{
		if( m_IndentationX != IndentationX )
		{
			m_IndentationX = IndentationX; 
			ChangeRect();
		}
	}

	//******************************************************************************
	// 方    法: CUITreeCtrl::SetHotNodeBkGnd
	// 访问权限: public  
	// 参    数: IN HBITMAP hHotBk
	// 返 回 值: HBITMAP
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetHotNodeBkGnd( IN const IMG& image );

	//******************************************************************************
	// 方    法: CUITreeCtrl::SetSelectNodeBkGnd
	// 访问权限: public  
	// 参    数: IN HBITMAP hSelectBk
	// 返 回 值: HBITMAP
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetSelectNodeBkGnd( IN const IMG& image );

	//******************************************************************************
	// 方    法: CUITreeCtrl::SetBkGnd
	// 访问权限: public  
	// 参    数: IN ARGB clrErase
	// 返 回 值: ARGB
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetLineSpaceImage( const IMG& imagePtr ){ m_imageLineSpace = imagePtr;}

	//******************************************************************************
	// 方    法: CUITreeCtrl::SetExpandImage
	// 访问权限: public  
	// 参    数: IN ARGB clrErase
	// 返 回 值: ARGB
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	void SetExpandAttrib( INT32 nXCtrlIdExpandShow, INT32 nXCtrlIdUnExpandShow )
	{
		m_nXCtrlIdExpandShow = nXCtrlIdExpandShow;
		m_nXCtrlIdUnexpandShow = nXCtrlIdUnExpandShow;
	}
	
	void EnableDbClickExpand( bool bEnable = false )
	{
		m_bDbClickExpand = bEnable;
	}

	bool IsEmpty()
	{
		return m_tree.IsEmpty();
	}

	void Show( bool bShow );
	HNODE GetNodeFromPoint( POINT point );

	bool HasChildren( const HNODE& hNode )const
	{
		if ( hNode )
		{
			return NULL != hNode->m_children;
		}
		return false;
	}

	NODEDATA* GetNodeData( HNODE& hNode )const
	{
		if( hNode )
		{
			return &(hNode->m_nodeData);
		}
		return NULL;
	}

	bool IsExpand( HNODE& hNode )const
	{
		if( hNode )
		{
			return hNode->m_bExpand;
		}
		return false;
	}
	
	void Expand( HNODE& hNode, bool bExpand = true );
	HNODE GetParentNode( HNODE& hNode )const
	{
		if( hNode )
		{
			return hNode->m_parent;
		}
		return NULL;
	}

	HNODE GetNextBrother( HNODE& hNode )const
	{
		if ( hNode )
		{
			return hNode->m_next;
		}
		return NULL;
	}

	HNODE GetPreBrother( HNODE& hNode )const
	{
		if ( hNode )
		{
			return hNode->m_pre;
		}
		return NULL;
	}

	HNODE GetFirstChild( HNODE& hNode )const
	{
		if ( hNode )
		{
			return hNode->m_children;
		}
		return NULL;
	}

	HNODE GetRoot()const
	{
		return m_tree.GetRoot();
	}
	
	long CountChildren( HNODE hNode )
	{
		if ( hNode )
		{
			int i = 0;
			HNODE hTmp = hNode->m_children;
			while( hTmp )
			{
				++i;
				hTmp = hTmp->m_next;
			}
			return i;
		}
		return 0;
	}
protected:
	void EraseNodeBkGnd( HNODE& pNode,  IPainter* painter );
	void OnDraw(  IPainter* painter );
	static bool IsNodeVisable( const NODE& node, DWORD dwParam );
	static bool IsNodeNotVisable( const NODE& node, DWORD dwParam );
	static void ChangingRect( NODE& node, DWORD dwParam );
	static bool PtInNode( const NODE& node, DWORD dwParam );
	HNODE FoundNextVisableIn( const HNODE& hNode );
	HNODE FoundPreNotVisableIn( const HNODE& hNode );
	HNODE FoundFirstVisable( )
	{
		return m_tree.FindNode( CUITreeCtrl::IsNodeVisable, (DWORD)(DWORD_PTR)this, true );
	}
	HNODE FoundLastVisable()
	{
		HNODE hVisable = m_hFirstVisable;
		HNODE hLast = m_hFirstVisable;
		while( hVisable )
		{
			hLast = hVisable;
			hVisable = FoundNextVisableIn( hLast );
		}
		return hLast;
	}
	void ChangeRect( )
	{
		m_nHelpY = m_margin.top;
		m_tree.EnumNode( CUITreeCtrl::ChangingRect, (DWORD)(DWORD_PTR)this, true );
		_UpdateScroll( m_nHelpY+ m_margin.bottom );
		m_nHelpY = m_margin.top;
		m_hFirstVisable = FoundFirstVisable();
		InvalidRect();
	}

	BOOL HorzScroll(int nSBCode, int nPos);
	BOOL VertScroll( int nSBCode, int nPos, int iDrect );
	BOOL MouseWheel(int zDelta);
	BOOL MouseMoving(POINT point);
	BOOL CursorIn( POINT point );
	BOOL MouseLBUp(POINT point );
	BOOL MouseLDBClick(POINT point );
	BOOL MouseRBUp(POINT point );
	BOOL MouseRDBClick(POINT point );
	void MouseLeave( );
	BOOL MouseLBDown(POINT point);
	BOOL MouseRBDown(POINT point);
	virtual BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam );;
	void _UpdateScroll( int nMaxVScroll = 1, int nMaxHScroll = 1 );
	void _Notify( XLAY_MESSAGE message, POINT point )
	{
		NM_PARAM param = {0};
		param.point = point;
		param.nCtrlId = GetID();
		param.nIndex  = -1;
		param.nParentId = -1;
		Notify( message, GetID(), (LPARAM)(LONG_PTR)&param );
	}
private:
	CUITree	m_tree;
	HNODE   m_hFirstVisable;
	UINT32  m_IndentationX;//父节点与子节点之间的缩进方式
	UINT32  m_nLineSpace;
	IMG		m_imageLineSpace;
	UINT32  m_nHelpY;
	INT32   m_nXCtrlIdExpandShow;
	INT32   m_nXCtrlIdUnexpandShow;
	HNODE   m_hHotNode;
	HNODE   m_hMouseIn;
	HNODE   m_hFocusNode;
	HNODE	m_hSelected;
	IMG		m_imageHot;
	IMG		m_imageSelect;
	bool	m_bDbClickExpand;
	
};
#endif
inline void CUITreeCtrl::SetHotNodeBkGnd( const IMG& imageHot )
{
	m_imageHot = imageHot;
}

inline void CUITreeCtrl::SetSelectNodeBkGnd( const IMG& imageSelect )
{
	m_imageSelect = imageSelect;
}