#pragma once
#include<DirectXMath.h>
#include<vector>

namespace Engine {
	using namespace DirectX;

class Light
{
public:
	Light();
	Light(float intensity, float range);
	~Light();

	void setIntensity(float intensity);
	void setRange(float range);
	void setColor(XMVECTOR color) {
		XMStoreFloat3(&mColor, color);
	}

	float getIntensity() { return intensity; };
	float getRange() { return range; };
	XMVECTOR getColor() { return XMVectorSet(mColor.x, mColor.y, mColor.z, 1.0f); }

	XMMATRIX getOrientation() { return  XMLoadFloat3x3(&mOrientation); };
	XMMATRIX getTransform();
	void setOrientation(CXMMATRIX orientation);

	XMVECTOR getForwardVector();

	void setPosition(XMVECTOR pos);
	XMVECTOR getPosition();

	void setRotation(XMVECTOR rot);
	XMVECTOR getRotation();

	void lootAt(XMVECTOR at);

	//XMMATRIX getProjectionMatrix();
	XMMATRIX getViewMatrix();
	void setLightFrustum(std::vector<XMVECTOR> points, XMMATRIX *projMatrix);

private:
	XMFLOAT3 mColor;
	XMFLOAT3 mPosition;
	XMFLOAT3 mScaling;
	XMFLOAT3 mRotation;
	XMFLOAT3X3 mOrientation;

	//light projection stuff. move this to the camera calss later
	XMMATRIX projectionMatrix;
	float width, aspectRatio;
	float mNear = 0.1f;
	float mFar = 1000.0f;

	float range;
	float intensity;
};

}


