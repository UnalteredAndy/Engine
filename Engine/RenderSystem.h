#pragma once
#include<DirectXMath.h>
#include <d3d11_1.h>
#include "Window.h"
#include "SceneManager.h"
#include <vector>

namespace Engine {

	class RenderSystem
	{
	public:
		static RenderSystem * getInstance();
		ID3D11Device * getDevice() { return m_d3d_Device; };
		ID3D11DeviceContext * getImmediateContext() { return m_pImmediateContext; };
		void onResize();
		void init();
		void renderOneFrame(double deltaTime);
		void clearRenderTargetView();
		void clearDepthStencilView();

	private:
		RenderSystem();
		~RenderSystem();

		void renderShadowMaps(float deltaTime);
		void initShadows();

		static RenderSystem * theInstance;
		
		void allocateBuffers();

		ID3D11Device * m_d3d_Device;
		ID3D11RenderTargetView * mRenderTargetView;
		ID3D11DepthStencilView * mDepthStencilView;
		ID3D11DepthStencilState * mDepthStencilState;
		ID3D11Texture2D * pDepthStencil = NULL;

		IDXGISwapChain * m_pSwapChain;
		ID3D11DeviceContext * m_pImmediateContext;

		ID3D11RasterizerState * m_pRasterizerState;//not in use at the moment


		//shadow stuff
		int numShadowMaps = 1;
		int shadowMapHeight = 600, shadowMapWidth = 800;
		std::vector<ID3D11DepthStencilView*> mShadowDSV;
		ID3D11ShaderResourceView * shadowMapSRV;

		DirectX::XMMATRIX lightViewProjMatrix;
	};

}