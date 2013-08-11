#ifndef __LC_TRAY_H__
#define __LC_TRAY_H__
#include "Aplication.h"
#include "UIType.h"
#include <shellapi.h>
#include <map>
#include <vector>
using std::vector;
using std::map;
class CTray
{
public:
	CTray()
		:m_bVisable(false)
		,m_hIcon(NULL)
		,m_curFrame(0)
		,m_dwParam(0)
	{

	}
	~CTray()
	{
		Destroy();
	}
	
	BOOL Create( UINT nId, LPCTSTR lpszTip = NULL );
	void Destroy();
	BOOL SetImage( const HIMAGE* pHImages, int nImages );
	BOOL Show( bool bShow );
	HIMAGE GetImage(  )
	{
		if ( m_vecImage.size() )
		{
			return *m_vecImage.begin();
		}
		return NULL;
	}
	BOOL SetTipInfo( const tstring& strTip );
#if (_WIN32_IE >= 0x0500)
	enum TIPTYPE
	{
		E_NONE,			// NIIF_NONE(0)
		E_INFO,			// NIIF_INFO(1)
		E_WARNING,		// NIIF_WARNING(2)
		E_ERROR			// NIIF_ERROR(3)
	};
	BOOL ShowBalloonTip( const tstring& strTitle, const tstring& strInfo, int nTimeOut = 10000, TIPTYPE type = E_NONE );
#endif

	void BeginFlash( int nRate = 100 );
	void StopFlash();
	void SetUICALLBACK( UICALLBACK listener, DWORD dwParam = 0 );
protected:
	static LRESULT CALLBACK TrayMessage(HWND, UINT, WPARAM, LPARAM);
private:
	NOTIFYICONDATA m_notifyData;
	vector<HIMAGE> m_vecImage;
	HICON		   m_hIcon;
	UINT		   m_TrayId;
	DWORD		   m_dwParam;
	int			   m_curFrame;
	static HWND	   m_hMessage;
	struct LISTENER
	{
		CTray*	pThis;
		UICALLBACK listener;
	};
	static map< UINT, LISTENER > m_mapUICALLBACKs;
	bool		   m_bVisable;
};

#endif