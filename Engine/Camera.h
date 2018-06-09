#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include"Window.h"
#include<sstream>

namespace Engine {

	using namespace DirectX;

class Camera
{
public:
	Camera(float fov, float aspect, float nearPlane, float farPlane);
	Camera(float fov, float aspect, float nearPlane, float farPlane, XMVECTOR& pos, XMVECTOR& rot);
	~Camera();

	XMMATRIX getOrientation();

	void setPosition(XMVECTOR pos);
	XMVECTOR getPosition();

	void setRotation(XMVECTOR rot);
	XMVECTOR getRotation();

	XMMATRIX getProjectionMatrix();
	XMMATRIX getViewMatrix();

	std::vector<XMVECTOR> getFrustumPoints();
	void setAspectRatio(float newAspectRatio);

private:

	XMMATRIX m_viewMatrix; //the view matrix
	float mFov, mAspect, mNear, mFar;

	XMVECTOR mPosition;
	XMVECTOR mRotation;
};

}


