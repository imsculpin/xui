#ifndef __UI_PLUS_H__
#define __UI_PLUS_H__
#include <windows.h>
#include "ImagingPlus.h"
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef UIPLUS_EXPORTS
#	define DLLAPI(ret) _declspec(dllexport) ret WINAPI
#else
#	define DLLAPI(ret) _declspec(dllimport) ret WINAPI
#endif
	class IImage;
	struct MENUDATA
	{
		UINT    cbSize;
		UINT    nCmdId;
		TCHAR   pszText[128];
		HIMAGE  hImage;
		BOOL    bEnable;
		LPVOID  pSubMenu;
		LPVOID  pSubAccel;
		int     nSubCount; 
		int     nSubAccelcount;
	};


	class IUIBase;
#define SURPORT_DWM
#ifdef SURPORT_DWM
	DLLAPI( LRESULT ) EnableBlurWindow( HWND hWnd, bool bEnable );
	DLLAPI( LRESULT ) EnableComposition( bool bEnable );
#endif

#ifdef SURPORT_EXT
	//******************************************************************************
	// ��    ��: UpdateBkGnd
	// ����Ȩ��: export
	// ��    ��: pUIWnd������ָ��
	// �� �� ֵ: N/A
	// ��    ��: superlive/wangshuhe
	// ��    ��: ˢ�´��ڱ���
	//******************************************************************************
	DLLAPI( LRESULT ) UpdateBkGnd( HWND hWnd, IUIBase* pUIWnd = NULL );

#else
	//******************************************************************************
	// ��    ��: UpdateBkGnd
	// ����Ȩ��: export
	// ��    ��: hWnd�����ڱ���
	// �� �� ֵ: N/A
	// ��    ��: superlive/wangshuhe
	// ��    ��: ˢ�´��ڱ���
	//******************************************************************************
	DLLAPI( LRESULT ) UpdateBkGnd( HWND hWnd );
#endif
	//******************************************************************************
	// ��    ��: RegisterUIWnd
	// ����Ȩ��: export
	// ��    ��: pUIwnd ע�ᴰ��ָ��
	// ��    ��: pInsertBefore	ע��λ��
	// �� �� ֵ: HRESULT S_OK ���ĳɹ� S_FALSE ����ʧ��
	// ��    ��: superlive/wangshuhe
	// ��    ��: ������Ϣ
	//******************************************************************************
	DLLAPI(HRESULT) RegisterUIWnd( IUIBase* pUIwnd, IUIBase* pInsertBefore = NULL );

	//******************************************************************************
	// ��    ��: UnregisterUIWnd
	// ����Ȩ��: export
	// ��    ��: pUIwnd ע������ָ��
	// �� �� ֵ: HRESULT S_OK �˶��ɹ� S_FALSE �˶�ʧ��
	// ��    ��: superlive/wangshuhe
	// ��    ��: ע������
	//******************************************************************************
	DLLAPI(HRESULT) UnregisterUIWnd( IUIBase* pUIwnd  );
	//******************************************************************************
	// ��    ��: InvalidRect
	// ����Ȩ��: export
	// ��    ��: pUIwnd ˢ�´���
	// ��    ��: lpRect	ˢ������
	// ��    ��: nMode  ˢ��ģʽ
	// �� �� ֵ: HRESULT S_OK �˶��ɹ� S_FALSE �˶�ʧ��
	// ��    ��: superlive/wangshuhe
	// ��    ��: ˢ�´�������
	//******************************************************************************
	DLLAPI(BOOL)  InvalidRect( HWND hWnd, LPCRECT lpRect, int nMode = 0  );
	DLLAPI(IUIBase*) SetUIFocus( IUIBase* pUI );
	DLLAPI(IUIBase*) GetUIFocus( );
	DLLAPI(IUIBase*) SetUIWndMouseIn( IUIBase* pUI );
	DLLAPI(IUIBase*) GetUIWndMouseIn( );
	DLLAPI(void)	 SetUICapture( IUIBase* pCapture );
	DLLAPI(void)	 ReleaseUICapture( );
	DLLAPI(IUIBase*) GetUICapture();
	

#ifdef __cplusplus
}
#endif

#endif//end of __LAAPI_DEFINE_H__