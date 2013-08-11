#pragma once
#include <vector>
using std::vector;
#include "UIPlus.h"


class CUIMenu
{
public:
	CUIMenu();
	virtual ~CUIMenu()
	{

	}
	/*
	描述:插入菜单项
	strText 菜单项标题
	hImage  菜单项左侧显示的图标(在菜单有效时，请不要析构或释放)
	nCmdId  菜单命令ID
	bEnable 激活TRUE/禁用FALSE菜单项
	*/

	BOOL InsertMenu( const tstring& strText, IImage* pImage, UINT nCmdId, BOOL bEnable = TRUE );
	/*
	描述:插入空白菜单项
	*/
	BOOL InsertSpaceMenu(  );
	/*
	描述：插入子菜单
	nCmdId   父菜单项的命令ID
	pSubMenu 子菜单指针
	*/
	BOOL InsertSubMenu( const tstring& strText, IImage* pImage, const CUIMenu* pSubMenu, BOOL bEnable = TRUE );
	/*
	描述：添加快捷键
	accel 快捷键内容 目前只支持FVIRTKEY;
	*/
	BOOL AddAccel( ACCEL& accel );
	//移除菜单项
	void RemoveMenu( UINT nCmdId );
	void RemoveSubMenu( UINT nCmdId );
	//弹出菜单
	UINT TrackPopupMenu( int x, int y, UINT uFlags );
private:
	vector< MENUDATA >	  m_vecMenuData;
	vector< ACCEL >		  m_vecAccel;
};
