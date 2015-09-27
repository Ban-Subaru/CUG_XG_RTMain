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
	resFailed = -1, // ����δ��
	resNotImpl = -2, // ����δʵ��
	resForceDWord = 0x7FFFFFFF
};


/*
cWindow
	Window ��װ��
*/
class cWindow
{
public:
	cWindow(int width,int height,const TCHAR* win_name = L"Ĭ�ϴ���");
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

	//true��ʾ��Ϣ�ڳ�ȡ������
	bool HasMessages();

	// res_AllGood -> ��Ⱦ
	// res_False -> got a WM_QUIT.  ֹͣ
	eResult Pump();

	//����Ψһʵ��
	static cWindow* GetMainWindow()
	{
		return m_pGlobalWindow;
	}
protected:
	/*g�߶� ���*/
	int m_Width, m_Height;

	/*windows���*/
	HWND m_hWnd;

	//���ڱ���
	std::wstring m_name;

	//��Ǵ����Ƿ�Ϊ��Ծ̬
	bool m_bActive;

	static cWindow* m_pGlobalWindow;
};
inline cWindow* MainWindow()
{
	return cWindow::GetMainWindow();
}
#endif //WINDOW_H_