#include "RenderSystem.h"
#include<comdef.h>

using namespace Engine;
using namespace DirectX;
float rotation = 0;
RenderSystem * RenderSystem::theInstance = nullptr;

RenderSystem* RenderSystem::getInstance()
{
	if (RenderSystem::theInstance == nullptr) {
		RenderSystem::theInstance = new RenderSystem();
		Window::getInstance()->init();
		RenderSystem::theInstance->init();
		
	}
	return RenderSystem::theInstance;


}

RenderSystem::RenderSystem() :
	m_d3d_Device(0),
	mRenderTargetView(0),
	m_pSwapChain(0),
	m_pImmediateContext(0),
	lightViewProjMatrix(XMMatrixIdentity()),
	shadowMapSRV(nullptr)
{
}


RenderSystem::~RenderSystem()
{
}


void RenderSystem::init()
{
	D3D_FEATURE_LEVEL featureLevelsList[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1 }; // a choice of feature levels for d3d to choose from
	int numFeatureLevelsRequested = 3; //size of the array above
	D3D_FEATURE_LEVEL featureLevel; // the supported feature level

	DXGI_SWAP_CHAIN_DESC sd;

	Window * wnd = Window::getInstance();
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferCount = 1;
	sd.BufferDesc.Height = wnd->getHeight();
	sd.BufferDesc.Width = wnd->getWidth();
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.Windowed = true; //generally set this to true and let user set it to false later to prevent display errors see docs https://msdn.microsoft.com/en-us/library/windows/desktop/bb173075
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = 0;
	sd.OutputWindow = wnd->getMainWindow();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		featureLevelsList,
		numFeatureLevelsRequested,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain,
		&m_d3d_Device,
		&featureLevel,
		&m_pImmediateContext);

	if (FAILED(hr)) {
		//device creation failed
		//SHOW_ERROR(hr);
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return;
	}
	else {
		//MessageBox(0, L"D3D11CreateDevice Success.", L"Yay!", 0);
	}               

	//call on resize here ( allocate the buffers)
	//onResize();
	allocateBuffers();

	return;
}

void Engine::RenderSystem::renderShadowMaps(float deltaTime)
{
	SceneManager * sm = SceneManager::getInstance();

	//first check if we don´t already have a shadow map set up
	if (shadowMapSRV == nullptr)
	{
		initShadows();
	}

	for (UINT i = 0; i < numShadowMaps; i++)
	{
		m_pImmediateContext->OMSetRenderTargets(0, 0, mShadowDSV.at(i));
		m_pImmediateContext->ClearDepthStencilView(mShadowDSV.at(i), D3D10_CLEAR_DEPTH, 1.0f, 0);
		

		Shader * shader = new Shader();
		shader->setVS("../Engine/Shaders/shadowVS.cso");
		shader->setPS("../Engine/Shaders/shadowPS.cso");
		shader->buildShader();

		XMMATRIX lightProjMatrix;
		XMMATRIX lightViewMatrix = sm->sceneLights.at(0)->getViewMatrix();
		sm->sceneLights.at(0)->setLightFrustum(sm->getMainCamera()->getFrustumPoints(), &lightProjMatrix);

		lightViewProjMatrix = lightProjMatrix * lightViewMatrix;

		shader->setLightShaderConstants(sm->sceneLights.at(0));

		//now draw everything
		//this is provisional for testing. this should be replaced with enities or nodes later
		//this for loop is copy/pasted from the renderOneFrame method
		for (Mesh* mesh : SceneManager::getInstance()->sceneMeshes)
		{
			//mesh->setRotation(XMVectorSet(0.0f, deltaTime, 0.0f, 1.0f));
			mesh->DrawMesh();

			//shadow shader. this doesnßt actuall have to be bound to the mesh to work
			
			shader->setShaderConstants(mesh->getTransform(), lightViewMatrix, lightProjMatrix, lightViewProjMatrix);
			shader->DrawShader(mesh->getIndexCount());
		}
	}
	

	//create a viewport
	D3D11_VIEWPORT vp;
	vp.Height = static_cast<float>(shadowMapHeight);
	vp.Width = static_cast<float>(shadowMapWidth);
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
}

void Engine::RenderSystem::initShadows()
{
	//make the shadow map slightly bigger that the window size. we can change this later
	shadowMapHeight = Window::getInstance()->getHeight() * 1.5;
	shadowMapWidth = Window::getInstance()->getWidth() * 1.5;

	//create Depth stencil
	D3D11_TEXTURE2D_DESC dc;
	dc.ArraySize = numShadowMaps;
	dc.Height = static_cast<float> (shadowMapHeight);
	dc.Width = static_cast<float> (shadowMapWidth);
	dc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	dc.Usage = D3D11_USAGE_DEFAULT;
	dc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dc.CPUAccessFlags = 0;
	dc.MipLevels = 1;
	dc.MiscFlags = 0;
	dc.SampleDesc.Count = 1;
	dc.SampleDesc.Quality = 0;

	ID3D11Texture2D* shadowMap = 0;
	HRESULT hr = m_d3d_Device->CreateTexture2D(&dc, 0, &shadowMap);


	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed CreateTexture2D: ", L"RenderSystem Error!", 0); // do failed stuff;
	}
	//we don´t need a depth/stencil description

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//same format at the texture
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;

	//create depth stencil views
	for (int i = 0; i < numShadowMaps; i++)
	{
		ID3D11DepthStencilView * shadowDSV;

		dsvDesc.Texture2DArray.FirstArraySlice = i;
		hr = m_d3d_Device->CreateDepthStencilView(shadowMap, &dsvDesc, &shadowDSV);

		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errMsg = err.ErrorMessage();

			MessageBox(Window::getInstance()->getMainWindow(),errMsg, L"Failed CreateDepthStencilView. Shadow Map.", 0); // do failed stuff;
		}
		mShadowDSV.push_back(shadowDSV);
	}

	//create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = dc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = m_d3d_Device->CreateShaderResourceView(shadowMap, &srvDesc, &shadowMapSRV);

	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		MessageBox(Window::getInstance()->getMainWindow(), L"Failed CreateShaderResourceView. ", L"RenderSystem Error!", 0); // do failed stuff;
	}

	//don´t need this anymore
	shadowMap->Release();

}


