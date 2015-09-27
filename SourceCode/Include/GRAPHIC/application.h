#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <windows.h>

#include "GraphicsLayer.h"
#include "InputLayer.h"

class cWindow;


class cApplication
{
protected:

	//==========--------------------------  成员变量

	std::wstring	m_title; // 窗口名称
	int		m_width; // 窗体宽
	int		m_height; // 窗体高
	int		m_bpp;

	bool	m_bActive; // 是否为活跃态

	UINT	m_glFlags; // 图层标记

	static cApplication*	m_pGlobalApp;

	virtual void InitPrimaryWindow();
	virtual void InitGraphics();
	virtual void InitInput();
	virtual void InitExtraSubsystems();

public:

	cApplication();
	virtual ~cApplication();

	/**
	* 默认的app实现，可以基于此更改
	*/
	virtual void Init();

	/**
	* 渲染，一般不需要更改
	*/
	virtual void Run();
	/**
	* 处理每一帧应该运行的代码
	* timeDelta == 自上一次响应到现在的时间（以秒计算）
	*/
	virtual void DoFrame(float timeDelta);
	virtual void DoIdleFrame(float timeDelta);


	/**
	* 场景默认函数,可继承
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
* 这需要通过继承的类来实现
*/
cApplication*	CreateApplication(); // 返回一个指向有效的应用程序对象的指针




#endif // _APPLICATION_H
