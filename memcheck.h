#ifndef __MEM_LEAK_CHECK_H__
#define __MEM_LEAK_CHECK_H__
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h> 
#include<crtdbg.h>

class CheckInstance
{
	friend class DoCheck;
  	CheckInstance()
	{
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	}
};

class DoCheck
{
	static CheckInstance _check;
};

#define MEM_ASSERT( num ) _CrtSetBreakAlloc(num)
#else
#define MEM_ASSERT( num ) 

#endif

#endif