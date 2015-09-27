#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <windows.h>

#include "GraphicsLayer.h"
#include "InputLayer.h"

class cWindow;


class cApplication
{
protected:

	//==========--------------------------  ��Ա����

	std::wstring	m_title; // ��������
	int		m_width; // �����
	int		m_height; // �����
	int		m_bpp;

	bool	m_bActive; // �Ƿ�Ϊ��Ծ̬

	UINT	m_glFlags; // ͼ����

	static cApplication*	m_pGlobalApp;

	virtual void InitPrimaryWindow();
	virtual void InitGraphics();
	virtual void InitInput();
	virtual void InitExtraSubsystems();

public:

	cApplication();
	virtual ~cApplication();

	/**
	* Ĭ�ϵ�appʵ�֣����Ի��ڴ˸���
	*/
	virtual void Init();

	/**
	* ��Ⱦ��һ�㲻��Ҫ����
	*/
	virtual void Run();
	/**
	* ����ÿһ֡Ӧ�����еĴ���
	* timeDelta == ����һ����Ӧ�����ڵ�ʱ�䣨������㣩
	*/
	virtual void DoFrame(float timeDelta);
	virtual void DoIdleFrame(float timeDelta);


	/**
	* ����Ĭ�Ϻ���,�ɼ̳�
	*/
	virtual void SceneInit();
	virtual void SceneEnd();

	void Pause() { m_bActive = false; }
	void UnPause() { m_bActive = true; }

	static cApplication* GetApplication() { return m_pGlobalApp; }

	static void KillApplication()
	{
		delete m_pGlobalApp;
		m_pGlobalApp = NULL;
	}
};

inline cApplication* Application()
{
	return cApplication::GetApplication();
}

HINSTANCE AppInstance();

/**
* ����Ҫͨ���̳е�����ʵ��
*/
cApplication*	CreateApplication(); // ����һ��ָ����Ч��Ӧ�ó�������ָ��




#endif // _APPLICATION_H
