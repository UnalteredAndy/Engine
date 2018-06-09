#include "Shader.h"

using namespace Engine;
using namespace DirectX;

Shader::Shader(): 
	m_pInputLayout(nullptr),
	m_pMatrixBuffer(nullptr),
	m_pPixelShader(nullptr),
	m_pVertexShader(nullptr),
	m_pDiffuseMap(nullptr),
	m_pNormalsMap(nullptr),
	m_pShadowMap(nullptr)
{
	//just builds the shader for now (compile and make buffers)
}


Shader::~Shader()
{
}

void Engine::Shader::setDiffuseMap(const wchar_t* filename)
{
	RenderSystem * rs = RenderSystem::getInstance();
	CreateWICTextureFromFile(rs->getDevice(), rs->getImmediateContext(), filename, &m_pDiffuseMap, &m_pDiffuseMapSRV, 0);
}

void Engine::Shader::setNormalMap(const wchar_t* filename)
{
	RenderSystem * rs = RenderSystem::getInstance();
	CreateWICTextureFromFile(rs->getDevice(), rs->getImmediateContext(), filename, &m_pNormalsMap, &m_pNormalsMapSRV, 0);
}

void Shader::buildShader()
{
	//need this to create the resources
	RenderSystem* rs = RenderSystem::getInstance();
	ID3D11Device * device = rs->getDevice();

	//shader file names/location
	//WCHAR * filename_vs = L"../Engine/Shaders/colorVS.hlsl";
	//WCHAR * filename_ps = L"../Engine/Shaders/colorPS.hlsl";
	//LPCSTR shaderName_vs = "ColorVertexShader";
	//LPCSTR shaderName_ps = "ColorPixelShader";

	HRESULT hr;

	if (m_pVertexShader == nullptr) 
	{
		setVS("../Engine/Shaders/colorVS.cso");
	}

	if (m_pPixelShader == nullptr) 
	{
		setPS("../Engine/Shaders/colorPS.cso");
	}


	//create the Matrix buffer
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;


	m_pVSConstData.worldMatrix = { 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	m_pVSConstData.viewMatrix = { 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	m_pVSConstData.projMatrix = { 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &m_pVSConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	hr = rs->getDevice()->CreateBuffer(&matrixBufferDesc, &InitData, &m_pMatrixBuffer);

	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed creating Matrix buffer: ", L"Shader Error!", 0); // do failed stuff;
	}
	//rs->getImmediateContext()->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);


	//create light input buffer for the pixel shader
	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	m_pPSConstData.dirLight =  { XMFLOAT3(1, 1, 1), 500.0f, XMFLOAT4(1, 1, 1 ,0)} ;
	//and initialize any other lights to default values
	
	//reuse initData from the matrix buffer
	InitData.pSysMem = &m_pPSConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	//create the light buffer
	hr = device->CreateBuffer(&lightBufferDesc, &InitData, &m_pLightBuffer);

	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed creating Light buffer: ", L"Shader Error!", 0); // do failed stuff;
	}

}

void __vectorcall Shader::setShaderConstants(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, XMMATRIX lightViewProjMatrix)
{
	RenderSystem* rs = RenderSystem::getInstance();
	ID3D11DeviceContext * deviceImmediateContext =  rs->getImmediateContext();

	XMStoreFloat4x4(&m_pVSConstData.worldViewProjMatrix, XMMatrixTranspose(worldMatrix *viewMatrix* projMatrix));
	//must transpose first for shader
	XMStoreFloat4x4(&m_pVSConstData.projMatrix, XMMatrixTranspose(projMatrix));
	XMStoreFloat4x4(&m_pVSConstData.viewMatrix, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(&m_pVSConstData.worldMatrix, XMMatrixTranspose(worldMatrix));
	XMStoreFloat4x4(&m_pVSConstData.lightViewProjMatrix, XMMatrixTranspose(lightViewProjMatrix));

	//deviceImmediateContext->UpdateSubresource(m_pMatrixBuffer, 0, nullptr, &m_pVSConstData, 0, 0 );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceImmediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	MatrixBufferType * dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->worldMatrix = m_pVSConstData.worldMatrix;
	dataPtr->viewMatrix = m_pVSConstData.viewMatrix;
	dataPtr->projMatrix = m_pVSConstData.projMatrix;
	dataPtr->worldViewProjMatrix = m_pVSConstData.worldViewProjMatrix;

	deviceImmediateContext->Unmap(m_pMatrixBuffer, 0);
}

void Engine::Shader::setLightShaderConstants(Light * light)
{
	RenderSystem* rs = RenderSystem::getInstance();
	ID3D11DeviceContext * deviceImmediateContext = rs->getImmediateContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceImmediateContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	LightBufferType * dataPtr = (LightBufferType*)mappedResource.pData;

	XMStoreFloat3(&m_pPSConstData.dirLight.dir, light->getForwardVector());

	XMStoreFloat4(&m_pPSConstData.dirLight.color, light->getColor());

	dataPtr->dirLight.intensity = light->getIntensity();
	dataPtr->dirLight.color = m_pPSConstData.dirLight.color;
	dataPtr->dirLight.dir = m_pPSConstData.dirLight.dir;

	deviceImmediateContext->Unmap(m_pLightBuffer, 0);

}

void Shader::DrawShader(UINT indexCount)
{
	RenderSystem* rs = RenderSystem::getInstance();
	ID3D11DeviceContext * deviceImmediateContext = rs->getImmediateContext();

	deviceImmediateContext->IASetInputLayout(m_pInputLayout);

	deviceImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);

	deviceImmediateContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	deviceImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);

	if (m_pLightBuffer != nullptr) {
		deviceImmediateContext->PSSetConstantBuffers(1, 1, &m_pLightBuffer);
	}
	if (m_pDiffuseMap != nullptr) 
	{
		deviceImmediateContext->PSSetShaderResources(0, 1, &m_pDiffuseMapSRV);
	}
	if (m_pNormalsMap != nullptr)
	{
		deviceImmediateContext->PSSetShaderResources(1, 1, &m_pNormalsMapSRV);
	}
	if (m_pShadowMap != nullptr) {
		deviceImmediateContext->PSSetShaderResources(2, 1, &m_pShadowMap);
		//MessageBox(0, L"setting shadow map", 0, 0);
	}
	
	//render
	deviceImmediateContext->DrawIndexed(indexCount, 0, 0);

	//MessageBox(Window::getInstance()->getMainWindow(), L"Rendering Frame: ", L"Shader!", 0);
}

