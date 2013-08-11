#ifndef __APLICATION_H__
#define __APLICATION_H__
#include "uithread.h"
class CAplication;

CAplication* GLGetApp();
class CAplication
	:public CThread
{
public:
	CAplication();
	~CAplication();
	
public:
	LPTSTR	   m_pszCommandLine;
	HINSTANCE  m_hInstance;
	int		   m_nShowCmd;
};

#endif