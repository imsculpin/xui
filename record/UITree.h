#ifndef __UI_TREE_H__
#define __UI_TREE_H__
#include "UIDefine.h"
#include "UILayPanel.h"

class CUITreeNode;
typedef CUILayPanel XTreeNode;
__DEFINE_SMART_HANDLE_NEED_DELETER( CUITreeNode, HXTREENODE, DeteleDeleter<CUITreeNode>  );

typedef CUILayPanel XNODEDATA;
class CUITreeNode
{
public:
	CUITreeNode()
		:m_children(NULL)
		,m_parent(NULL)
		,m_pre (NULL)
		,m_next(NULL)
		,m_bExpand(false)
	{

	}
	CUITreeNode( const XNODEDATA& data )
		:m_children(NULL)
		,m_parent(NULL)
		,m_pre (NULL)
		,m_next(NULL)
		,m_nodeData(data)
		,m_bExpand(false)
	{

	}
	~CUITreeNode();
	bool IsVisable()const
	{
		return m_nodeData.IsVisable()
			&& ( ( NULL == m_parent )
			|| ( NULL != m_parent
			&& m_parent->m_bExpand ) );
	}
	HXTREENODE m_children;
	HXTREENODE m_parent;
	HXTREENODE m_next;
	HXTREENODE m_pre;
	XNODEDATA  m_nodeData;
	bool       m_bExpand;
};

inline HXTREENODE Last( const HXTREENODE& hNode )
{
	if ( hNode )
	{
		HXTREENODE hLast = hNode;
		while ( hLast->m_next )
		{
			hLast = hLast->m_next;
		}
		return hLast;
	}
	return NULL;
}

inline HXTREENODE First( const HXTREENODE& hNode )
{
	if ( hNode )
	{
		HXTREENODE hFirst = hNode;
		while ( hFirst->m_pre )
		{
			hFirst = hFirst->m_pre;
		}
		return hFirst;
	}
	return NULL;
}

class CUITree
{
public:
	CUITree(  )
	{
	}
	~CUITree(  )
	{
         FreeAll();
	}
	
	HXTREENODE InsertNode( const XNODEDATA& nodeData, \
		HXTREENODE _before = NULL, HXTREENODE _parent = NULL );
	HXTREENODE InsertNode( HXTREENODE node, \
		HXTREENODE _before = NULL, HXTREENODE _parent = NULL )
	{
		if ( node )
		{
			return InsertNode( node->m_nodeData, _before, _parent );
		}
		return NULL;
	}

	HXTREENODE InsertTree( HXTREENODE hTree,  \
		HXTREENODE _before = NULL, HXTREENODE _parent = NULL  );

	HXTREENODE RemoveNode( HXTREENODE& hRemove, bool bFreeChildren = TRUE );
	void RemoveAll(  )
	{
		FreeAll(  );
	}

	//从整个树里查找
	template <class Fun>
	HXTREENODE FindNode( Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		return FindNode( m_root, funcation, dwParam, bOnlyVisable );
	}

	//从指定的节点开始查找
	template <class Fun>
	HXTREENODE FindNode( HXTREENODE hNode, Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		if ( NULL == hNode )
		{
			return NULL;
		}
		HXTREENODE _next = hNode;
		while ( _next )
		{
			if ( ((bOnlyVisable && _next->IsVisable())
				|| (!bOnlyVisable))
				&& funcation( *(_next), dwParam ) )
			{
				return (_next);
			}
			
			if ( NULL != (_next)->m_children 
				&& (( !bOnlyVisable )
				|| ( bOnlyVisable
				&& (_next)->m_children->IsVisable() )))
			{
				_next = (_next)->m_children;
				continue;
			}

			if ( (_next)->m_next )
			{
				_next = (_next)->m_next;
				continue;
			}
			_next = (_next)->m_parent;
			while ( _next )
			{
				if ( _next->m_next )
				{
					_next = _next->m_next;
					break;
				}
				else
				{
					_next = _next->m_parent;
				}
			}
		}
		return NULL;
	}

	//从整个树里查找枚举
	template <class Fun>
	void EnumNode( Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		EnumNode( m_root, funcation, dwParam, bOnlyVisable );
	}