void Engine::Shader::setPS(std::string filename)
{
	RenderSystem* rs = RenderSystem::getInstance();
	ID3D11Device * device = rs->getDevice();

	std::ifstream psFile(filename, std::ios::binary);

	std::vector<char> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };


	//Create shaders from buffers

	HRESULT hr = device->CreatePixelShader(psData.data(), psData.size(), NULL, &m_pPixelShader);
	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed creating vertex shader: ", L"Shader Error!", 0);
		return;
	}
}

void Engine::Shader::setVS(std::string filename)
{

	RenderSystem* rs = RenderSystem::getInstance();
	ID3D11Device * device = rs->getDevice();

	std::ifstream vsFile(filename, std::ios::binary);

	std::vector<char> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };


	//Create shaders from buffers

	HRESULT hr = device->CreateVertexShader(vsData.data(), vsData.size(), NULL, &m_pVertexShader);
	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed creating vertex shader: ", L"Shader Error!", 0);
		return;
	}

	if (m_pInputLayout != nullptr) {
		m_pInputLayout->Release();
	}

	hr = device->CreateInputLayout(vertLayout, numElements, vsData.data(), vsData.size(), &m_pInputLayout);


	if (FAILED(hr))
	{
		LPCWSTR warningMessage = L"Failed creating input layout: ";
		warningMessage += hr;
		MessageBox(Window::getInstance()->getMainWindow(), L"Failed creating input layout: ", L"Shader Error!", 0);
		return;
	}
}

