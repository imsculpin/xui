#ifndef __UI_PLUS_H__
#define __UI_PLUS_H__
#include "Switch.h"
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef UIPLUS_EXPORTS
#	define DLLAPI(ret) _declspec(dllexport) ret _cdecl
#else
#	define DLLAPI(ret) _declspec(dllimport) ret _cdecl
#endif
	class IImage;
	class IUIBase;
	struct IMG;
	struct Margin;
	struct MENUDATA
	{
		UINT    cbSize;
		UINT    nCmdId;
		TCHAR   pszText[128];
		IImage* pImage;
		BOOL    bEnable;
		MENUDATA*  pSubMenu;
		ACCEL*  pSubAccel;
		int     nSubCount; 
		int     nSubAccelcount;
	};
	typedef BOOL (__stdcall *MENUMSGFILTER)( MSG* pMsg );
	enum MENUATTRIBMASK
	{
		MA_MARGIN	  = 0x00000001,
		MA_BKGND	  = 0x00000002,
		MA_BKITEM	  = 0x00000004,
		MA_SUBARROW   = 0x00000008,
		MA_SEPARATOR  = 0x00000020,
		MA_ITEMHEIGHT = 0x00000040,
		MA_ICONWIDTH  = 0x00000080,
		MA_DRAWMASK   = 0x00000100,
		MA_ALPHA      = 0x00000200,
		MA_SUBMARGIN  = 0x00000400,
		MA_SUBBKGND	  = 0x00000800,
		MA_FONT		  = 0x00001000,
		MA_TEXTCLR	  = 0x00002000,
		MA_GRAYTEXTCLR= 0x00004000,
		MA_ITEMOFFSET = 0x00008000,
		MA_MESSAGEFILTER=0x00010000,
	};

	enum MENUDRAWMASK
	{
		MDM_ACCEL_NULL		   = 0x00000001,
		MDM_ACCEL_LEFT		   = 0x00000002,
		MDM_ACCEL_RIGHT		   = 0x00000004,
		MDM_ICON_LEFT		   = 0x00000010,
		MDM_ICON_RIGHT		   = 0x00000020,
		MDM_ICON_MID		   = 0x00000040,
		MDM_SEPARATOR_FULLITEM = 0x00000080,
		MDM_PREFIX_HIDE		   = 0x00000100,
		MDM_PREFIX_SHOW		   = 0x00000200,
		MDM_PREFIX_NONE		   = 0x00000400,
	};

	struct MENUATTRIBUTE
	{
		UINT uMask;		     //参数掩码MENUATTRIBMASK
		Margin* pMargin;	 //菜单边框
		IMG*	pBkGnd;		 //背景位图
		Margin* pSubMargin;  //子菜单边框
		IMG*	pSubBkGnd;   //子菜单背景位图
		IMG*    pBkItem;	 //菜单项鼠标经过时的背景
		IMG*    pSeparator;  //分隔栏图标
		IImage* pSubArrow;	 //子菜单标识图
		UINT	uItemHeight; //菜单高度
		UINT    uItemOffset; //菜单项偏移值
		UINT    uIconWidth;  //菜单项左侧图标宽度
		UINT    uDrawMask;   //菜单项绘制掩码 见MENUDRAWMASK
		UINT    uAlpha;      //菜单透明度
		HFONT   hFont;		 //菜单字体设置
		DWORD   clrFont;	 //菜单字体颜色
		DWORD   clrGrayFont; //菜单灰化字体颜色
		MENUMSGFILTER filter;//菜单消息过虑
	};

	enum MENUALIGN
	{
		MAL_TOP = 0x00000001,
		MAL_BOTTOM= 0x00000002,
		MAL_LEFT= 0x00000004,
		MAL_RIGHT= 0x00000008,
	};
	//字体样式
	enum FONT_STYLE
	{
		F_NORMAL	 = 0x00,//普通样式
		F_BLOD		 = 0x01,//粗体
		F_ITALIC	 = 0x02,//斜体
		F_BLODITALIC = 0x03,//粗体+斜体
		F_UNDERLINET = 0x04,//下划线
		F_STRIKEOUT  = 0x08,//删除线
	};

	typedef void (__stdcall *UIDEFAULTSKINFN)( IUIBase* pUI );