	//从指定的节点开始枚举
	template <class Fun>
	void EnumNode( HXTREENODE hNode, Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		if ( NULL == hNode )
		{
			return;
		}
		HXTREENODE _next = hNode;
		HXTREENODE _stop = hNode->m_parent;
		while ( _next )
		{
			if ( (bOnlyVisable 
				&& _next->IsVisable())
				|| (!bOnlyVisable) )
			{
				funcation( *_next, dwParam );
			}

			if ( NULL != (_next)->m_children 
				&& (( !bOnlyVisable )
				|| ( bOnlyVisable
				&& (_next)->m_children->IsVisable() )))
			{
				_next = _next->m_children;
				continue;
			}

			if ( (_next)->m_next )
			{
				_next = (_next)->m_next;
				continue;
			}
			_next = (_next)->m_parent;
			if ( _stop == _next )
			{
				_next = NULL;
			}
			while ( _next )
			{
				if ( _next->m_next )
				{
					_next = _next->m_next;
					break;
				}
				else
				{
					_next = _next->m_parent;
					if ( _stop == _next )
					{
						_next = NULL;
					}
				}
			}
		}
	}

	//从整个树里逆向查找
	template <class Fun>
	HXTREENODE ReverseFindNode( Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		//到达最后一个叶子节点
		HXTREENODE last = Last(m_root);
		while( NULL != last 
			&& NULL!= last->m_children )
		{
			last = Last( last->m_children );
		}
		return ReverseFindNode( last, funcation, dwParam, bOnlyVisable );
	}

	//从指定的节点开始逆向查找
	template <class Fun>
	HXTREENODE ReverseFindNode( HXTREENODE hNode, Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		if ( NULL == hNode )
		{
			return NULL;
		}
		HXTREENODE _Pre = hNode;
		while ( _Pre )
		{
			if ( ((bOnlyVisable && _Pre->IsVisable())
				|| (!bOnlyVisable))
				&& funcation( *(_Pre), dwParam ) )
			{
				return (_Pre);
			}

			//找前一个满足条件的节点
			if ( _Pre->m_pre )
			{
				_Pre = _Pre->m_pre;
			}
			else
			{
				_Pre = _Pre->m_parent;
				continue;
			}
			if ( _Pre )
			{
				//到达_Pre的最后一个叶子节点
				while( ((bOnlyVisable && _Pre->IsVisable())
					|| (!bOnlyVisable))
					&& NULL != _Pre->m_children )
				{
					_Pre = Last( _Pre->m_children );
				}
			}
		}
		return NULL;
	}

	//从整个树里逆向枚举
	template <class Fun>
	void ReverseEnumNode( Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		//到达最后一个叶子节点
		HXTREENODE last = Last(m_root);
		while( NULL != last 
			&& NULL!= last->m_children )
		{
			last = Last( last->m_children );
		}
		return ReverseEnumNode( last, funcation, dwParam, bOnlyVisable );
	}

	//从指定的节点开始逆向枚举
	template <class Fun>
	void ReverseEnumNode( HXTREENODE hNode, Fun funcation, DWORD dwParam, bool bOnlyVisable = false )
	{
		if ( NULL == hNode )
		{
			return NULL;
		}
		HXTREENODE _Pre = hNode;
		while ( _Pre )
		{
			if ( (bOnlyVisable 
				&& _Pre->IsVisable())
				|| (!bOnlyVisable) )
			{
				funcation( *_Pre, dwParam );
			}

			//找前一个满足条件的节点
			if ( _Pre->m_pre )
			{
				_Pre = _Pre->m_pre;
			}
			else
			{
				_Pre = _Pre->m_parent;
				continue;
			}
			if ( _Pre )
			{
				//到达_Pre的最后一个叶子节点
				while( ((bOnlyVisable && _Pre->IsVisable())
					|| (!bOnlyVisable))
					&& NULL != _Pre->m_children )
				{
					_Pre = Last( _Pre->m_children );
				}
			}
		}
		return NULL;
	}

	bool IsEmpty()
	{
		return NULL == m_root;
	}

	HXTREENODE GetRoot()const
	{
		return m_root;
	}

	

protected:
	void FreeTree( HXTREENODE& hRoot );
	void FreeAll()
	{
		FreeTree( m_root );
	}
private:
	HXTREENODE m_root;
};
#endif