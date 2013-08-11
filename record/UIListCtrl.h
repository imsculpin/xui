#ifndef __UI_LISTCTRL_H__
#define __UI_LISTCTRL_H__
#include "UIView.h"
#include "UIDb.h"
#include "UIList.h"
#include "UIText.h"
class CUIListCtrl: public CUIView
{
public:
	typedef  CUIList::Iter            Iter;
	typedef  CUIList::Const_Iter      Const_Iter;
	typedef  CUIList::LPITEM			 LPROW;
	typedef  CUIList::LPCITEM		 LPCROW;
	typedef  CUIList::ITEM		     ROW;

public:
	CUIListCtrl(void)
		:m_nRowHeight(RL_AUTOPANELHEIGHT)
		,m_nMouseInIndex(NAPos)
		,m_nIndexsPerWheel(1)
		,m_nLineSpace(0)
		,m_nListSize(0)
		,m_nPreEnumRowPos(NAPos)
		,m_nHotRow(NAPos)
		,m_nFocusRow(NAPos)
		,m_pdataBase(NULL)
		,m_format(NULL)
		,m_pRowTemplate(NULL)
		,m_nScrollStep( 50 )
		,m_bScrolling( FALSE )
	{
		m_list.Parent( this );
	}

	~CUIListCtrl(void)
	{
	
	}

	/*!< 销毁List */
	inline void Destroy(  );
	//******************************************************************************
	// 方    法: CUIListCtrl::SetVirtualMode
	// 访问权限: public  
	// 返 回 值: BOOL
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 设置虚拟模式
	//******************************************************************************
	BOOL SetVirtualMode( IDataInterface* pDB, FORMAT format, const ROW* pTemplate = NULL );
	inline IDataInterface* GetDataInterface(  )
	{
		return m_pdataBase;
	}
	bool IsVirtualMode(  )const
	{
		return NULL != m_pdataBase;
	}

public:
	Iter Begin(  )
	{
		return m_list.Begin();
	}

	Iter End(  )
	{
		return m_list.End();
	}

	Iter Next( Iter& iter )
	{
		return m_list.Next( iter );
	}

	bool IsValid( const Iter& iter )
	{
		return m_list.IsValid( iter );
	}

	Const_Iter Begin(  )const
	{
		return m_list.Begin();
	}

	Const_Iter End(  )const
	{
		return m_list.End();
	}

	Const_Iter Next( Const_Iter& iter )const
	{
		return m_list.Next( iter );
	}

	bool IsValid( Const_Iter& iter )const
	{
		return m_list.IsValid( iter );
	}

	template< class _Fun >
	LPROW FoundRow( _Fun founder, DWORD dwParam )
	{
		return m_list.FoundItem( founder, dwParam );
	}
	LPROW InsertRow( const ROW& _row );
	Iter InsertRow( Iter& _before,  ROW& _row );
	void RemoveRow( LPROW _remove )
	{
		assert( !IsVirtualMode() );
		if ( IsVirtualMode() )
		{
			return ;
		}
		RemoveRow( m_list.Hxitem2Iter( _remove ) );
	}
	void RemoveRow( Iter& _remove );
	void RemoveAll( );

public:

	//******************************************************************************
	// 方    法: CUIListCtrl::GetRowFromPoint
	// 访问权限: public  
	// 参    数: IN const POINT & point
	// 返 回 值: CUIListCtrl::LPROW
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	LPROW GetRowFromPoint( IN const POINT& point );

	//******************************************************************************
	// 方    法: CUIListCtrl::GetAt
	// 访问权限: public  
	// 参    数: IN INT32 nIndex
	// 返 回 值: CUIListCtrl::LPROW
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline LPROW GetAt( IN INT32 nIndex );

	//******************************************************************************
	// 方    法: CUIListCtrl::GetAt
	// 访问权限: public  
	// 参    数: IN INT32 nIndex
	// 返 回 值: CUIListCtrl::LPCROW
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline LPCROW GetAt( IN INT32 nIndex )const;

	//******************************************************************************
	// 方    法: CUIListCtrl::Size
	// 访问权限: public  
	// 返 回 值: INT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline const INT32 Size  (  )const;

	//******************************************************************************
	// 方    法: GetState
	// 访问权限: public   
	// 参    数: IN INT32 nIndex ROW
	// 返 回 值: void　
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 获取行的状态
	//******************************************************************************
	inline INT32 GetState( INT32 nIndex )
	{
		INT32 state = RRS_NORMAL;
		if( m_setSelected.end() != m_setSelected.find( nIndex ) )
		{
			state|=RRS_SELECTED;
		}
		if ( m_nHotRow == nIndex )
		{
			state |= RRS_HOT;
		}
		if ( m_nFocusRow == nIndex )
		{
			state |= RRS_FOCUSED;
		}
		return state;
	}

