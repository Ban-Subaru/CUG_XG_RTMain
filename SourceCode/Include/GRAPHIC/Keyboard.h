#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <memory.h>
#include "config.h"
class cInputLayer;

/**
* 接收键盘输入
*/
struct iKeyboardReceiver
{
	virtual void KeyUp(int key){};
	virtual void KeyDown(int key){};
};


class cKeyboard
{

	LPDIRECTINPUTDEVICE8	m_pDevice;	// 输入设备

	char					m_keyState[256];

	iKeyboardReceiver*		m_pTarget;

public:

	void ClearTable()
	{
		memset(m_keyState, 0, sizeof(char) * 256);
	}

	cKeyboard(HWND hWnd);
	~cKeyboard();

	// 确定key确实按下
	bool Poll(int key);

	// 创建键盘接收事件
	void SetReceiver(iKeyboardReceiver* pTarget);

	eResult Update();
};

#endif //_KEYBOARD_H
