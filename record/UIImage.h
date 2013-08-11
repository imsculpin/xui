#ifndef __UI_CTRLIMAGE_H__
#define __UI_CTRLIMAGE_H__
#include "UIText.h"
class CUIImage
	: public CUIText
{
public:
	CUIImage()
		:m_clrBack(X_TRANSPARENT)
	{
	}

	CUIImage( const CUIImage& uObj )
		:CUIText( uObj )
		,m_clrBack( uObj.m_clrBack )
		,m_image( uObj.m_image)
	{

	}

	CUIImage& operator =( const CUIImage& uObj )
	{
		if ( this == &uObj )
		{
			return *this;
		}
		CUIText::operator =(uObj);
		m_clrBack = uObj.m_clrBack;
		m_image = uObj.m_image;
		return *this;
	}

	virtual ~CUIImage()
	{

	}

	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )
	{
		assert( XW_NULL == m_nClass );
		__super::Create( strText, rect, nCommandId );
		m_nClass = XW_IMAGE;
		return RL_OK;
	}

public:
	void SetImage( const IMG& image )
	{
		m_image = image;
	}

	IMG* GetImage()
	{
		return &m_image;
	}
	void SetBkColor( ARGB clrBack = X_TRANSPARENT ){m_clrBack = clrBack;}
public:
	virtual void OnDraw(  IPainter* painter  );

private:
	IMG    m_image;
	ARGB m_clrBack;
};
#endif