	void CalcSize( INT32 nIndex );

	//******************************************************************************
	// 方    法: ScrollTo
	// 访问权限: public   
	// 参    数: IN INT32 nIndex ROW
	// 返 回 值: void　
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 滚动到某一行
	//******************************************************************************
	void ScrollTo( IN INT32 nIndex );

	//******************************************************************************
	// 方    法: SetScrollPos
	// 访问权限: public   
	// 参    数: IN INT32 nIndex ROW
	// 返 回 值: void　
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 设置
	//******************************************************************************
	virtual void SetScrollPos( int nBar, int nPos, BOOL bRedraw  = TRUE  )
	{
		__super::SetScrollPos( nBar, nPos, bRedraw );
		_UpdateShow();
	}
	void SetRect( const RECT& rcSet );

public:
	//******************************************************************************
	// 方    法: CUIListCtrl::operator[]
	// 访问权限: public  
	// 参    数: IN INT32 nIndexIndex
	// 返 回 值: CUIListCtrl::LPCROW
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: []重载 取对应行的指针
	//******************************************************************************
	inline LPCROW operator []( IN INT32 nIndexIndex )const;

	//******************************************************************************
	// 方    法: CUIListCtrl::operator[]
	// 访问权限: public  
	// 参    数: IN INT32 nIndexIndex
	// 返 回 值: CUIListCtrl::LPROW
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: []重载 取对应行的指针
	//******************************************************************************
	inline LPROW operator []( IN INT32 nIndexIndex );

	//******************************************************************************
	// 方    法: CUIListCtrl::FixedRowHeight
	// 访问权限: public  
	// 参    数: IN INT32 nHeight　行的高度,
	//           如果 nHeight = RL_AUTOPANELHEIGHT则程序会自动计算行的高度
	// 返 回 值: void
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 固定行的高度
	//******************************************************************************
	inline void SetRowHeight( IN INT32 nHeight = RL_AUTOPANELHEIGHT );

	void SetScrollStep( INT32 nStep = 0 )
	{
		m_nScrollStep = nStep;
	}

	//******************************************************************************
	// 方    法: CRListCtrl::SetHotRowBkGnd
	// 访问权限: public  
	// 参    数: IN HBITMAP hHotBk
	// 返 回 值: HBITMAP
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetHotRowBkGnd( IN const IMG& image );

	//******************************************************************************
	// 方    法: CRListCtrl::SetSelectRowBkGnd
	// 访问权限: public  
	// 参    数: IN HBITMAP hSelectBk
	// 返 回 值: HBITMAP
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetSelectRowBkGnd( IN const IMG& image );

	//******************************************************************************
	// 方    法: CRListCtrl::SetBkGnd
	// 访问权限: public  
	// 参    数: IN ARGB clrErase
	// 返 回 值: ARGB
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetLineSpaceImage( const IMG& imagePtr ){ m_imageLineSpace = imagePtr;}

	//******************************************************************************
	// 方    法: CUIListCtrl::SetCurSel
	// 访问权限: public  
	// 参    数: IN INT32 nIndex
	// 返 回 值: INT32 行号
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 设置选中行
	//******************************************************************************
	BOOL SetCurSel( IN INT32 nIndex );

	//******************************************************************************
	// 方    法: CUIListCtrl::GetCurSel
	// 访问权限: public  
	// 参    数: IN INT32 nIndex
	// 返 回 值: INT32 行号
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 获取当前选中行，如有多行选中，返回第一行
	//******************************************************************************
	inline INT32 GetCurSel(  )const;

	//******************************************************************************
	// 方    法: CUIListCtrl::GetHotRow
	// 访问权限: public  
	// 参    数: NA
	// 返 回 值: INT32 行号
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 获取当前活跃行
	//******************************************************************************
	inline INT32 GetHotRow(  )const
	{
		return m_nHotRow;
	}

	//******************************************************************************
	// 方    法: CUIListCtrl::SetLineSpace
	// 访问权限: public  
	// 参    数: IN INT32 nLineSpace
	// 返 回 值: void
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetLineSpace( IN INT32 nLineSpace ) { m_nLineSpace = nLineSpace; }

	//******************************************************************************
	// 方    法: CUIListCtrl::GetRowHeight
	// 访问权限: public  
	// 返 回 值: INT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline INT32 GetRowHeight()const;

	//******************************************************************************
	// 方    法: CUIListCtrl::GetRowRect
	// 访问权限: public  
	// 参    数: IN INT32 nIndex
	// 返 回 值: RECT
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline RECT GetRowRect( IN INT32 nIndex )const ;

