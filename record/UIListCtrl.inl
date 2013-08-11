inline void CUIListCtrl::Destroy(  )
{
// 	
// 	__super::DestroyWindow();
}


inline const INT32 CUIListCtrl::Size  (  )const
{
	if ( m_pdataBase )
	{
		return m_pdataBase->size();
	}
	return m_list.Size();
}


inline  CUIListCtrl::LPCROW CUIListCtrl::operator []( INT32 nIndexIndex )const
{
	return _GetRow( nIndexIndex );
}


inline  CUIListCtrl::LPROW CUIListCtrl::operator []( INT32 nIndexIndex )
{
	
	return _GetRow( nIndexIndex );
}

inline void CUIListCtrl::_SetShowPos( INT32 nPos )
{
// 	switch( nPos )
// 	{
// 	case POS_HOME :
// 		{
// 			SendMessage( WM_KEYDOWN, VK_HOME, 0);
// 			if ( Size() )
// 			{
// 				SetCurSel( 0 );
// 			}
// 		}
// 		break;
// 	case POS_END:
// 		{
// 			SendMessage( WM_KEYDOWN, VK_END, 0 );
// 			if ( Size() )
// 			{
// 				SetCurSel( Size()-1 );
// 			}
// 		}
// 		break;
// 	default:
// 		break;
// 	}
}


inline void CUIListCtrl::SetRowHeight( INT32 nHeight )
{
	m_nRowHeight = nHeight;
	InvalidRect(NULL);
}


inline void CUIListCtrl::SetHotRowBkGnd( const IMG& imageHot )
{
	m_imageHot = imageHot;
}

inline void CUIListCtrl::SetSelectRowBkGnd( const IMG& imageSelect )
{
	m_imageSelect = imageSelect;
}

inline INT32 CUIListCtrl::GetRowHeight()const
{
	return m_nRowHeight;
}

inline RECT CUIListCtrl::GetRowRect( INT32 nIndex ) const
{
	RECT rowRect;
	RECT_EMPTY( rowRect );
	LPCROW pRow = _GetRow( nIndex );
	if ( pRow )
	{
		pRow->GetRect( rowRect );
	}
	return rowRect;
}

inline DWORD CUIListCtrl::GetRowData( INT32 nIndex ) const
{
	LPCROW pRow = _GetRow( nIndex );
	if ( pRow )
	{
		return pRow->GetData();
	}
	return 0;
}

inline void CUIListCtrl::SetRowData( INT32 nIndex, DWORD dwData )
{
	LPROW pRow = _GetRow( nIndex );
	if ( pRow )
	{
		pRow->SetData( dwData );
	}
}


inline INT32 CUIListCtrl::GetFirstSel( )
{
	m_nPreEnumRowPos = NAPos;
	if ( m_setSelected.size() )
	{
		m_nPreEnumRowPos = *m_setSelected.begin();
	}
	return m_nPreEnumRowPos;
}

inline UINT CUIListCtrl::GetSelectedCount(  )
{
	return (UINT)m_setSelected.size();
}

inline INT32 CUIListCtrl::GetNextSel( )
{
	if ( NAPos != m_nPreEnumRowPos )
	{
		std::set<INT32>::iterator _it =  m_setSelected.find( m_nPreEnumRowPos );
		if ( (_it != m_setSelected.end()) && ((++_it)!=m_setSelected.end()) )
		{
			m_nPreEnumRowPos = *_it;
			return m_nPreEnumRowPos;
		}
	}
	m_nPreEnumRowPos = NAPos;
	return m_nPreEnumRowPos;
}


inline BOOL CUIListCtrl::SetCurSel( INT32 nIndex )
{
	m_setSelected.clear();
	if(  VALUE_IN_RANGE(nIndex, 0, Size()-1 ) )
	{
		m_setSelected.insert( nIndex );
		if ( !IsRowVisable( nIndex ) )
		{
			ScrollTo( nIndex );
		}
		return TRUE;
	}
	return FALSE;
}

inline INT32 CUIListCtrl::GetCurSel(  )const
{
	if ( m_setSelected.size() )
	{
		return *m_setSelected.begin();
	}
	return -1;
}

inline SIZE CUIListCtrl::_GetVirtualSize(  )
{
	SIZE ret = {0, 0};
	if ( !m_pdataBase )
	{
		return ret;
	}
	INT32 nIndexHeight = Size()*(m_nRowHeight+m_nLineSpace);
	RECT rcClient;
	GetClient( rcClient );
	ret.cx = RECT_W(rcClient);
	ret.cy = nIndexHeight;
	return ret;
}

inline CUIListCtrl::LPROW CUIListCtrl::GetAt( INT32 nIndex )
{
	return _GetRow( nIndex );
}

inline CUIListCtrl::LPCROW CUIListCtrl::GetAt( INT32 nIndex )const
{

	return _GetRow( nIndex );
}

inline CUIListCtrl::LPROW CUIListCtrl::_GetRow( INT32 nIndexIndex )
{
	if ( nIndexIndex < 0
		|| nIndexIndex >= Size() )
	{
		return NULL;
	}
	if ( IsVirtualMode() )
	{
		if ( !m_rgShow.DataInRange( nIndexIndex ) )
		{
			return NULL;
		}
		nIndexIndex -= m_rgShow.m_min;
	}
	Iter _RowIter = m_list.Offset( m_list.Begin(), nIndexIndex );
	if ( IsValid( _RowIter ) )
	{
		return *_RowIter;
	}
	return NULL;
}

inline CUIListCtrl::LPCROW CUIListCtrl::_GetRow( INT32 nIndexIndex )const
{
	if ( IsVirtualMode() && Size() )
	{
		if ( !m_rgShow.DataInRange( nIndexIndex ) )
		{
			return NULL;
		}
		nIndexIndex -= m_rgShow.m_min;
	}
	Const_Iter _RowIter = m_list.Offset( m_list.Begin(), nIndexIndex );
	if ( m_list.IsValid( _RowIter ) )
	{
		return *_RowIter;
	}
	return NULL;
}