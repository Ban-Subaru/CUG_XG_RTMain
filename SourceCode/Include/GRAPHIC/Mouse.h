#ifndef _MOUSE_H
#define _MOUSE_H
#include "config.h"
#include <dinput.h>

/**
* 接收用户鼠标输入
*/
struct iMouseReceiver
{
	virtual void MouseMoved(int dx, int dy){};
	virtual void MouseButtonUp(int button){};
	virtual void MouseButtonDown(int button){};
};


class cMouse
{
	LPDIRECTINPUTDEVICE8	m_pDevice;

	DIMOUSESTATE			m_lastState;

	iMouseReceiver*			m_pTarget;

public:

	cMouse(HWND hWnd, bool bExclusive);
	~cMouse();

	/**
	* 接收输入信息
	*/
	void SetReceiver(iMouseReceiver* pTarget);

	eResult Update();

	eResult Acquire();
	void UnAcquire();
};

#endif // _MOUSE_H
