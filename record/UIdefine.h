#ifndef  __RL_DEFINE_HEADER_201001111039_H__
#define  __RL_DEFINE_HEADER_201001111039_H__
#include <wtypes.h>
// #define INT32 long
// #define UINT32 unsigned long
#define RL_OK 0
#define RL_FAIL -1

//#define  WM_XCTRL_MESSAGE WM_USER + 2


#define NAPos -1
#define X_TRANSPARENT   -1

#define  CLR_DEFAULT_HOTBK		RGB( 128, 0, 128 )
#define  CLR_DEFAULT_SELECTBK	RGB( 0, 128, 128 )
#define  CLR_DEFAULT_EARSE	RGB( 255, 255, 255 )
#define  CLR_DEFAULT_TEXT	RGB( 0, 0, 0 )
#define  CLR_DEFAULT_LINK	RGB( 0, 0, 255 )
#define  CLR_DEFAULT_GRAY	RGB( 119, 119, 119 )
#define  CLR_DEFAULT_BTN	RGB( 119, 119, 119 )

#define  UNIT_TEXT_ID	-1
#define  IDC_RL_SCROLLBAR_VERT  1000
#define  IDC_RL_SCROLLBAR_HORZ  1001

#define  RECT_W( rect ) ((rect).right-(rect).left)
#define  RECT_H( rect ) ((rect).bottom-(rect).top)
#define  RECT_ISEMPTY( rect ) ( ( rect).left == (rect).right|| (rect).top==(rect).bottom )
#define  RECT_EMPTY( rect ) ( rect).left = (rect).right=(rect).top=(rect).bottom=0 
#define  RECT_TL( rect ) (*((LPPOINT)&(rect)))
#define  RECT_EQUAL( rect1, rect2 ) \
	((rect1).right == (rect2).right\
	&& (rect1).bottom == (rect2).bottom\
	&& (rect1).left == (rect2).left\
	&& (rect1).top == (rect2).top )

inline void RECT_TOX( RECT& rect, int x )
{
	(rect).right = (rect).right-(rect).left+x;
	(rect).left=x;
}

inline void RECT_TOY( RECT& rect, int y )
{
	(rect).bottom = (rect).bottom-(rect).top+y;
	(rect).top=y;
}

inline void RECT_TO( RECT& rect, int x, int y )
{
	RECT_TOX( rect, x );
	RECT_TOY( rect, y );
}

inline void RECT_INFLATE( RECT& rect, int left, int top, int right, int bottom )
{
	(rect).left -= (left);
	(rect).top-=(top);
	(rect).right+=(right);
	(rect).bottom+=(bottom);
};

inline RECT RECT_C( int x, int y, int x1, int y1 )
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x1;
	rect.bottom = y1;
	return rect;
}

inline POINT POINT_C( int x, int y )
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	return pt;
}

inline RECT& RECT_S( RECT& rect, int x, int y, int x1, int y1 )
{
	rect.left = x;
	rect.top = y;
	rect.right = x1;
	rect.bottom = y1;
	return rect;
}

#define  DEFAULT_LIST_CAPACITY  10000

//UICtrl消息
/*
WPARAM:RListCtrl id
LPARAM:NM_RECORD
*/
enum XWM_MESSAGE
{
	XWM_LCLICK=	0x1000,
	XWM_LDBCLICK,
	XWM_LBDOWN,
	XWM_RBDOWN,
	XWM_LBUP,
	XWM_RBUP,
	XWM_RCLICK,
	XWM_RDBCLICK,
	XWM_MOUSEMOVE,
	XWM_MOUSEIN,
	XWM_MOUSELEAVE,
	XWM_FOCUSIN,
	XWM_KEYDOWN,
	XWM_KEYUP,
	XWM_FOCUSLEAVE,
	XWM_CHANGED,
	XWM_MENU,
	XWM_NULL,
};

//RListCtrl消息
enum XLAY_MESSAGE
{
	XLAY_LCLICK = XWM_NULL + 0x10,
	XLAY_LDBCLICK,
	XLAY_RCLICK,
	XLAY_RBUP,
	XLAY_LBUP,
	XLAY_LBDOWN,
	XLAY_RBDOWN,
	XLAY_RDBCLICK,
	XLAY_MOUSEMOVE,
	XLAY_MOUSELEAVE,
	XLAY_MOUSEIN,
	XLAY_KEYDOWN,
	XLAY_KEYUP,
	XLAY_MOUSEWHEEL,
	XLAY_SIZECHANGED,
};

//UICtrl类型
enum XW_CLS
{
	XW_NULL,
	XW_TEXT,
	XW_LINK,
	XW_BUTTON,
	XW_IMAGE,
	XW_GROUP,
	XW_SCROLL,
	XW_CHECKBOX,
	XW_COMBO,
	XW_EDIT,
	XW_RICHEDIT,
};

//UICtrl扩展类型
enum XW_STYLE
{
	XWS_NULL		= 0x00000000,
	XWS_SHOWTIPS	= 0x00000001,
	XWS_ENDELLIPSE	= 0x00000002,
	XWS_PATHELLIPSE	= 0x00000004,
	XWS_SINGLELINE  = 0x00000008,
	XWS_HOTSHOW		= 0x00000010,
	XWS_HOTHIDE		= 0x00000020,
	XWS_UNDERLINE   = 0x00000040,
	XWS_AUTOSIZE	= 0x00000080,
	XWS_VSCROLL		= 0x00000100,
	XWS_HSCROLL     = 0x00000200,
	XWS_CAPTION		= 0x00000400,
	XWS_ALIGNLEFT   = 0x00000800,
	XWS_ALIGNRIGHT   = 0x00001000,
	XWS_ALIGNHCENTER = 0x00002000,
	XWS_ALIGNVCENTER = 0x00004000,
	XWS_ALIGNCENTER  = 0x00006000,
	XWS_NOPREFIX	 = 0x00008000,
	XWS_PREFIXHIDE   = 0x00010000,
	XWS_PREFIXSHOW   = 0x00020000,
	XWS_VERTTEXTOUT	 = 0x00040000,
	XWS_RIGHTTOLEFT  = 0x00080000,
	XWS_MOUSETRACE	 = 0x00100000,
	XWS_SINGLESEL	 = 0x00200000,
	XWS_CLIPMODE     = 0x00400000,
};

//BAR控件常量
enum BARDEF
{
	VERT, 
	HORZ, 
	THUMB, 
};

// 行状态
#define RRS_NORMAL			   0x0000
#define RRS_FOCUSED            0x0001
#define RRS_SELECTED           0x0002
#define RRS_HOT				   0x0004

//other defines
#define RL_AUTOPANELHEIGHT		   -1L
#endif
