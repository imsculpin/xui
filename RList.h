// RList.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once
#include "Aplication.h"
#include "RListDlg.h"

class CRList:public CAplication
{
public:
	BOOL InitInstance();
	int ExitInstance();
private:
	BOOL _LoadImages();
	CRListDlg dlg;
	/*!> λͼ�ڻ����е�λ�� */
	enum{ BMP_PHONE_NORMAL = 0, BMP_PHONE_HOT, BMP_PHONE_PRESSED, BMP_SMS_NORMAL, BMP_SMS_HOT, BMP_SMS_PRESSED,\
		BMP_SEND, BMP_RECIEVED, BMP_ROWHOT, BMP_ROWSEL, BMP_OFFLINE, BMP_ONLINE,BMP_L1,BMP_L2,BMP_L3,BMP_R1,BMP_R2,BMP_R3, BMP_PAD, BMP_CAR, BMP_END };
	static HIMAGE	  m_RLBmpArray[BMP_END];   //λͼ����
};