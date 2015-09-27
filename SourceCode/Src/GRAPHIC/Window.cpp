#include "Window.h"
#include "GraphicsLayer.h"
#include "InputLayer.h"
#include "application.h"
using namespace std;

cWindow* cWindow::m_pGlobalWindow = NULL;

LRESULT CALLBACK GlobalWndProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	assert(MainWindow());

	return MainWindow()->WndProc(hWnd, uMsg, wParam, lParam);
}


cWindow::cWindow(int width, int height,const TCHAR* win_name):
m_Width(width),
m_Height(height),
m_hWnd(NULL)
{
	if (m_pGlobalWindow)
	{
		assert("窗体对象已被实例化！");
	}
	m_pGlobalWindow = this;
	m_bActive = false;

	//保存窗体名称
	m_name = wstring(win_name);
}

cWindow::~cWindow()
{
	DestroyWindow(m_hWnd);
	m_pGlobalWindow = NULL;
}

LRESULT cWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bRunDefault = false;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		MainWindow()->m_hWnd = hWnd;
		MainWindow()->m_bActive = true;
		break;
	}
	case WM_PAINT:
	{
		ValidateRect(hWnd, NULL);
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
			// 退出
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
			break;
		}
		default:
		{
			break;
		}
		}
	}

	case WM_SETFOCUS:
	{
		if (Input())
		{
			Input()->SetFocus();
		}
		break;
	}
	case WM_KILLFOCUS:
	{
		if (Input())
		{
			Input()->KillFocus();
		}
		break;
	}
	case WM_ERASEBKGND:
	{
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		bRunDefault = true;
		break;
	}
	if (bRunDefault)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void cWindow::RegisterClass(WNDCLASSEX* pWc)
{
	WNDCLASSEX wcex;
	if (pWc)
	{
		wcex = *pWc;
	}
	else
	{
		//默认赋值
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = GlobalWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = AppInstance();
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = m_name.c_str();
		wcex.hIconSm = 0;
	}

	if (0 == RegisterClassEx(&wcex))
	{
		assert("[cWindow::RegisterClass]: Registering window class failed!\n");
	}
}

void cWindow::InitInstance()
{

	HINSTANCE hi = AppInstance();

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = m_Width;
	rc.bottom = m_Height;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	/**
	* Usually we capture the return value of CreateWindow,
	* as it is the window handle for our window. However,
	* before CreateWindow finishes, WM_CREATE is called
	* on our window, and we just capture the window handle
	* there.
	*/
	CreateWindow(
		m_name.c_str(),
		m_name.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		AppInstance(),
		NULL);

	if (!m_hWnd)
	{
		assert("[cWindow::InitInstance]: Creating window failed!\n");
	}

	// 绘制
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}


bool cWindow::HasMessages()
{
	MSG msg;
	if (0 == PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE))
	{
		// 没有消息
		return false;
	}
	return true;
}

eResult cWindow::Pump()
{
	MSG msg;
	PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

	if (WM_QUIT == msg.message)
		return resFalse;

	TranslateMessage(&msg);
	DispatchMessage(&msg);
	//初始化背景色
	Graphics()->Render();
	return resAllGood;
}
