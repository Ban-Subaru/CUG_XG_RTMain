#ifndef _GRAPHICS_LAYER_H
#define _GRAPHICS_LAYER_H

#include "DXHelper.h"
#include "config.h"


class cApplication;

class cGraphicsLayer
{
public:
	~cGraphicsLayer();
	virtual void InitD3D(int width, int height);
	/*添加光源*/
	virtual void AddLight(D3DXCOLOR& vColor, D3DXVECTOR3 vDir);
	// 发送光色信息到shader
	virtual void UpdateLights();

	// 设置转换信息道矩阵
	void SetWorldMtx(const D3DXMATRIX& mtxWorld){ m_mtxWorld = mtxWorld; }
	void SetViewMtx(const D3DXMATRIX& mtxView){ m_mtxView = mtxView; }
	void SetProjMtx(const D3DXMATRIX& mtxProj){ m_mtxProj = mtxProj; }

	// 得到转换信息道矩阵
	const D3DXMATRIX& GetWorldMtx() const { return m_mtxWorld; }
	const D3DXMATRIX& GetViewMtx() const { return m_mtxView; }
	const D3DXMATRIX& GetProjdMtx() const { return m_mtxProj; }

	// 发送转换信息到shader
	void UpdateMatrices();

	// 数据销毁
	void DestroyAll();
	/**
	* 默认的消息响应函数
	*/
	void CreateDeviceAndSwapChain();
	void CreateViewport();
	void CreateDebugText();
	void CreateDepthStencilBuffer();
	void CreateDefaultShader();

	/**
	* 通过dx将文本写到屏幕上
	* 使用图形设备接口会更快
	*/
	void DrawTextString(int x, int y, D3DXCOLOR color, const TCHAR* str);

	ID3D10EffectTechnique* GetDefaultTechnique() const { return m_pDefaultTechnique; }

	HWND GetHWnd(){ return m_hWnd; }

	//初始化失败发出消息
	void DumpMessages();
	//==========--------------------------  Accessor functions

	// 得到设备指针
	ID3D10Device* GetDevice()
	{
		return m_pDevice;
	}

	virtual void Render();

	// 得到后台缓冲去区指针
	ID3D10Texture2D* GetBackBuffer()
	{
		return m_pBackBuffer;
	}

	// 得到窗口的宽度
	int Width() const
	{
		return m_rcScreenRect.right;
	}

	//得到窗口的高度
	int Height() const
	{
		return m_rcScreenRect.bottom;
	}

	// 展现了后台缓区到屏幕
	void Present();

	// 清空后台缓冲
	void Clear(const float(&colClear)[4]);

	// 清除深度模板缓存
	void ClearDepthStencil(const float fDepth, const UINT8 uiStencil);

	// 得到指向主图形显卡核心的对象
	static cGraphicsLayer* GetGraphics()
	{
		return m_pGlobalGLayer;
	}

	// 默认的this对象
	static void Create(
		HWND hWnd, // windows句柄
		short width, short height); // 设备GUi
public:
	struct cDefaultVertex
	{
	public:
		D3DXVECTOR3 m_vPosition;
		D3DXVECTOR3 m_vNormal;
		D3DXCOLOR m_vColor;
		D3DXVECTOR2 m_TexCoords;
	};

	struct cLight
	{
	public:
		D3DXCOLOR m_vColor;
		D3DXVECTOR3 m_vDirection;
	};
protected:
	cGraphicsLayer(HWND hWnd);                              //Constructor
	static cGraphicsLayer*	m_pGlobalGLayer;				// 指向图形核心的全局静态对象

	HWND						m_hWnd;						//windows句柄

	ID3D10Device*				m_pDevice;					//IDirect3DDevice10 界面指针

	ID3D10Texture2D*			m_pBackBuffer;				//指向纹理缓冲区的指针

	ID3D10Texture2D*			m_pDepthStencilBuffer;		//指向深度、模板缓冲区的指针
	ID3D10DepthStencilState*	m_pDepthStencilState;		//深度、模板缓冲区的状态
	ID3D10DepthStencilView*		m_pDepthStencilView;		//深度模板缓存视图

	ID3D10RenderTargetView*		m_pRenderTargetView;		//指向渲染目标视图的指针

	IDXGISwapChain*				m_pSwapChain;				//交换链指针

	RECT						m_rcScreenRect;				//分辨率的大小

	ID3DX10Font*				m_pFont;					//显示字体
	ID3DX10Sprite*				m_pFontSprite;				//字体模块

	ID3D10InfoQueue*			m_pMessageQueue;			//用于保存D3D消息的队列

	static const UINT			m_uiMAX_CHARS_PER_FRAME = 512;	//最大帧数

	ID3D10Effect*				m_pDefaultEffect;			//默认的渲染着色器
	ID3D10EffectTechnique*		m_pDefaultTechnique;		//默认的渲染方法
	ID3D10InputLayout*			m_pDefaultInputLayout;      //默认的顶点布局

	ID3D10EffectMatrixVariable* m_pmtxWorldVar;		// Pointer to world matrix variable in the default shader
	ID3D10EffectMatrixVariable* m_pmtxViewVar;		// Pointer to the view matrix variable in the default shader
	ID3D10EffectMatrixVariable* m_pmtxProjVar;		// Pointer to the projection matrix variable in the default shader

	ID3D10EffectVectorVariable* m_pLightDirVar;		// Pointer to the light direction array in the shader
	ID3D10EffectVectorVariable* m_pLightColorVar;	// Pointer to the light color array in the shader
	ID3D10EffectVectorVariable* m_pNumLightsVar;	// Pointer to the variable holding the number of lights 

	D3DXMATRIX m_mtxWorld;					// 世界 matrix
	D3DXMATRIX m_mtxView;					// 摄像机 matrix
	D3DXMATRIX m_mtxProj;					// 投影 matrix

	cLight m_aLights[MAX_LIGHTS];			// 光源对象 array
	int m_iNumLights;						// 活跃光数

};
inline cGraphicsLayer* Graphics()
{
	return cGraphicsLayer::GetGraphics();
}
#endif  //_GRAPHICS_LAYER_H