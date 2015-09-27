#include "InputLayer.h"
#include "window.h"

#include <stack>
using namespace std;

#include "Keyboard.h"


cKeyboard::cKeyboard(HWND hWnd)
{
	m_pTarget = NULL;

	HRESULT hr;

	/**
	* 得到界面句柄
	*/
	LPDIRECTINPUT8 pDI = Input()->GetDInput();

	/**
	* 创建键盘设备
	*
	*/
	hr = Input()->GetDInput()->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL);
	if (FAILED(hr))
	{
		assert("Keyboard could not be created\n");
	}

	/**
	* 设置键盘数据格式
	*/
	hr = m_pDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		SafeRelease(m_pDevice);
		assert("Keyboard could not be created\n");
	}

	/**
	* 设置联合等级
	*/
	hr = m_pDevice->SetCooperativeLevel(
		hWnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		SafeRelease(m_pDevice);
		assert("Keyboard coop level could not be changed\n");
	}

	memset(m_keyState, 0, 256 * sizeof(bool));
}

cKeyboard::~cKeyboard()
{
	if (m_pDevice)
	{
		m_pDevice->Unacquire();
		SafeRelease(m_pDevice);
	}
}



void cKeyboard::SetReceiver(iKeyboardReceiver* pTarget)
{
	// 设置接收新的目标事件
	m_pTarget = pTarget;
}


bool cKeyboard::Poll(int key)
{
	if (m_keyState[key] & 0x80)
		return true;
	return false;
}


eResult cKeyboard::Update()
{
	BYTE     newState[256];
	HRESULT  hr;

	hr = m_pDevice->Poll();
	hr = m_pDevice->GetDeviceState(sizeof(newState), (LPVOID)&newState);

	if (FAILED(hr))
	{
		hr = m_pDevice->Acquire();
		if (FAILED(hr))
		{
			return resFailed;
		}

		hr = m_pDevice->Poll();
		hr = m_pDevice->GetDeviceState(sizeof(newState), (LPVOID)&newState);
		if (FAILED(hr))
		{
			return resFailed;
		}
	}


	if (m_pTarget)
	{
		int i;
		for (i = 0; i< 256; i++)
		{
			if (m_keyState[i] != newState[i])
			{
				// 在最后检测key发生的事件
				if (!(newState[i] & 0x80))
				{
					// 按键抬起
					m_pTarget->KeyUp(i);
				}
				else
				{
					// 在按键按下时，如果有需要做的事请添加
				}
			}

			// 保存结束状态
			m_keyState[i] = newState[i];

			if (Poll(i))
			{
				// 按键按下
				m_pTarget->KeyDown(i);
			}
		}
	}
	else
	{
		// 保存新的结束状态
		memcpy(m_keyState, newState, 256);
	}

	return resAllGood;
}