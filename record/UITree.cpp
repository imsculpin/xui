#include "UITree.h"

CUITreeNode::~CUITreeNode()
{
	m_children = NULL;
	m_pre = NULL;
	m_next = NULL;
	m_children = NULL;
}

HXTREENODE CUITree::InsertNode( const XNODEDATA& nodeData, \
							  HXTREENODE _before /*= NULL*/, HXTREENODE _parent /*= NULL */ )
{
	HXTREENODE hNode = NULL;
	try
	{
		hNode = new CUITreeNode(nodeData);
	}
	catch( ... )
	{

	}
	if ( hNode )
	{
		if ( _before )
		{
			hNode->m_pre = _before->m_pre;
			hNode->m_next= _before;
			hNode->m_parent = _before->m_parent;
			if ( _before->m_pre )
			{
				_before->m_pre->m_next = hNode;
			}
			_before->m_pre = hNode;
			if ( m_root == _before )
			{
				m_root = hNode;
			}
		}
		else
		{
			hNode->m_parent = _parent;
			HXTREENODE _last;
			if ( _parent )
			{
				_last = Last( _parent->m_children );
				if ( _last )
				{
					hNode->m_pre = _last;
					_last->m_next = hNode;
				}
				else
				{
					_parent->m_children = hNode;
				}
				return hNode;
			}
			_last = Last( m_root );
			if ( _last )
			{
				hNode->m_pre = _last;
				_last->m_next = hNode;
			}
			else
			{
				m_root = hNode;
			}
		}
	}
	return hNode;
}

HXTREENODE CUITree::InsertTree( HXTREENODE hTree,\
							  HXTREENODE _before /*= NULL*/, HXTREENODE _parent /*= NULL */ )
{
	HXTREENODE hLast = Last( hTree );
	if ( hTree )
	{	
		if ( _before )
		{
			hTree->m_pre = _before->m_pre;
			hLast->m_next= _before;
			if ( _before->m_pre )
			{
				_before->m_pre->m_next = hTree;
			}
			while( hTree )
			{
				hTree->m_parent = _before->m_parent;
				hTree = hTree->m_next;
				if ( hTree == hLast )
				{
					break;
				}
			}
			_before->m_pre = hLast;
			if ( _before == m_root )
			{
				m_root = hTree;
			}
		}
		else
		{
			HXTREENODE _last;
			if ( _parent )
			{
				_last = Last( _parent->m_children );
				if ( _last )
				{
					hTree->m_pre = _last;
					_last->m_next = hTree;
				}
				else
				{
					_parent->m_children = hTree;
				}
				return hTree;
			}
			_last = Last( m_root );
			if ( _last )
			{
				hTree->m_pre = _last;
				_last->m_next = hTree;
			}
			else
			{
				m_root = hTree;
			}
			while( hTree )
			{
				hTree->m_parent = _before->m_parent;
				hTree = hTree->m_next;
				if ( hTree == hLast )
				{
					break;
				}
			}
		}
	}
	return NULL;
}

HXTREENODE CUITree::RemoveNode( HXTREENODE& hRemove, bool bFreeChildren /*= TRUE*/ )
{
	if ( hRemove->m_pre )
	{
		hRemove->m_pre->m_next = hRemove->m_next;
	}
	if ( hRemove->m_next )
	{
		hRemove->m_next->m_pre = hRemove->m_pre;
	}
	if ( hRemove == m_root )
	{
		m_root = m_root->m_next;
	}
	if ( NULL != hRemove->m_parent 
		&& hRemove->m_parent->m_children == hRemove)
	{
		hRemove->m_parent->m_children = hRemove->m_next;
	}
	HXTREENODE hChild = hRemove->m_children;
	while( hChild )
	{
		hChild->m_parent = NULL;
		hChild = hChild->m_next;
	}
	hChild = hRemove->m_children;
	hRemove->m_parent = NULL;
	hRemove->m_children = NULL;
	hRemove->m_pre = NULL;
	hRemove->m_next = NULL;
	if ( bFreeChildren )
	{
		FreeTree( hChild );
	}
	return hChild;
}

void CUITree::FreeTree( HXTREENODE& hRoot )
{
	if ( NULL == hRoot )
	{
		return;
	}
	HXTREENODE last = Last(hRoot);
	while( NULL != last 
		&& NULL!= last->m_children )
	{
		last = Last( last->m_children );
	}

	HXTREENODE _Pre = last;
	while ( _Pre )
	{
		_Pre->m_next = NULL;
		_Pre->m_children = NULL;
		//找前一个满足条件的节点
		if ( _Pre->m_pre )
		{
			_Pre = _Pre->m_pre;
			_Pre->m_next->m_pre = NULL;
		}
		else
		{
			_Pre = _Pre->m_parent;
			if( _Pre )
			{
				_Pre->m_children->m_parent = NULL;
			}
			continue;
		}
		if ( _Pre )
		{
			//到达_Pre的最后一个叶子节点
			while( NULL != _Pre->m_children )
			{
				_Pre = Last( _Pre->m_children );
			}
		}
	}
	hRoot = NULL;
}