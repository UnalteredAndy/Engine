#pragma once
#include<d3d11.h>
#include <DirectXMath.h>
#include<vector>
#include"RenderSystem.h"
#include"Shader.h"
#include"SceneManager.h"
#include<assimp\scene.h>
#include<assimp\Importer.hpp>
#include<assimp\postprocess.h>

namespace Engine {

	using namespace DirectX;

	class Shader;

	class Mesh
	{
	public:

		struct Vertex {
			XMFLOAT4 pos;
			XMFLOAT4 col;
			XMFLOAT4 normal;
			XMFLOAT4 tangent;
			XMFLOAT4 bitangent;
			XMFLOAT2 uv;

		};

		Mesh();
		~Mesh();

		void setPosition(XMVECTOR pos);
		XMVECTOR getPosition();

		void setRotation(XMVECTOR rot);
		XMMATRIX getRotationMatrix();

		void setScalingVector(XMVECTOR scale);
		XMVECTOR getScalingVector();
		XMMATRIX getScalingMatrix();

		void loadMesh(const std::string& filename);

		XMMATRIX getTransform();

		Shader* getShader() { return m_pShader; };
		void setShader(Shader* shader) { m_pShader = shader; };

		UINT getIndexCount() { return indexCount; };
		void DrawMesh();

	private:

		void InitFromScene(const aiScene* pScene, const std::string& Filename);
		void InitMesh(UINT Index, const aiMesh* paiMesh);

		void initBuffer();
		ID3D11Buffer * m_pVertBuffer;
		ID3D11Buffer * m_pIndexBuffer;

		Shader * m_pShader;

		XMFLOAT3 mPosition;
		XMFLOAT3X3 mOrientation;
		XMFLOAT3 mScaling;

		UINT indexCount;

		std::vector<Vertex> vertices;
		std::vector<UINT> indices;
		//std::vector<> m_Entries;
	};

}