	//******************************************************************************
	// 方    法: CUIListCtrl::GetRowData
	// 访问权限: public  
	// 参    数: IN INT32 nIndex
	// 返 回 值: RECT
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline DWORD GetRowData( IN INT32 nIndex )const;

	//******************************************************************************
	// 方    法: CUIListCtrl::SetRowData
	// 访问权限: public  
	// 参    数: IN INT32 nIndex 
	// 返 回 值: RECT
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline void SetRowData( INT32 nIndex, DWORD dwData );

	//******************************************************************************
	// 方    法: CUIListCtrl::GetFirstSel
	// 访问权限: public  
	// 返 回 值: INT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline INT32 GetFirstSel();

	//******************************************************************************
	// 方    法: CUIListCtrl::GetNextSel
	// 访问权限: public  
	// 返 回 值: INT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline INT32 GetNextSel();

	//******************************************************************************
	// 方    法: CUIListCtrl::GetSelectedCount
	// 访问权限: public  
	// 返 回 值: UINT 选中的行数
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline UINT GetSelectedCount(  );

	//******************************************************************************
	// 方    法: CUIListCtrl::GetLineSpace
	// 访问权限: public  
	// 返 回 值: INT32
	// 作    者: 超邮团队[1/12/2010 wWX11702]
	// 描    述: 
	//******************************************************************************
	inline INT32 GetLineSpace(  )const { return m_nLineSpace; }

protected:	
	void OnDraw(  IPainter* painter );
	void EraseRowBkGnd( LPROW pRowErase, IPainter* painter );
	bool IsEmpty()
	{
		return 0 == Size();
	}
protected:
	/*!> 较正初始化数据 Correct the row data */
	inline void _CorrectRow(  );
	void _UpdateScroll( int nMaxVScroll = 1, int nMaxHScroll = 1 );
	void _UpdateShow( BOOL bRedraw = TRUE  )
	{
		if ( !Size() )
		{
			m_rgShow.Reset();
			InvalidRect();
			return;
		}
		m_rgShow.m_min = _FirstVisable();
		m_rgShow.m_max = _LastVisable( m_rgShow.m_min );
		if ( bRedraw )
		{
			InvalidRect( );
			Update();
		}
	}
	LPROW _GetRow( INT32 nIndexIndex );
	LPCROW _GetRow( INT32 nIndexIndex )const;
	INT32 GetVirtualModeBufferSize();
	inline void _SetShowPos( INT32 nPos );
	inline INT32 _GetMouseScrollLines();
	inline SIZE _GetVirtualSize( );
	SIZE _CalculateRowSize( LPCROW row );
	inline BOOL IsRowVisable( INT32 nIndex )
	{
		RECT rcRow = GetRowRect( nIndex );
		if ( RECT_ISEMPTY(rcRow) )
		{
			return TRUE;
		}
		RECT_TOY( rcRow, rcRow.top-m_VScroll.GetPos() );
		RECT rectTmp;
		::IntersectRect( &rectTmp, &m_rect, &rcRow );
		if ( 0 == memcmp( &rectTmp, &rcRow, sizeof(RECT)) )
		{
			return TRUE;
		}
		return FALSE;
	}
	//返回可见区域的第一行
	INT32 _FirstVisable();
	//返回可见区域的最后一行
	INT32 _LastVisable( INT32 nFirst );
	//当前行是否可视判断
	bool IsRowVisable( const Iter& _row );
public:
	BOOL HorzScroll(int nSBCode, int nPos);
	BOOL VertScroll(int nSBCode, int nPos, int iDrect);
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
	void DBChanged( BOOL bReflashAllData = TRUE );
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
	//
	friend CUILayPanel;
	enum { POS_HOME, POS_END = 0xFFFFFFFF };
	IDataInterface*		m_pdataBase;
	FORMAT				m_format;
	LPROW				m_pRowTemplate;
	std::set<INT32>	m_setSelected;
	RANGE<>			m_rgShow;	
	INT32			m_nFocusRow;
	INT32		    m_nHotRow;
	INT32		    m_nPreEnumRowPos;//当前枚举选中行的位置
	INT32			m_nRowHeight;
	INT32			m_nScrollStep;
	INT32			m_nIndexsPerWheel;
	INT32			m_nTotalRows;
	INT32			m_nMouseInIndex;
	INT32			m_nLineSpace;
	INT32		    m_nListSize;
	IMG				m_imageHot;
	IMG				m_imageSelect;
	IMG				m_imageLineSpace;
	CUIList			m_list;
	BOOL			m_bScrolling;
	CPTDC			m_bufferDc;
};

#include "UIListCtrl.inl"
#endif
