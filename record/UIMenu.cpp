
#include "Switch.h"
// UIWnd.cpp : 实现文件
#include <algorithm>
#include "UIMenu.h"

// CUIMenu
CUIMenu::CUIMenu()
{

}


BOOL CUIMenu::InsertMenu( const tstring& strText, IImage* pImage, UINT nCmdId, BOOL bEnable /*= TRUE*/ )
{
	for ( vector<MENUDATA>::iterator _lab = m_vecMenuData.begin();
		_lab !=  m_vecMenuData.end(); ++_lab )
	{
		if ( _lab->nCmdId == nCmdId )
		{
			_lab->bEnable = bEnable;
			return TRUE;
		}
	}
	//添加菜单项
	MENUDATA menuData;
	menuData.cbSize = sizeof( menuData );
	m_vecMenuData.push_back( menuData );
	RtlCopyMemory( m_vecMenuData.back().pszText, strText.c_str(), (strText.length()+1)*sizeof(TCHAR) );
	m_vecMenuData.back().pImage = (IImage*)pImage;
	m_vecMenuData.back().nCmdId = nCmdId;
	m_vecMenuData.back().pSubMenu = NULL;
	m_vecMenuData.back().nSubCount = 0;
	m_vecMenuData.back().pSubAccel = NULL;
	m_vecMenuData.back().nSubAccelcount = 0;
	m_vecMenuData.back().bEnable = bEnable;
	return TRUE;
}

BOOL CUIMenu::InsertSpaceMenu(  )
{
	MENUDATA menuData;
	menuData.cbSize = sizeof( menuData );
	m_vecMenuData.push_back( menuData );
	m_vecMenuData.back().nCmdId = 0;
	m_vecMenuData.back().pszText[0] = 0;
	return TRUE;
}

BOOL CUIMenu::InsertSubMenu( const tstring& strText, IImage* pImage, const CUIMenu* pSubMenu, BOOL bEnable )
{
	if ( pSubMenu )
	{
		//添加菜单项
		MENUDATA menuData;
		menuData.cbSize = sizeof( menuData );
		m_vecMenuData.push_back( menuData );
		RtlCopyMemory( m_vecMenuData.back().pszText, strText.c_str(), (strText.length()+1)*sizeof(TCHAR) );
		m_vecMenuData.back().pImage = (IImage*)pImage;
		m_vecMenuData.back().nCmdId = 1;
		m_vecMenuData.back().pSubMenu = (MENUDATA*)&(pSubMenu->m_vecMenuData[0]);
		m_vecMenuData.back().nSubCount = (int)pSubMenu->m_vecMenuData.size();
		m_vecMenuData.back().pSubAccel = NULL;
		m_vecMenuData.back().nSubAccelcount = 0;
		m_vecMenuData.back().bEnable = bEnable;
		if( pSubMenu->m_vecAccel.size() )
		{
			m_vecMenuData.back().pSubAccel = (ACCEL*)&(pSubMenu->m_vecAccel[0]);
			m_vecMenuData.back().nSubAccelcount = (int)pSubMenu->m_vecAccel.size();
		}
	}
	return TRUE;
}

void CUIMenu::RemoveMenu( UINT nCmdId )
{
	//移除菜单
	for ( vector<MENUDATA>::iterator _lab = m_vecMenuData.begin();
		_lab !=  m_vecMenuData.end(); ++_lab )
	{
		if ( _lab->nCmdId == nCmdId )
		{
			m_vecMenuData.erase( _lab );
			break;	
		}
	}

	//移除快捷键
	for ( vector<ACCEL>::iterator _lab = m_vecAccel.begin();
		_lab !=  m_vecAccel.end(); ++_lab )
	{
		if ( _lab->cmd == nCmdId )
		{
			m_vecAccel.erase( _lab );
			break;	
		}
	}
}

void CUIMenu::RemoveSubMenu( UINT nCmdId )
{
	//移除菜单
	for ( vector<MENUDATA>::iterator _lab = m_vecMenuData.begin();
		_lab !=  m_vecMenuData.end(); ++_lab )
	{
		if ( _lab->nCmdId == nCmdId )
		{
			_lab->pSubMenu		= NULL;
			_lab->nSubCount		= 0;
			_lab->pSubAccel		= NULL;
			_lab->nSubAccelcount= 0;
			break;	
		}
	}
}

UINT CUIMenu::TrackPopupMenu( int x, int y, UINT uFlags )
{
	if ( m_vecMenuData.size() )
	{
		ACCEL* pAccels = NULL;
		if ( m_vecAccel.size() )
		{
			pAccels = &m_vecAccel[0];
		}
		//弹起Popmenu
		return ::UITrackPopupMenu( &m_vecMenuData[0], (int)m_vecMenuData.size(),\
			x, y, uFlags, pAccels, (int)m_vecAccel.size(), true );
	}
	return 0;
}

BOOL CUIMenu::AddAccel( ACCEL& accel )
{
	for ( int _i = 0; _i < (int)m_vecAccel.size(); ++_i )
	{
		if( m_vecAccel[_i].cmd == accel.cmd
			&& accel.fVirt == m_vecAccel[_i].fVirt 
			&& accel.key == m_vecAccel[_i].key )	
		{
			return TRUE;
		}
	}
	m_vecAccel.push_back( accel );
	return TRUE;
}

