#ifndef WINDOW_H_
#define WINDOW_H_

#include <windows.h>
#include <list>
#include <string>
#include "config.h"


enum eResult
{
	resAllGood = 0, // run
	resFalse = 1, // purse
	resFailed = -1, // 函数未见
	resNotImpl = -2, // 函数未实现
	resForceDWord = 0x7FFFFFFF
};


/*
cWindow
	Window 封装类
*/
class cWindow
{
public:
	cWindow(int width,int height,const TCHAR* win_name = L"默认窗体");
	~cWindow();

	virtual LRESULT WndProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	virtual void RegisterClass(WNDCLASSEX* pWc = NULL);
	virtual void InitInstance();

	HWND GetHWnd()
	{
		return m_hWnd;
	}

	bool IsActive()
	{
		return m_bActive;
	}

	//true表示消息在抽取队列中
	bool HasMessages();

	// res_AllGood -> 渲染
	// res_False -> got a WM_QUIT.  停止
	eResult Pump();

	//返回唯一实例
	static cWindow* GetMainWindow()
	{
		return m_pGlobalWindow;
	}
protected:
	/*g高度 宽度*/
	int m_Width, m_Height;

	/*windows句柄*/
	HWND m_hWnd;

	//窗口标题
	std::wstring m_name;

	//标记窗口是否为活跃态
	bool m_bActive;

	static cWindow* m_pGlobalWindow;
};
inline cWindow* MainWindow()
{
	return cWindow::GetMainWindow();
}
#endif //WINDOW_H_