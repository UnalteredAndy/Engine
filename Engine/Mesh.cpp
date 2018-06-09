#include "Mesh.h"

using namespace Engine;
using namespace DirectX;

Mesh::Mesh():
	mPosition(0.0f, 0.0f, 0.0f),
	mScaling(1.0f, 1.0f, 1.0f)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat3x3(&mOrientation, I);
}


Mesh::~Mesh()
{
}

void Engine::Mesh::setPosition(XMVECTOR pos)
{
	XMStoreFloat3(&mPosition, pos);
}

XMVECTOR Engine::Mesh::getPosition()
{
	return XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 1.0f);
}

void Engine::Mesh::setRotation(XMVECTOR rot)
{
	XMFLOAT3 mRotation;
	XMStoreFloat3(&mRotation, rot);
	if (mRotation.x != 0.0f) {
		XMMATRIX o = XMLoadFloat3x3(&mOrientation) * XMMatrixRotationAxis(XMVectorSet(mOrientation._11, mOrientation._12, mOrientation._13, 0.0f), mRotation.x);
		XMStoreFloat3x3(&mOrientation, o);
	}
	if (mRotation.y != 0.0f) {
		XMMATRIX o = XMLoadFloat3x3(&mOrientation) * XMMatrixRotationAxis(XMVectorSet(mOrientation._21, mOrientation._22, mOrientation._23, 0.0f), mRotation.y);
		XMStoreFloat3x3(&mOrientation, o);
	}
	if (mRotation.z != 0.0f) {
		XMMATRIX o = XMLoadFloat3x3(&mOrientation) * XMMatrixRotationAxis(XMVectorSet(mOrientation._31, mOrientation._32, mOrientation._33, 0.0f), mRotation.z);
		XMStoreFloat3x3(&mOrientation, o);
	}
}

XMMATRIX Engine::Mesh::getRotationMatrix()
{
	
	//return  XMLoadFloat4x4(&wm);
	return XMLoadFloat3x3(&mOrientation);
}

void Engine::Mesh::setScalingVector(XMVECTOR scale)
{
	XMStoreFloat3(&mScaling, scale);
}

XMVECTOR Engine::Mesh::getScalingVector()
{
	return XMVectorSet(mScaling.x, mScaling.y, mScaling.z, 1.0f);
}

XMMATRIX Engine::Mesh::getScalingMatrix()
{
	return XMMatrixScaling(mScaling.x, mScaling.y, mScaling.z);
}

XMMATRIX Engine::Mesh::getTransform()
{
	return  getRotationMatrix() * getScalingMatrix() * XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
}

void Engine::Mesh::DrawMesh()
{
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//render geometry
	rs->getImmediateContext()->IASetVertexBuffers(0, 1, &m_pVertBuffer, &stride, &offset);
	rs->getImmediateContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	rs->getImmediateContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::Mesh::loadMesh(const std::string & filename)
{
	//reseale any previously loaded mesh
	//Clear();

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filename.c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (pScene)
	{
		InitFromScene(pScene, filename);
	}
	else
	{
		std::wostringstream outs;
		outs.precision(6);
		outs << L"Error Parsing Mesh!: " << filename.c_str() << L"\n Importer Error String: " << importer.GetErrorString();
		MessageBox(0, outs.str().c_str(), L"Mesh assimp parse error", 0);
	}
}

void Engine::Mesh::InitFromScene(const aiScene * pScene, const std::string & Filename)
{
	//m_Entries.resize(pScene->mNumMeshes);

	// Initialize the meshes in the scene one by one
/*	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}
	*/
	const aiMesh* paiMesh = pScene->mMeshes[0];

	InitMesh(0, paiMesh); // just read the first mesh found. for now we only support one mesh per scene

}

void Engine::Mesh::InitMesh(UINT Index, const aiMesh * paiMesh)
{

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	const aiColor4D ZeroC4D(0.0f, 0.0f, 0.0f, 0.0f);

	//create the vertices
	for (UINT i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiColor4D* pColor = paiMesh->HasVertexColors(0) ? &(paiMesh->mColors[0][i]) : &ZeroC4D;

		const aiVector3D* pTangent = &Zero3D;
		const aiVector3D* pBitangent = &Zero3D;

		
		if (paiMesh->HasTangentsAndBitangents())
		{
			pTangent = &(paiMesh->mTangents[i]);
			pBitangent = &(paiMesh->mBitangents[i]);
		}

		Vertex v({ XMFLOAT4(pPos->x, pPos->y,pPos->z, 1.0f),
				   XMFLOAT4(pColor->r, pColor->g, pColor->b, pColor->a),
			       XMFLOAT4(pNormal->x, pNormal->y, pNormal->z, 0.0f),
			       XMFLOAT4(pTangent->x, pTangent->y, pTangent->z, 0.0f),
			       XMFLOAT4(pBitangent->x, pBitangent->y, pBitangent->z, 0.0f),
			       XMFLOAT2(pTexCoord->x, pTexCoord->y) });

		vertices.push_back(v);
	}

	//create the indices
	for (UINT i = 0; i < paiMesh->mNumFaces; i++)
	{
		for (UINT j = 0; j < paiMesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(paiMesh->mFaces[i].mIndices[j]);
		}
	}

	initBuffer();
}

void Engine::Mesh::initBuffer()
{
	
	//vertex buffer description
	D3D11_BUFFER_DESC  vertBufferDesc;
	vertBufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
	vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.CPUAccessFlags = 0;
	vertBufferDesc.MiscFlags = 0;

	//recource data
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vertices[0];
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	//now create vertex buffer
	RenderSystem *rs = RenderSystem::getInstance();

	HRESULT hr = rs->getDevice()->CreateBuffer(&vertBufferDesc, &InitData, &m_pVertBuffer);

	if (FAILED(hr))
	{
		MessageBox(Window::getInstance()->getMainWindow(), L"Vertex Buffer Creation Failed", L"Mesh!", 0);
	}
	

	indexCount = indices.size();

	//index buffer desc
	CD3D11_BUFFER_DESC indBufferDesc(
		sizeof(UINT) * indexCount,
		D3D11_BIND_INDEX_BUFFER
	);

	//resource data
	D3D11_SUBRESOURCE_DATA indInitData;
	ZeroMemory(&indInitData, sizeof(indInitData));
	indInitData.pSysMem = &indices[0];

	//create the buffer
	hr = rs->getDevice()->CreateBuffer(&indBufferDesc, &indInitData, &m_pIndexBuffer);

	if (FAILED(hr)) {
		MessageBox(Window::getInstance()->getMainWindow(), L"Index Buffer Creation Failed", L"Mesh!", 0);// do failed stuff
	}

	//m_pShader->buildShader();
}
