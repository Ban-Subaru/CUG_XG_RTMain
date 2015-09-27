#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <memory.h>
#include "config.h"
class cInputLayer;

/**
* ���ռ�������
*/
struct iKeyboardReceiver
{
	virtual void KeyUp(int key){};
	virtual void KeyDown(int key){};
};


class cKeyboard
{

	LPDIRECTINPUTDEVICE8	m_pDevice;	// �����豸

	char					m_keyState[256];

	iKeyboardReceiver*		m_pTarget;

public:

	void ClearTable()
	{
		memset(m_keyState, 0, sizeof(char) * 256);
	}

	cKeyboard(HWND hWnd);
	~cKeyboard();

	// ȷ��keyȷʵ����
	bool Poll(int key);

	// �������̽����¼�
	void SetReceiver(iKeyboardReceiver* pTarget);

	eResult Update();
};

#endif //_KEYBOARD_H
