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
		UINT uMask;		     //��������MENUATTRIBMASK
		Margin* pMargin;	 //�˵��߿�
		IMG*	pBkGnd;		 //����λͼ
		Margin* pSubMargin;  //�Ӳ˵��߿�
		IMG*	pSubBkGnd;   //�Ӳ˵�����λͼ
		IMG*    pBkItem;	 //�˵�����꾭��ʱ�ı���
		IMG*    pSeparator;  //�ָ���ͼ��
		IImage* pSubArrow;	 //�Ӳ˵���ʶͼ
		UINT	uItemHeight; //�˵��߶�
		UINT    uItemOffset; //�˵���ƫ��ֵ
		UINT    uIconWidth;  //�˵������ͼ����
		UINT    uDrawMask;   //�˵���������� ��MENUDRAWMASK
		UINT    uAlpha;      //�˵�͸����
		HFONT   hFont;		 //�˵���������
		DWORD   clrFont;	 //�˵�������ɫ
		DWORD   clrGrayFont; //�˵��һ�������ɫ
		MENUMSGFILTER filter;//�˵���Ϣ����
	};

	enum MENUALIGN
	{
		MAL_TOP = 0x00000001,
		MAL_BOTTOM= 0x00000002,
		MAL_LEFT= 0x00000004,
		MAL_RIGHT= 0x00000008,
	};
	//������ʽ
	enum FONT_STYLE
	{
		F_NORMAL	 = 0x00,//��ͨ��ʽ
		F_BLOD		 = 0x01,//����
		F_ITALIC	 = 0x02,//б��
		F_BLODITALIC = 0x03,//����+б��
		F_UNDERLINET = 0x04,//�»���
		F_STRIKEOUT  = 0x08,//ɾ����
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
	// ��    ��: �˵�
	// ����Ȩ��: export
	// ��    ��: pMenuData �˵�����
	// ��    ��: fuFlags �˵����
	// ��    ��: x �˵�x��ԭ������
	// ��    ��: y �˵�y��ԭ������
	// �� �� ֵ: UINT �˵�Id
	// ��    ��: superlive/wangshuhe
	// ��    ��: �����˵�
	//******************************************************************************
	DLLAPI(UINT) UITrackPopupMenu( MENUDATA* pMenuData, int nCount, int x, int y, int fuFlags, \
		ACCEL* pAccels = NULL, int nAccelCount = 0, bool bLayered = false );
	DLLAPI(BOOL) UISetMenuAttribute( const MENUATTRIBUTE* pAttrib );

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
	// ��    ��: UIGetDefaultFont
	// ����Ȩ��: export
	// ��    ��: NA
	// �� �� ֵ: �ɹ� ����������,���򷵻�NULL
	// ��    ��: superlive/wangshuhe
	// ��    ��: ȡĬ������
	//******************************************************************************
	DLLAPI(HFONT) UIGetDefaultFont(  );

	//******************************************************************************
	// ��    ��: UIGetFont
	// ����Ȩ��: export
	// ��    ��: iHeight �����С
	// ��    ��: lpszFace �������� ΪNULLֵʱ,ʹ��Ĭ������
	// ��    ��: style  ������ʽ �μ�FONT_STYLE
	// �� �� ֵ: NA
	// ��    ��: superlive/wangshuhe
	// ��    ��: ˢ�´�������
	//******************************************************************************
	DLLAPI(void)  UISetDefalutFont( int iHeight, LPCTSTR lpszFace = NULL, int style=F_NORMAL );

	//******************************************************************************
	// ��    ��: UIGetFont
	// ����Ȩ��: export
	// ��    ��: iHeight �����С
	// ��    ��: lpszFace �������� ΪNULLֵʱ,ʹ��Ĭ������
	// ��    ��: style  ������ʽ �μ�FONT_STYLE
	// �� �� ֵ: �ɹ� ����������,���򷵻�NULL
	// ��    ��: superlive/wangshuhe
	// ��    ��: ˢ�´�������
	//******************************************************************************
	DLLAPI(HFONT)  UIGetFont( int iHeight, LPCTSTR lpszFace = NULL, int style=F_NORMAL );

#ifdef __cplusplus
}
#endif

#endif//end of __LAAPI_DEFINE_H__