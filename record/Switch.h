#ifndef __UI_SWITCH_H__
#define __UI_SWITCH_H__

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0500		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0501		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0410 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0501	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

//mfc support
//#define _MFC_SUPPORT
#ifdef  _MFC_SUPPORT
#ifndef _USRDLL
#ifndef _AFXEXT
#define _AFXEXT
#endif
#endif
#ifdef _WINDOWS_
#undef _WINDOWS_
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ��ͷ���ų�����ʹ�õ�����
#endif
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ tstring ���캯��������ʽ��

#define _AFX_ALL_WARNINGS
#include <afxwin.h>         // MFC ���ĺͱ�׼���
#include <afxext.h>         // MFC ��չ
#include <afxdisp.h>        // MFC �Զ�����
#include <afxdtctl.h>		// Internet Explorer 4 �����ؼ��� MFC ֧��
// #ifndef _AFX_NO_AFXCMN_SUPPORT
// #include <CommCtrl.h>
// #include <afxcmn.h>			// Windows �����ؼ��� MFC ֧��
// #endif // _AFX_NO_AFXCMN_SUPPORT
#else//_MFC_SUPPORT
#include <Windows.h>
#include <wtypes.h>
#include <windef.h>
#endif// mfc disable

#include <assert.h>
#include <tchar.h>
#include "smart.h"
#include "sgt.h"

#endif