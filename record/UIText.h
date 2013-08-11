#ifndef __UI_CTRLTEXT_H__
#define __UI_CTRLTEXT_H__
#include "UICtrlBase.h"
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <math.h>
#include <algorithm>
#include "CPTDC.h"
#include "UIPlus.h"
#include "strfun.h"
#ifndef UIPLUS_EXPORTS
//#pragma comment(lib,"uiplus.lib")
// #ifdef _DEBUG
// #pragma comment(lib,"..\\lib\\debug\\uiplus.lib")
// #else
// #pragma comment(lib,"..\\lib\\Release\\uiplus.lib")
// #endif
#endif

using namespace std;
class CUIText
	:public IUICtrl
{
public:
	CUIText();
	virtual ~CUIText();

	CUIText( const CUIText& uObj );
	const CUIText& operator =( const CUIText& uObj );
	INT32 Create( const tstring& strText, const RECT& rect, \
		INT32 nCommandId = UNIT_TEXT_ID );
public:
	void SetTextColor( ARGB clr )
	{
		assert( XW_NULL != m_nClass );
		m_crText = clr;
	}

	void SetFont( HFONT pFont )
	{
		assert( XW_NULL != m_nClass );
		m_hFont = pFont;
		m_bChanged = true;
	}

	void SetText( const tstring& strText )
	{
		assert( XW_NULL != m_nClass );
		m_strText = strText;
		m_strTextBase = strText;
		_FormatText( m_strText );
		m_bChanged = true;
	}
	void SetRect( const RECT& rcSet, bool bErase = false );
	void SetStyle( UINT32 nStyle )
	{
		m_nStyle = nStyle;
	}
	void SetParent( IUIBase* parent ) 
	{ 
		m_parent = parent; 		
		m_bChanged = true;
	}
	void SetID( INT32 nCmd ){ m_nCommandId = nCmd;return; }
	void SetData( DWORD dwData ){ m_dwExtParam = dwData; }
	void SetShowCondition( ShowCondition condition = NULL )
	{
		m_showCondition = condition;
	}
	tstring GetText(  )const
	{
		assert( XW_NULL != m_nClass );
		return m_strTextBase;
	}
	void GetRect( RECT& rect )const;
	void  GetClient( RECT& rect )
	{
		GetRect( rect );
	}
	SIZE		GetSize(  )const
	{
		SIZE size = { RECT_W(m_rcXCtrl), RECT_H(m_rcXCtrl) };
		return size;
	}
	ARGB GetTextColor( )const
	{
		assert( XW_NULL != m_nClass );
		return m_crText ;
	}
	HCURSOR GetCursor( )const
	{
		assert( XW_NULL != m_nClass );
		return NULL;
	}
	ARGB GetHotTextColor( )const
	{
		assert( XW_NULL != m_nClass );
		return m_crText;
	}
	UINT32 GetStyle()const
	{
		return m_nStyle;
	}
	HFONT GetFont(  )const
	{
		assert( XW_NULL != m_nClass );
		if( NULL == m_hFont
			&& m_parent )
		{
			return m_parent->GetFont();
		}
		return m_hFont;
	}

	XW_CLS GetClass()const{ return m_nClass; }
	IUIBase* GetParent()const { return m_parent; }
	virtual void Enable( bool bEnable )
	{
		bEnable;
	}
	void Show( bool bShow = true )
	{	
		m_bShow = bShow;
	}
	bool IsVisable(  )const;
	INT32 GetID()const { return m_nCommandId; }
	DWORD GetData()const { return m_dwExtParam; }
	void HasNotify( bool bNotify )
	{
		m_bHasNotify = bNotify;
	}
	
	bool HasNotify(  )const
	{
		return m_bHasNotify;
	}

	virtual bool InvalidRect( LPCRECT lpRect = NULL )
	{
		if ( m_parent )
		{
			if ( NULL != lpRect )
			{
				return m_parent->InvalidRect( lpRect );
			}
			RECT rcThis;
			GetRect( rcThis );
			return InvalidRect( &rcThis );
		}
		return false;
	}

	virtual bool		IsHot()const
	{
		return IsMouseIn();
	}
	virtual IUIWindow* GetUIWindow()const
	{
		if ( m_parent )
		{
			return m_parent->GetUIWindow();
		}
		return NULL;
	}
	void EnableOutGlaw( bool bEnable )
	{
		if ( (m_nClass == XW_TEXT
			|| m_nClass == XW_EDIT )
			&& m_nStyle&XWS_SINGLELINE
			)
		{
			m_bEnableOutGlaw = bEnable;
		}
	}
	INT32 GetScrollPos(int nBar)const
	{
		if ( m_parent )
		{
			return m_parent->GetScrollPos( nBar );
		}
		return 0;
	}
public:
	virtual void OnDraw(  IPainter* painter  );
	virtual BOOL CursorIn( POINT point  )
	{
		return FALSE;
	}
public:
	inline virtual BOOL MouseMoving( POINT pt );
	inline virtual BOOL MouseLClick	( POINT pt );
	inline virtual BOOL MouseRBDown ( POINT pt );
	inline virtual BOOL MouseRClick	( POINT pt );
	inline virtual BOOL MouseRDBClick( POINT pt );
	inline virtual BOOL MouseLBUp( POINT pt );
	inline virtual BOOL MouseLDBClick( POINT pt );
	inline virtual BOOL MouseRBUp( POINT pt );
	inline virtual BOOL MouseLBDown( POINT pt );
	inline virtual void MouseIn(  );
	inline virtual void MouseLeave(  );
	virtual BOOL KeyDown( WPARAM wParam, LPARAM lParam );
	virtual BOOL KeyUp( WPARAM wParam, LPARAM lParam );
	virtual BOOL FocusIn( POINT point );
	virtual BOOL FocusLeave(  );

	BOOL MouseWheel( int zDelta )
	{
		return FALSE;
	}
	virtual BOOL VertScroll( int nSBCode, int nPos, int iDrect )
	{
		return FALSE;
	}
	virtual BOOL HorzScroll( int sCODE, int nPos  )
	{
		return FALSE;
	}
	virtual BOOL Notify( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if ( m_parent )
		{
			return m_parent->Notify( message, wParam, lParam );
		}
		return FALSE;
	}

protected:
	friend class CUILayPanel;
	inline void _OnNotify( XWM_MESSAGE message, POINT point )
	{
		NM_PARAM param = {0};
		param.point = point;
		param.nCtrlId = m_nCommandId;
		if ( m_parent )
		{
			param.nParentId = m_parent->GetID();
		}
		Notify( message, 0, (LPARAM)(LONG_PTR)&param );
	}
	bool IsMouseIn()const{return m_bMouseIn;}
	void _FormatText( tstring& strText );
	void _Update()
	{
		RECT rcInvalid; 
		GetRect( rcInvalid );
		InvalidRect( &rcInvalid );
		Update();
	};
	virtual void _CorrectSize();
  
protected:
	XW_CLS			m_nClass;
	UINT32			m_nStyle;
	tstring			m_strText;
	tstring			m_strTextBase;
	IUIBase*		m_parent;
	HFONT			m_hFont;
	ShowCondition	m_showCondition;	//返回true，显示，返回false，隐藏
	RECT		    m_rcXCtrl;//(以行的原点为坐标原点)Base on row origin
	bool			m_bRbDown;
	bool			m_bLBDown;
	bool			m_bShow;
	bool			m_bEnableOutGlaw;
private:
	bool          m_bHasNotify;
	bool		  m_bMouseIn;
	bool		  m_bChanged;
	ARGB		  m_crText;
	INT32		  m_nCommandId;		//UICtrl 的ＩＤ
	DWORD		  m_dwExtParam;
};
#endif