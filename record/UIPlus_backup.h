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
	// 方    法: UpdateBkGnd
	// 访问权限: export
	// 参    数: pUIWnd　窗口指针
	// 返 回 值: N/A
	// 作    者: superlive/wangshuhe
	// 描    述: 刷新窗口背景
	//******************************************************************************
	DLLAPI( LRESULT ) UpdateBkGnd( HWND hWnd, IUIBase* pUIWnd = NULL );

#else
	//******************************************************************************
	// 方    法: UpdateBkGnd
	// 访问权限: export
	// 参    数: hWnd　窗口背景
	// 返 回 值: N/A
	// 作    者: superlive/wangshuhe
	// 描    述: 刷新窗口背景
	//******************************************************************************
	DLLAPI( LRESULT ) UpdateBkGnd( HWND hWnd );
#endif
	//******************************************************************************
	// 方    法: RegisterUIWnd
	// 访问权限: export
	// 参    数: pUIwnd 注册窗口指针
	// 参    数: pInsertBefore	注册位置
	// 返 回 值: HRESULT S_OK 订阅成功 S_FALSE 订阅失败
	// 作    者: superlive/wangshuhe
	// 描    述: 订阅消息
	//******************************************************************************
	DLLAPI(HRESULT) RegisterUIWnd( IUIBase* pUIwnd, IUIBase* pInsertBefore = NULL );

	//******************************************************************************
	// 方    法: UnregisterUIWnd
	// 访问权限: export
	// 参    数: pUIwnd 注销窗口指针
	// 返 回 值: HRESULT S_OK 退订成功 S_FALSE 退订失败
	// 作    者: superlive/wangshuhe
	// 描    述: 注销窗口
	//******************************************************************************
	DLLAPI(HRESULT) UnregisterUIWnd( IUIBase* pUIwnd  );
	//******************************************************************************
	// 方    法: InvalidRect
	// 访问权限: export
	// 参    数: pUIwnd 刷新窗口
	// 参    数: lpRect	刷新区域
	// 参    数: nMode  刷新模式
	// 返 回 值: HRESULT S_OK 退订成功 S_FALSE 退订失败
	// 作    者: superlive/wangshuhe
	// 描    述: 刷新窗口区域
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