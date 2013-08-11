#pragma once
#include <map>
#include "UIImage.h"
#define SEP_VERT 1
#define SEP_HORZ 2
class CXSep : public CUIImage
{
public:
	CXSep()
		:m_bLButtonDown(false)
		,m_clickPoint( 0, 0 )
		,m_dwStyle( 0 )
		,m_hCursor(NULL)
		,CUIImage()
	{

	}
	~CXSep()
	{
		if ( m_hCursor )
		{
			DeleteObject( m_hCursor );
		}
	}
	/*!< ´´½¨´°¿Ú */
	INT32 CreateXCtrl( tstring strText, const CRect& rect, \
		INT32 nCommandId = UNIT_TEXT_ID )
	{
		ASSERT( UNS_NULL == m_nStyle );
		__super::CreateXCtrl( strText, rect, nCommandId );
		m_nStyle = UNS_SEP;
		return RL_OK;
	}
	
protected:
	virtual BOOL OnLButtonDown( CPoint pt );
	virtual BOOL OnLClick( CPoint pt );
	virtual BOOL OnMouseMove( CPoint pt );
	virtual BOOL OnSetCursor();
	DECLARE_MESSAGE_MAP()

private:
	bool   m_bLButtonDown;
	CPoint m_clickPoint;
	DWORD  m_dwStyle;
	HCURSOR m_hCursor;
};

