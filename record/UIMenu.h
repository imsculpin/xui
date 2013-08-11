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
	����:����˵���
	strText �˵������
	hImage  �˵��������ʾ��ͼ��(�ڲ˵���Чʱ���벻Ҫ�������ͷ�)
	nCmdId  �˵�����ID
	bEnable ����TRUE/����FALSE�˵���
	*/

	BOOL InsertMenu( const tstring& strText, IImage* pImage, UINT nCmdId, BOOL bEnable = TRUE );
	/*
	����:����հײ˵���
	*/
	BOOL InsertSpaceMenu(  );
	/*
	�����������Ӳ˵�
	nCmdId   ���˵��������ID
	pSubMenu �Ӳ˵�ָ��
	*/
	BOOL InsertSubMenu( const tstring& strText, IImage* pImage, const CUIMenu* pSubMenu, BOOL bEnable = TRUE );
	/*
	��������ӿ�ݼ�
	accel ��ݼ����� Ŀǰֻ֧��FVIRTKEY;
	*/
	BOOL AddAccel( ACCEL& accel );
	//�Ƴ��˵���
	void RemoveMenu( UINT nCmdId );
	void RemoveSubMenu( UINT nCmdId );
	//�����˵�
	UINT TrackPopupMenu( int x, int y, UINT uFlags );
private:
	vector< MENUDATA >	  m_vecMenuData;
	vector< ACCEL >		  m_vecAccel;
};
