#ifndef _MOUSE_H
#define _MOUSE_H
#include "config.h"
#include <dinput.h>

/**
* �����û��������
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
	* ����������Ϣ
	*/
	void SetReceiver(iMouseReceiver* pTarget);

	eResult Update();

	eResult Acquire();
	void UnAcquire();
};

#endif // _MOUSE_H
