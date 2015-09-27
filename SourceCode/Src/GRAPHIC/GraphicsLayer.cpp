#include <algorithm>
#include "GraphicsLayer.h"
using namespace std;

/*初始化全局对象*/
cGraphicsLayer* cGraphicsLayer::m_pGlobalGLayer = NULL;

cGraphicsLayer::cGraphicsLayer(HWND hWnd)
{
	if (m_pGlobalGLayer)
		assert("cGraphicsLayer 对象已被实例化 \n");
	m_pGlobalGLayer = this;

	m_hWnd = hWnd;
	m_pDevice = NULL;
	m_pBackBuffer = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
	m_pFont = NULL;
	m_pFontSprite = NULL;
	m_pMessageQueue = NULL;
	m_pDefaultEffect = NULL;
	m_pDefaultTechnique = NULL;
	m_pDefaultInputLayout = NULL;
	m_pmtxWorldVar = NULL;
	m_pmtxViewVar = NULL;
	m_pmtxProjVar = NULL;
	m_pLightDirVar = NULL;
	m_pLightColorVar = NULL;
	m_pDepthStencilBuffer = NULL;
	m_pDepthStencilState = NULL;
	m_pDepthStencilView = NULL;
	m_pNumLightsVar = NULL;

	// 默认的矩阵正交化
	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXMatrixIdentity(&m_mtxView);
	D3DXMatrixIdentity(&m_mtxProj);

	m_iNumLights = 0;
}


void cGraphicsLayer::AddLight(D3DXCOLOR& vColor, D3DXVECTOR3 vDir)
{
	if (m_iNumLights < MAX_LIGHTS)
	{
		m_aLights[m_iNumLights].m_vColor = vColor;
		m_aLights[m_iNumLights].m_vDirection = vDir;
		m_iNumLights++;
	}
}


void cGraphicsLayer::DestroyAll()
{
	if (m_pDevice)
		m_pDevice->ClearState();

	// 安全释放对象
	SafeRelease(m_pDefaultInputLayout);
	SafeRelease(m_pDefaultEffect);
	SafeRelease(m_pMessageQueue);
	SafeRelease(m_pFont);
	SafeRelease(m_pFontSprite);
	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pBackBuffer);
	SafeRelease(m_pDepthStencilBuffer);
	SafeRelease(m_pSwapChain);
	SafeRelease(m_pDevice);

	m_pGlobalGLayer = NULL;
}

cGraphicsLayer::~cGraphicsLayer()
{
	DestroyAll();
}


void cGraphicsLayer::Present()
{
	HRESULT r = S_OK;

	assert(m_pDevice);

	r = m_pSwapChain->Present(0, 0);
	if (FAILED(r))
	{
		OutputDebugString(L"Present Failed!\n");
	}

	DumpMessages();
}

void cGraphicsLayer::DumpMessages()
{
	assert(m_pMessageQueue);

	HRESULT r = 0;

	int iCount = 0;
	while (1)
	{
		iCount++;
		if (iCount > 10)
			break;

		// 得到消息的字段大小
		SIZE_T messageLength = 0;
		r = m_pMessageQueue->GetMessage(0, NULL, &messageLength);
		if (messageLength == 0)
			break;

		// 分配消息空间
		D3D10_MESSAGE * pMessage = (D3D10_MESSAGE*)malloc(messageLength);
		r = m_pMessageQueue->GetMessage(0, pMessage, &messageLength);
		if (FAILED(r))
		{
			OutputDebugString(L"Failed to get Direct3D Message");
			break;
		}

		TCHAR strOutput[2048];

		const char* pCompileErrors = static_cast<const char*>(pMessage->pDescription);
		TCHAR wcsErrors[2048];
		mbstowcs(wcsErrors, pCompileErrors, 2048);

		swprintf_s(strOutput, 2048, L"D3DDMSG [Cat[%i] Sev[%i] ID[%i]: %s\n",
			pMessage->Category, pMessage->Severity, pMessage->ID, wcsErrors);
		OutputDebugString(strOutput);
	}
}

void cGraphicsLayer::Render()
{
	if (m_pDevice == 0)
		return;

	float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
	m_pDevice->ClearRenderTargetView(m_pRenderTargetView, clearColor);

	m_pSwapChain->Present(0, 0);
}


void cGraphicsLayer::Clear(const float(&colClear)[4])
{
	assert(m_pDevice);
	m_pDevice->ClearRenderTargetView(m_pRenderTargetView, colClear);
}

void cGraphicsLayer::ClearDepthStencil(const float fDepth, const UINT8 uiStencil)
{
	assert(m_pDevice);
	m_pDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, fDepth, uiStencil);
}

void cGraphicsLayer::UpdateLights()
{
	int iLightData[4] = { m_iNumLights, 0, 0, 0 };

	m_pNumLightsVar->SetIntVector(iLightData);

	for (int iCurLight = 0; iCurLight < m_iNumLights; iCurLight++)
	{
		m_pLightDirVar->SetFloatVectorArray((float*)m_aLights[iCurLight].m_vDirection, iCurLight, 1);
		m_pLightColorVar->SetFloatVectorArray((float*)m_aLights[iCurLight].m_vColor, iCurLight, 1);
	}
}

