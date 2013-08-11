#ifndef __CPTDC_H__
#define __CPTDC_H__
#include "Switch.h"
//ºÊ»›DC
struct CPTDC
{
	CPTDC(  )
		:m_hCPTDC(NULL)
		//		,m_nSaveDC(0)
	{
	}
	~CPTDC( )
	{
		ReleaseDC();
	}
	BOOL GetDC( HDC hSrc )
	{
		if ( m_hCPTDC )
		{
			ReleaseDC();
		}
		m_hCPTDC = CreateCompatibleDC(hSrc);
		return NULL!=m_hCPTDC;
	}
	void ReleaseDC(  )
	{
		if ( m_hCPTDC )
		{
			DeleteDC(m_hCPTDC);
			m_hCPTDC = NULL;
		}
	}
	operator HDC()
	{
		return m_hCPTDC;
	}
private:
	HDC m_hCPTDC;
};
#endif