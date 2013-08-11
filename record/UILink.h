#ifndef __UI_CTRLLINK_H__
#define __UI_CTRLLINK_H__
#include "UIText.h"
class CUILink:public CUIText
{	
public:
	CUILink()
		:CUIText()
		,m_hCursor( NULL )
		,m_crHotText(CLR_DEFAULT_LINK)
		,m_crDisable(CLR_DEFAULT_GRAY)
		,m_bEnable(true)
	{
		m_nStyle |= XWS_UNDERLINE;
	}
	CUILink( const CUILink& uObj )
		:CUIText( uObj )
		,m_hCursor( uObj.m_hCursor )
		,m_crHotText(uObj.m_crHotText)
		,m_bEnable(uObj.m_bEnable)
	{

	}
	const CUILink& operator =( const CUILink& uObj )
	{
		if ( this == &uObj )
		{
			return *this;
		}
		CUIText::operator =(uObj);
		m_hCursor = uObj.m_hCursor;
		m_crHotText = uObj.m_crHotText;
		m_bEnable = uObj.m_bEnable;
		return *this;
	}
	virtual ~CUILink()
	{
	}

	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )
	{
		assert( XW_NULL == m_nClass );
		__super::Create( strText, rect, nCommandId );
		m_nClass	 = XW_LINK;
		return RL_OK;
	}
public:
	virtual void Enable( bool bEnable )
	{
		if ( m_bEnable != bEnable )
		{
			m_bEnable = bEnable;
			_Update();
		}
	}
public:
	void SetCursor( HCURSOR hCursor )
	{
		assert( XW_NULL != m_nClass );
		m_hCursor = hCursor;
	}

	void SetHotTextColor( ARGB clrHotText )
	{
		assert( XW_NULL != m_nClass );
		m_crHotText = clrHotText;
		_Update();
	}

	HCURSOR GetCursor()
	{
		assert( XW_NULL != m_nClass );
		return m_hCursor;
	}

	ARGB GetHotTextColor()
	{
		assert( XW_NULL != m_nClass );
		return m_crHotText;
	}
	virtual void OnDraw(  IPainter* painter  );
	virtual BOOL CursorIn( POINT point  );
private:
	bool		m_bEnable;
	HCURSOR		m_hCursor;
	ARGB	m_crHotText;
	ARGB	m_crDisable;
};
#endif