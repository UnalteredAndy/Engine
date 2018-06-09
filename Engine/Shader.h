#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <fstream>
#include <vector>
#include "RenderSystem.h"
#include "Window.h"
#include"WICTextureLoader.h"
#include "Light.h"
#include<string>

namespace Engine {

	using namespace DirectX;

	class Shader
	{
	public:
		Shader();
		~Shader();

		void setDiffuseMap(const wchar_t*  filename);
		void setNormalMap(const wchar_t* filename);

		void setShadowMap(ID3D11ShaderResourceView *shadowMap) { m_pShadowMap = shadowMap; };

		void buildShader(); // initialize
		void __vectorcall setShaderConstants(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, XMMATRIX lightViewProjMatrix);
		void setLightShaderConstants(Light* light);
		void DrawShader(UINT indexCount);

		void setPS(std::string filename);
		void setVS(std::string filename);
		//Shader(const Shader& other) { };

		//void operator=(const Shader& other) { };

	private:

#pragma pack( push, 16 )
		struct MatrixBufferType
		{
			XMFLOAT4X4 worldMatrix;
			XMFLOAT4X4 viewMatrix;
			XMFLOAT4X4 projMatrix;
			XMFLOAT4X4 worldViewProjMatrix;
			XMFLOAT4X4 lightViewProjMatrix;
		};

		struct LightBufferType
		{
			/* using this later
			struct PointLight
			{
				XMFLOAT3 pos;
				float attenuation;
				XMFLOAT3 color;
				float intensity;
			};
			*/
			struct DirectionalLight
			{
				XMFLOAT3 dir;
				float intensity;
				XMFLOAT4 color;
			} dirLight;
		};
#pragma pack(pop)

		ID3D11VertexShader*  m_pVertexShader;
		ID3D11PixelShader *  m_pPixelShader;
		ID3D11InputLayout *  m_pInputLayout;

		//matrix buffer for VS
		ID3D11Buffer * m_pMatrixBuffer;
		MatrixBufferType  m_pVSConstData;

		//light buffer for PS
		ID3D11Buffer * m_pLightBuffer;
		LightBufferType m_pPSConstData;

		//texture maps
		ID3D11Resource * m_pDiffuseMap;
		ID3D11Resource * m_pNormalsMap;
		ID3D11ShaderResourceView * m_pDiffuseMapSRV;
		ID3D11ShaderResourceView * m_pNormalsMapSRV;
		UINT shaderTextureCount = 0;

		//shadows
		ID3D11ShaderResourceView * m_pShadowMap;

		//shader HLSL variables
		//ID3DX11EffectMatrixVariable * worldViewProjMatrix;

		//shader

		//Make the layout for the shader data
		//input layout
		const unsigned int numElements = 6;// number of elements in the vertex layout

		D3D11_INPUT_ELEMENT_DESC vertLayout[6] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BITANGENT"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD"   , 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	};

}