void Engine::RenderSystem::renderOneFrame(double deltaTime)
{
	Window * wnd = Window::getInstance();
	SceneManager * sm = SceneManager::getInstance();
	/*
	float color[4];
	color[0] = 0.15f;
	color[1] = 0.34f;
	color[2] = 0.51f;
	color[3] = 0.0f;
	*/
	float color[4];
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 0.0f;

	m_pImmediateContext->ClearRenderTargetView(mRenderTargetView, color);

	//DirectX::XMFLOAT4 bgColor;
	m_pImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//renderShadows
	renderShadowMaps(deltaTime);

	// Set the render target.
	m_pImmediateContext->OMSetRenderTargets( 2, &mRenderTargetView, mDepthStencilView);

	//now draw everything
	//this is provisional for testing. this should be replaced with enities or nodes later
	for(Mesh* mesh : SceneManager::getInstance()->sceneMeshes)
	{
		// float rotSpeed = 15;
		//rotation = (rotation + (deltaTime * rotSpeed));
		//if (rotation > 360)
		//	rotation = 0;

		//mesh->setRotation(XMVectorSet(0.0f, deltaTime, 0.0f, 1.0f));
		mesh->DrawMesh();

		Shader * shader = mesh->getShader();

		shader->setLightShaderConstants(sm->sceneLights.at(0));
		shader->setShaderConstants(mesh->getTransform(), sm->getMainCamera()->getViewMatrix(), sm->getMainCamera()->getProjectionMatrix(), lightViewProjMatrix);
		if (shadowMapSRV == nullptr)
		{
			MessageBox(0, L"null ptr!", 0, 0);
		}
		shader->setShadowMap(shadowMapSRV);
		shader->DrawShader(mesh->getIndexCount());
	}

	//present back buffer 
	m_pSwapChain->Present(1, 0);
}

//allocates buffers for the depth stencil
void RenderSystem::allocateBuffers()
{
	//need this to create the viewport and get width/height of the screen/render region
	Window * wnd = Window::getInstance();

	//the backbuffer
	ID3D11Texture2D* pBackBuffer;

	//create the buffer
	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(pBackBuffer), (void**) &pBackBuffer);

	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed GetBuffer: ", L"RenderSystem Error!", 0); // do failed stuff;
	}

	//create render target view
	hr =  m_d3d_Device->CreateRenderTargetView(pBackBuffer, 0, &mRenderTargetView);
	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed CreateRenderTargetView: ", L"RenderSystem Error!", 0); // do failed stuff;
	}

	pBackBuffer->Release();

	//create Depth stencil
	D3D11_TEXTURE2D_DESC dc;
	dc.ArraySize = 1;
	dc.Height = wnd->getHeight();
	dc.Width = wnd->getWidth();
	dc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dc.Usage = D3D11_USAGE_DEFAULT;
	dc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dc.CPUAccessFlags = 0;
	dc.MipLevels = 1;
	dc.MiscFlags = 0;
	dc.SampleDesc.Count = 1;
	dc.SampleDesc.Quality = 0;

	hr = m_d3d_Device->CreateTexture2D(&dc, NULL, &pDepthStencil);

	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed CreateTexture2D: ", L"RenderSystem Error!", 0); // do failed stuff;
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create depth stencil state
	m_d3d_Device->CreateDepthStencilState(&dsDesc, &mDepthStencilState);

	//bind depth stencil state to OM
	m_pImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	//create depth stencil view
	m_d3d_Device->CreateDepthStencilView(pDepthStencil, &dsvDesc, &mDepthStencilView);

	//bind view
	m_pImmediateContext->OMSetRenderTargets(2, &mRenderTargetView, mDepthStencilView);

	//create a viewport
	D3D11_VIEWPORT vp;
	vp.Height = static_cast<float>(wnd->getHeight());
	vp.Width = static_cast<float>(wnd->getWidth());
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pImmediateContext->RSSetViewports(1, &vp);

	//create a rasterizer state. I dont actually need this right now. But I am using if to turn off backface culling to make debugging easier
	ID3D11RasterizerState * g_pRasterState;

	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = false;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	m_d3d_Device->CreateRasterizerState(&rasterizerState, &g_pRasterState);
	m_pImmediateContext->RSSetState(g_pRasterState);
	
	if (SceneManager::getInstance()->getMainCamera() != NULL) {
		SceneManager::getInstance()->getMainCamera()->setAspectRatio(Window::getInstance()->getAspectRatio());
	}
}

//allocates buffers for the depth stencil
// reallocate buffers when resizing the window
void RenderSystem::onResize()
{
	//TODO: refactor this later
	if (!m_pSwapChain)
		return;

	if (mRenderTargetView) {
		mRenderTargetView->Release();
	}

	m_pImmediateContext->OMSetRenderTargets(0, 0, 0);
	

	Window * win = Window::getInstance();

	m_pSwapChain->ResizeBuffers(1, win->getWidth(), win->getHeight(), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	allocateBuffers();
}
