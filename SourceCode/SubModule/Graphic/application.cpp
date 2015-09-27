
#include <list>
using namespace std;

#include "config.h"
#include "application.h"
cApplication* cApplication::m_pGlobalApp = NULL;

HINSTANCE g_hInstance;

HINSTANCE AppInstance()
{
	return g_hInstance;
}

/**
* ��WinMain�����ڿ���
*/
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{

	cApplication* pApp;

	g_hInstance = hInstance;

    pApp = CreateApplication();

	pApp->Init();
	pApp->SceneInit();
	pApp->Run();

	if (Graphics())
	{
		Graphics()->DestroyAll();
	}

	delete pApp;
	return 0;
}



cApplication::cApplication()
{
	if (m_pGlobalApp)
	{
		assert("Ӧ�ó����Ѿ�������!\n");
	}
	m_pGlobalApp = this;

	m_title = wstring(L"Ĭ�ϴ�������");
	m_width = 1280;
	m_height = 960;
	m_bpp = 128;
	m_bActive = true;
}


cApplication::~cApplication()
{
	delete Graphics();
	delete Input();
	delete MainWindow();
}


void cApplication::Init()
{
	InitPrimaryWindow();
	InitGraphics();
	InitInput();
	InitExtraSubsystems();
}


void cApplication::Run()
{
	bool done = false;

	static float lastTime = (float)timeGetTime();

	while (!done)
	{
		/**
		* �Ƿ���Ҫ�˳�
		*/
			
		if( Input()->GetKeyboard() )
		{
		if( Input()->GetKeyboard()->Poll( DIK_ESCAPE ) ||
		Input()->GetKeyboard()->Poll( DIK_Q ) )
		{
		PostMessage( MainWindow()->GetHWnd(), WM_CLOSE, 0, 0 );
		}
	}


		/**
		* Message pump
		*/
		while (!done && MainWindow()->HasMessages())
		{
			if (resFalse == MainWindow()->Pump())
				done = true;
		}



		/**
		* We're about ready to let the class draw the frame.
		* find out how much time elapsed since the last frame
		* we calc these whether we have focus or not, to avoid
		* a large delta once we start rendering after idle time
		*/
		float currTime = (float)timeGetTime();
		float delta = (currTime - lastTime) / 1000.f;

		if (m_bActive)
		{
			// ���������豸
			if (Input())
				Input()->UpdateDevices();

			DoFrame(delta);
		}
		else
		{
			DoIdleFrame(delta);
		}

		lastTime = currTime;
	}
}


void cApplication::DoFrame(float timeDelta)
{
	assert("��Ҫ�̳�");
}

void cApplication::DoIdleFrame(float timeDelta)
{
	assert("��Ҫ�̳�");
}


void cApplication::SceneInit()
{
	assert("��Ҫ�̳�");
}


void cApplication::InitPrimaryWindow()
{
	new cWindow(m_width, m_height, m_title.c_str());

	MainWindow()->RegisterClass();
	MainWindow()->InitInstance();
}


void cApplication::InitGraphics()
{

	cGraphicsLayer::Create(
		MainWindow()->GetHWnd(),
		m_width, m_height);
}


void cApplication::InitInput()
{
	cInputLayer::Create(AppInstance(), MainWindow()->GetHWnd(), NULL, true, true);
}


void cApplication::InitExtraSubsystems()
{
}


void cApplication::SceneEnd()
{
}
