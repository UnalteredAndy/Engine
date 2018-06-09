#pragma once
#include"Mesh.h"
#include"Shader.h"
#include"SceneManager.h"

namespace Engine {
	using namespace DirectX;

class Entity
{
public:
	Entity();
	~Entity();

	void Draw();

private:
	Mesh * m_pMesh;
};
}