#define SURPORT_DWM
#ifdef  SURPORT_DWM
	DLLAPI( LRESULT ) EnableBlurWindow( HWND hWnd, bool bEnable );
	DLLAPI( LRESULT ) EnableComposition( bool bEnable );
#endif


	DLLAPI(BOOL) InitUIPlus( UIDEFAULTSKINFN defaultSkin = NULL, DWORD dwReserved = 0 );
	DLLAPI(void) UninitUIPlus(  );
	DLLAPI(UIDEFAULTSKINFN) UIGetDefaultSkinFn(  );

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
	// 方    法: 菜单
	// 访问权限: export
	// 参    数: pMenuData 菜单数据
	// 参    数: fuFlags 菜单标记
	// 参    数: x 菜单x轴原点坐标
	// 参    数: y 菜单y轴原点坐标
	// 返 回 值: UINT 菜单Id
	// 作    者: superlive/wangshuhe
	// 描    述: 弹出菜单
	//******************************************************************************
	DLLAPI(UINT) UITrackPopupMenu( MENUDATA* pMenuData, int nCount, int x, int y, int fuFlags, \
		ACCEL* pAccels = NULL, int nAccelCount = 0, bool bLayered = false );
	DLLAPI(BOOL) UISetMenuAttribute( const MENUATTRIBUTE* pAttrib );

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
	DLLAPI(BOOL)     InvalidRect( HWND hWnd, LPCRECT lpRect, int nMode = 0  );
	DLLAPI(IUIBase*) UISetFocus( IUIBase* pUI );
	DLLAPI(bool)	 UIIsFocusIn( IUIBase* pUI );
	DLLAPI(IUIBase*) UIGetFocus( );
	DLLAPI(IUIBase*) UISetMouseIn( IUIBase* pUI );
	DLLAPI(IUIBase*) UIGetWndMouseIn( );
	DLLAPI(void)	 UISetCapture( IUIBase* pCapture );
	DLLAPI(void)	 UIReleaseCapture( );
	DLLAPI(IUIBase*) UIGetCapture();
	DLLAPI(void)	 UIActiveProcess(  );

	//******************************************************************************
	// 方    法: UIGetDefaultFont
	// 访问权限: export
	// 参    数: NA
	// 返 回 值: 成功 返回字体句柄,否则返回NULL
	// 作    者: superlive/wangshuhe
	// 描    述: 取默认字体
	//******************************************************************************
	DLLAPI(HFONT) UIGetDefaultFont(  );

	//******************************************************************************
	// 方    法: UIGetFont
	// 访问权限: export
	// 参    数: iHeight 字体大小
	// 参    数: lpszFace 字体类型 为NULL值时,使用默认字体
	// 参    数: style  字体样式 参见FONT_STYLE
	// 返 回 值: NA
	// 作    者: superlive/wangshuhe
	// 描    述: 刷新窗口区域
	//******************************************************************************
	DLLAPI(void)  UISetDefalutFont( int iHeight, LPCTSTR lpszFace = NULL, int style=F_NORMAL );

	//******************************************************************************
	// 方    法: UIGetFont
	// 访问权限: export
	// 参    数: iHeight 字体大小
	// 参    数: lpszFace 字体类型 为NULL值时,使用默认字体
	// 参    数: style  字体样式 参见FONT_STYLE
	// 返 回 值: 成功 返回字体句柄,否则返回NULL
	// 作    者: superlive/wangshuhe
	// 描    述: 刷新窗口区域
	//******************************************************************************
	DLLAPI(HFONT)  UIGetFont( int iHeight, LPCTSTR lpszFace = NULL, int style=F_NORMAL );

#ifdef __cplusplus
}
#endif

#endif//end of __LAAPI_DEFINE_H__