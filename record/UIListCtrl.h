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

	/*!< ����List */
	inline void Destroy(  );
	//******************************************************************************
	// ��    ��: CUIListCtrl::SetVirtualMode
	// ����Ȩ��: public  
	// �� �� ֵ: BOOL
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ��������ģʽ
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
	// ��    ��: CUIListCtrl::GetRowFromPoint
	// ����Ȩ��: public  
	// ��    ��: IN const POINT & point
	// �� �� ֵ: CUIListCtrl::LPROW
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	LPROW GetRowFromPoint( IN const POINT& point );

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetAt
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex
	// �� �� ֵ: CUIListCtrl::LPROW
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline LPROW GetAt( IN INT32 nIndex );

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetAt
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex
	// �� �� ֵ: CUIListCtrl::LPCROW
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline LPCROW GetAt( IN INT32 nIndex )const;

	//******************************************************************************
	// ��    ��: CUIListCtrl::Size
	// ����Ȩ��: public  
	// �� �� ֵ: INT32
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline const INT32 Size  (  )const;

	//******************************************************************************
	// ��    ��: GetState
	// ����Ȩ��: public   
	// ��    ��: IN INT32 nIndex ROW
	// �� �� ֵ: void��
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ��ȡ�е�״̬
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
	// ��    ��: ScrollTo
	// ����Ȩ��: public   
	// ��    ��: IN INT32 nIndex ROW
	// �� �� ֵ: void��
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ������ĳһ��
	//******************************************************************************
	void ScrollTo( IN INT32 nIndex );

	//******************************************************************************
	// ��    ��: SetScrollPos
	// ����Ȩ��: public   
	// ��    ��: IN INT32 nIndex ROW
	// �� �� ֵ: void��
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ����
	//******************************************************************************
	virtual void SetScrollPos( int nBar, int nPos, BOOL bRedraw  = TRUE  )
	{
		__super::SetScrollPos( nBar, nPos, bRedraw );
		_UpdateShow();
	}
	void SetRect( const RECT& rcSet );

public:
	//******************************************************************************
	// ��    ��: CUIListCtrl::operator[]
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndexIndex
	// �� �� ֵ: CUIListCtrl::LPCROW
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: []���� ȡ��Ӧ�е�ָ��
	//******************************************************************************
	inline LPCROW operator []( IN INT32 nIndexIndex )const;

	//******************************************************************************
	// ��    ��: CUIListCtrl::operator[]
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndexIndex
	// �� �� ֵ: CUIListCtrl::LPROW
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: []���� ȡ��Ӧ�е�ָ��
	//******************************************************************************
	inline LPROW operator []( IN INT32 nIndexIndex );

	//******************************************************************************
	// ��    ��: CUIListCtrl::FixedRowHeight
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nHeight���еĸ߶�,
	//           ��� nHeight = RL_AUTOPANELHEIGHT�������Զ������еĸ߶�
	// �� �� ֵ: void
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: �̶��еĸ߶�
	//******************************************************************************
	inline void SetRowHeight( IN INT32 nHeight = RL_AUTOPANELHEIGHT );

	void SetScrollStep( INT32 nStep = 0 )
	{
		m_nScrollStep = nStep;
	}

	//******************************************************************************
	// ��    ��: CRListCtrl::SetHotRowBkGnd
	// ����Ȩ��: public  
	// ��    ��: IN HBITMAP hHotBk
	// �� �� ֵ: HBITMAP
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline void SetHotRowBkGnd( IN const IMG& image );

	//******************************************************************************
	// ��    ��: CRListCtrl::SetSelectRowBkGnd
	// ����Ȩ��: public  
	// ��    ��: IN HBITMAP hSelectBk
	// �� �� ֵ: HBITMAP
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline void SetSelectRowBkGnd( IN const IMG& image );

	//******************************************************************************
	// ��    ��: CRListCtrl::SetBkGnd
	// ����Ȩ��: public  
	// ��    ��: IN ARGB clrErase
	// �� �� ֵ: ARGB
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline void SetLineSpaceImage( const IMG& imagePtr ){ m_imageLineSpace = imagePtr;}

	//******************************************************************************
	// ��    ��: CUIListCtrl::SetCurSel
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex
	// �� �� ֵ: INT32 �к�
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ����ѡ����
	//******************************************************************************
	BOOL SetCurSel( IN INT32 nIndex );

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetCurSel
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex
	// �� �� ֵ: INT32 �к�
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ��ȡ��ǰѡ���У����ж���ѡ�У����ص�һ��
	//******************************************************************************
	inline INT32 GetCurSel(  )const;

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetHotRow
	// ����Ȩ��: public  
	// ��    ��: NA
	// �� �� ֵ: INT32 �к�
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: ��ȡ��ǰ��Ծ��
	//******************************************************************************
	inline INT32 GetHotRow(  )const
	{
		return m_nHotRow;
	}

	//******************************************************************************
	// ��    ��: CUIListCtrl::SetLineSpace
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nLineSpace
	// �� �� ֵ: void
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline void SetLineSpace( IN INT32 nLineSpace ) { m_nLineSpace = nLineSpace; }

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetRowHeight
	// ����Ȩ��: public  
	// �� �� ֵ: INT32
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline INT32 GetRowHeight()const;

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetRowRect
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex
	// �� �� ֵ: RECT
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline RECT GetRowRect( IN INT32 nIndex )const ;

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetRowData
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex
	// �� �� ֵ: RECT
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline DWORD GetRowData( IN INT32 nIndex )const;

	//******************************************************************************
	// ��    ��: CUIListCtrl::SetRowData
	// ����Ȩ��: public  
	// ��    ��: IN INT32 nIndex 
	// �� �� ֵ: RECT
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline void SetRowData( INT32 nIndex, DWORD dwData );

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetFirstSel
	// ����Ȩ��: public  
	// �� �� ֵ: INT32
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline INT32 GetFirstSel();

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetNextSel
	// ����Ȩ��: public  
	// �� �� ֵ: INT32
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline INT32 GetNextSel();

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetSelectedCount
	// ����Ȩ��: public  
	// �� �� ֵ: UINT ѡ�е�����
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
	//******************************************************************************
	inline UINT GetSelectedCount(  );

	//******************************************************************************
	// ��    ��: CUIListCtrl::GetLineSpace
	// ����Ȩ��: public  
	// �� �� ֵ: INT32
	// ��    ��: �����Ŷ�[1/12/2010 wWX11702]
	// ��    ��: 
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
	/*!> ������ʼ������ Correct the row data */
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
	//���ؿɼ�����ĵ�һ��
	INT32 _FirstVisable();
	//���ؿɼ���������һ��
	INT32 _LastVisable( INT32 nFirst );
	//��ǰ���Ƿ�����ж�
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
	INT32		    m_nPreEnumRowPos;//��ǰö��ѡ���е�λ��
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
