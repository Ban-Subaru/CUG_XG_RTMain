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
	/*��ӹ�Դ*/
	virtual void AddLight(D3DXCOLOR& vColor, D3DXVECTOR3 vDir);
	// ���͹�ɫ��Ϣ��shader
	virtual void UpdateLights();

	// ����ת����Ϣ������
	void SetWorldMtx(const D3DXMATRIX& mtxWorld){ m_mtxWorld = mtxWorld; }
	void SetViewMtx(const D3DXMATRIX& mtxView){ m_mtxView = mtxView; }
	void SetProjMtx(const D3DXMATRIX& mtxProj){ m_mtxProj = mtxProj; }

	// �õ�ת����Ϣ������
	const D3DXMATRIX& GetWorldMtx() const { return m_mtxWorld; }
	const D3DXMATRIX& GetViewMtx() const { return m_mtxView; }
	const D3DXMATRIX& GetProjdMtx() const { return m_mtxProj; }

	// ����ת����Ϣ��shader
	void UpdateMatrices();

	// ��������
	void DestroyAll();
	/**
	* Ĭ�ϵ���Ϣ��Ӧ����
	*/
	void CreateDeviceAndSwapChain();
	void CreateViewport();
	void CreateDebugText();
	void CreateDepthStencilBuffer();
	void CreateDefaultShader();

	/**
	* ͨ��dx���ı�д����Ļ��
	* ʹ��ͼ���豸�ӿڻ����
	*/
	void DrawTextString(int x, int y, D3DXCOLOR color, const TCHAR* str);

	ID3D10EffectTechnique* GetDefaultTechnique() const { return m_pDefaultTechnique; }

	HWND GetHWnd(){ return m_hWnd; }

	//��ʼ��ʧ�ܷ�����Ϣ
	void DumpMessages();
	//==========--------------------------  Accessor functions

	// �õ��豸ָ��
	ID3D10Device* GetDevice()
	{
		return m_pDevice;
	}

	virtual void Render();

	// �õ���̨����ȥ��ָ��
	ID3D10Texture2D* GetBackBuffer()
	{
		return m_pBackBuffer;
	}

	// �õ����ڵĿ��
	int Width() const
	{
		return m_rcScreenRect.right;
	}

	//�õ����ڵĸ߶�
	int Height() const
	{
		return m_rcScreenRect.bottom;
	}

	// չ���˺�̨��������Ļ
	void Present();

	// ��պ�̨����
	void Clear(const float(&colClear)[4]);

	// ������ģ�建��
	void ClearDepthStencil(const float fDepth, const UINT8 uiStencil);

	// �õ�ָ����ͼ���Կ����ĵĶ���
	static cGraphicsLayer* GetGraphics()
	{
		return m_pGlobalGLayer;
	}

	// Ĭ�ϵ�this����
	static void Create(
		HWND hWnd, // windows���
		short width, short height); // �豸GUi
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
	static cGraphicsLayer*	m_pGlobalGLayer;				// ָ��ͼ�κ��ĵ�ȫ�־�̬����

	HWND						m_hWnd;						//windows���

	ID3D10Device*				m_pDevice;					//IDirect3DDevice10 ����ָ��

	ID3D10Texture2D*			m_pBackBuffer;				//ָ������������ָ��

	ID3D10Texture2D*			m_pDepthStencilBuffer;		//ָ����ȡ�ģ�建������ָ��
	ID3D10DepthStencilState*	m_pDepthStencilState;		//��ȡ�ģ�建������״̬
	ID3D10DepthStencilView*		m_pDepthStencilView;		//���ģ�建����ͼ

	ID3D10RenderTargetView*		m_pRenderTargetView;		//ָ����ȾĿ����ͼ��ָ��

	IDXGISwapChain*				m_pSwapChain;				//������ָ��

	RECT						m_rcScreenRect;				//�ֱ��ʵĴ�С

	ID3DX10Font*				m_pFont;					//��ʾ����
	ID3DX10Sprite*				m_pFontSprite;				//����ģ��

	ID3D10InfoQueue*			m_pMessageQueue;			//���ڱ���D3D��Ϣ�Ķ���

	static const UINT			m_uiMAX_CHARS_PER_FRAME = 512;	//���֡��

	ID3D10Effect*				m_pDefaultEffect;			//Ĭ�ϵ���Ⱦ��ɫ��
	ID3D10EffectTechnique*		m_pDefaultTechnique;		//Ĭ�ϵ���Ⱦ����
	ID3D10InputLayout*			m_pDefaultInputLayout;      //Ĭ�ϵĶ��㲼��

	ID3D10EffectMatrixVariable* m_pmtxWorldVar;		// Pointer to world matrix variable in the default shader
	ID3D10EffectMatrixVariable* m_pmtxViewVar;		// Pointer to the view matrix variable in the default shader
	ID3D10EffectMatrixVariable* m_pmtxProjVar;		// Pointer to the projection matrix variable in the default shader

	ID3D10EffectVectorVariable* m_pLightDirVar;		// Pointer to the light direction array in the shader
	ID3D10EffectVectorVariable* m_pLightColorVar;	// Pointer to the light color array in the shader
	ID3D10EffectVectorVariable* m_pNumLightsVar;	// Pointer to the variable holding the number of lights 

	D3DXMATRIX m_mtxWorld;					// ���� matrix
	D3DXMATRIX m_mtxView;					// ����� matrix
	D3DXMATRIX m_mtxProj;					// ͶӰ matrix

	cLight m_aLights[MAX_LIGHTS];			// ��Դ���� array
	int m_iNumLights;						// ��Ծ����

};
inline cGraphicsLayer* Graphics()
{
	return cGraphicsLayer::GetGraphics();
}
#endif  //_GRAPHICS_LAYER_H