void cGraphicsLayer::InitD3D(int width, int height)
{
	HRESULT r = 0;

	// 拷贝窗口的大小
	m_rcScreenRect.left = m_rcScreenRect.top = 480;
	m_rcScreenRect.right = width;
	m_rcScreenRect.bottom = height;

	CreateDeviceAndSwapChain();
	CreateViewport();
	CreateDepthStencilBuffer();
	CreateDebugText();

	// 将渲染目标视图附加到输出合并状态
	m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	CreateDefaultShader();
}

void cGraphicsLayer::CreateDeviceAndSwapChain()
{
	HRESULT r = 0;

	// 构造设备参数
	DXGI_SWAP_CHAIN_DESC descSwap;
	ZeroMemory(&descSwap, sizeof(descSwap));

	// 只需要一块后台缓存
	descSwap.BufferCount = 1;

	// 大小
	descSwap.BufferDesc.Width = m_rcScreenRect.right;
	descSwap.BufferDesc.Height = m_rcScreenRect.bottom;

	// 声明格式
	descSwap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 最大刷新频率60hz
	descSwap.BufferDesc.RefreshRate.Numerator = 60;
	descSwap.BufferDesc.RefreshRate.Denominator = 1;
	descSwap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 连接到windows主窗口
	descSwap.OutputWindow = m_hWnd;

	// No multisampling
	descSwap.SampleDesc.Count = 1;
	descSwap.SampleDesc.Quality = 0;

	// Windowed mode
	descSwap.Windowed = TRUE;

	// 通过硬件加速器创建响应设备指针
	r = D3D10CreateDeviceAndSwapChain(
		NULL,							// 无默认适配器
		D3D10_DRIVER_TYPE_HARDWARE,		// 硬件加速设备
		NULL,							// 不适用软件库来加速
		D3D10_CREATE_DEVICE_DEBUG,		// 允许使用调试模式启动
		D3D10_SDK_VERSION,				// 标明使用的SDK版本
		&descSwap,
		&m_pSwapChain,
		&m_pDevice);

	if (FAILED(r))
	{
		assert("Could not create IDirect3DDevice10");
	}

	// 得到后台缓存的拷贝
	r = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&m_pBackBuffer);
	if (FAILED(r))
	{
		assert("Could not get back buffer");
	}

	// 创建并渲染到指定的视图
	r = m_pDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView);
	if (FAILED(r))
	{
		assert("Could not create render target view");
	}

	r = m_pDevice->QueryInterface(__uuidof(ID3D10InfoQueue), (LPVOID*)&m_pMessageQueue);
	if (FAILED(r))
	{
		assert("Could not create IDirect3DDevice10 message queue");
	}
	m_pMessageQueue->SetMuteDebugOutput(false);	// No muting
	m_pMessageQueue->SetMessageCountLimit(-1);	// 不限制消息
}

void cGraphicsLayer::CreateViewport()
{
	// 创建和后台缓存大小相同的视口
	D3D10_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(vp));
	vp.Width = m_rcScreenRect.right;
	vp.Height = m_rcScreenRect.bottom;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDevice->RSSetViewports(1, &vp);
}

void cGraphicsLayer::CreateDebugText()
{
	// 创建用于显示到渲染区域的字体
	D3DX10CreateFont(m_pDevice,
		15, 0,
		FW_BOLD,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Arial",
		&m_pFont);
	assert(m_pFont);

	D3DX10CreateSprite(m_pDevice, m_uiMAX_CHARS_PER_FRAME, &m_pFontSprite);
}

void cGraphicsLayer::CreateDepthStencilBuffer()
{
	HRESULT r = 0;
	// 创建深度缓存
	D3D10_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = m_rcScreenRect.right;
	descDepth.Height = m_rcScreenRect.bottom;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	r = m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencilBuffer);
	if (FAILED(r))
	{
		assert("Unable to create depth buffer");
	}

	D3D10_DEPTH_STENCIL_DESC descDS;
	ZeroMemory(&descDS, sizeof(descDS));
	descDS.DepthEnable = true;
	descDS.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc = D3D10_COMPARISON_LESS;

	// 模板检测值
	descDS.StencilEnable = true;
	descDS.StencilReadMask = (UINT8)0xFFFFFFFF;
	descDS.StencilWriteMask = (UINT8)0xFFFFFFFF;

	// 如果是屏幕像素则使用模板运算
	descDS.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	descDS.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	descDS.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	descDS.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// 后台缓冲区也是用模板运算
	descDS.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	descDS.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	descDS.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	descDS.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	r = m_pDevice->CreateDepthStencilState(&descDS, &m_pDepthStencilState);
	if (FAILED(r))
	{
		assert("Could not create depth/stencil state");
	}
	m_pDevice->OMSetDepthStencilState(m_pDepthStencilState, 1);

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSView;
	ZeroMemory(&descDSView, sizeof(descDSView));
	descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSView.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0;

	r = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &descDSView, &m_pDepthStencilView);
	if (FAILED(r))
	{
		assert("Could not create depth/stencil view");
	}
}

void cGraphicsLayer::CreateDefaultShader()
{
	HRESULT r = 0;

	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// 打开调试设置
	shaderFlags |= D3D10_SHADER_DEBUG;
#endif

	ID3D10Blob* pErrors = 0;
	// 创建默认的显示效果
	r = D3DX10CreateEffectFromFile(L"..\\SRC\\MEDIA\\DefaultShader.fx", NULL, NULL, "fx_4_0", shaderFlags, 0,
		m_pDevice, NULL, NULL, &m_pDefaultEffect, &pErrors, NULL);
	if (pErrors)
	{
		char* pCompileErrors = static_cast<char*>(pErrors->GetBufferPointer());
		TCHAR wcsErrors[MAX_PATH];
		mbstowcs(wcsErrors, pCompileErrors, MAX_PATH);
		OutputDebugString(wcsErrors);
	}

	if (FAILED(r))
	{
		assert("Could not create default shader - DefaultShader.fx");
	}
	if (m_pDefaultEffect)
	{
		m_pDefaultTechnique = m_pDefaultEffect->GetTechniqueByName("DefaultTechnique");
	}
	if (!m_pDefaultTechnique)
	{
		assert("Could not find default technique in DefaultShader.fx");
	}

	// 创建输入布局
	D3D10_INPUT_ELEMENT_DESC defaultLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uiNumElements = sizeof(defaultLayout) / sizeof(defaultLayout[0]);
	D3D10_PASS_DESC descPass;
	m_pDefaultTechnique->GetPassByIndex(0)->GetDesc(&descPass);
	r = m_pDevice->CreateInputLayout(defaultLayout, uiNumElements,
		descPass.pIAInputSignature, descPass.IAInputSignatureSize, &m_pDefaultInputLayout);
	if (FAILED(r))
	{
		assert("Could not create default layout");
	}
	m_pDevice->IASetInputLayout(m_pDefaultInputLayout);

	m_pmtxWorldVar = m_pDefaultEffect->GetVariableByName("g_mtxWorld")->AsMatrix();
	m_pmtxViewVar = m_pDefaultEffect->GetVariableByName("g_mtxView")->AsMatrix();
	m_pmtxProjVar = m_pDefaultEffect->GetVariableByName("g_mtxProj")->AsMatrix();

	D3DXMATRIX mtxWorld;
	D3DXMatrixIdentity(&mtxWorld);
	SetWorldMtx(mtxWorld);

	D3DXMATRIX mtxView;
	D3DXVECTOR3 vCamPos(0.0f, 1.0f, -3.0f);
	D3DXVECTOR3 vCamAt(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vCamUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mtxView, &vCamPos, &vCamAt, &vCamUp);
	SetViewMtx(mtxView);

	D3DXMATRIX mtxProj;
	D3DXMatrixPerspectiveFovLH(&mtxProj, (float)D3DX_PI * 0.5f,
		m_rcScreenRect.right / (float)m_rcScreenRect.bottom, 0.1f, 100.0f);
	SetProjMtx(mtxProj);

	UpdateMatrices();

	m_pLightDirVar = m_pDefaultEffect->GetVariableByName("g_vLightDirections")->AsVector();
	m_pLightColorVar = m_pDefaultEffect->GetVariableByName("g_vLightColors")->AsVector();
	m_pNumLightsVar = m_pDefaultEffect->GetVariableByName("g_viLightStatus")->AsVector();
}

void cGraphicsLayer::DrawTextString(int x, int y,
	D3DXCOLOR color, const TCHAR* strOutput)
{
	m_pFontSprite->Begin(0);
	RECT rect = { x, y, m_rcScreenRect.right, m_rcScreenRect.bottom };
	m_pFont->DrawText(m_pFontSprite, strOutput, -1, &rect, DT_LEFT, color);
	m_pFontSprite->End();
}


void cGraphicsLayer::Create(HWND hWnd, short width, short height)
{
	new cGraphicsLayer(hWnd); // 默认对象

	// 默认的Direct3D初始化
	Graphics()->InitD3D(width, height);
}

void cGraphicsLayer::UpdateMatrices()
{
	/*static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if( dwTimeStart == 0 )
	dwTimeStart = dwTimeCur;
	float   t = ( dwTimeCur - dwTimeStart) / 1000.0f;

	D3DXMatrixRotationY( &m_mtxWorld, t );*/
	//矩阵更新
	m_pmtxWorldVar->SetMatrix((float*)&m_mtxWorld);
	m_pmtxViewVar->SetMatrix((float*)&m_mtxView);
	m_pmtxProjVar->SetMatrix((float*)&m_mtxProj);
}