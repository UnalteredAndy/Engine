#include "Light.h"

using namespace Engine;
using namespace DirectX;

Light::Light():
	mPosition(0.0f, 0.0f, 0.0f),
	mScaling(1.0f, 1.0f, 1.0f),
	mRotation(1.0f, 1.0f, 1.0f)
{
	XMStoreFloat3x3(&mOrientation, XMMatrixIdentity());
}


Engine::Light::Light(float intensity, float range):
	range(range),
	intensity(intensity),
	mPosition(0.0f, 0.0f, 0.0f),
	mScaling(1.0f, 1.0f, 1.0f),
	mRotation(1.0f, 1.0f, 1.0f)
{
	XMStoreFloat3x3(&mOrientation, XMMatrixIdentity());
}

Light::~Light()
{
}

void Engine::Light::setIntensity(float intensity)
{
	this->intensity = intensity;
}

void Engine::Light::setRange(float range)
{
	this->range = range;
}


XMMATRIX Engine::Light::getTransform()
{
	return getOrientation()  * XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
}

void Engine::Light::setOrientation(CXMMATRIX orientation)
{
	XMStoreFloat3x3(&mOrientation, orientation);
}

XMVECTOR Engine::Light::getForwardVector()
{
	XMMATRIX world = getTransform();
	XMFLOAT4X4 wt;
	XMStoreFloat4x4(&wt, world);

	return XMVectorSet(wt._31, wt._32, wt._33, 0);
}

void Engine::Light::setPosition(XMVECTOR pos)
{
	XMStoreFloat3(&mPosition, pos);
}

XMVECTOR Engine::Light::getPosition()
{
	return XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 1.0f);
}

void Engine::Light::setRotation(XMVECTOR rot)
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

XMVECTOR Engine::Light::getRotation()
{
	return XMVectorSet(mRotation.x, mRotation.y, mRotation.z, 1.0f);
}

void Engine::Light::lootAt(XMVECTOR at)
{
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	up = XMVector3TransformCoord(up, XMLoadFloat3x3(&mOrientation));

	//TODO: Update mRotation

	XMMATRIX viewMatrix = XMMatrixLookAtRH(getPosition(), at, up);

	XMVECTOR det = XMMatrixDeterminant(viewMatrix);						//new orientation is the INVERSE of the view matrix
	viewMatrix = XMMatrixInverse(&det, viewMatrix);

	XMStoreFloat3x3(&mOrientation, viewMatrix);
}

XMMATRIX Engine::Light::getViewMatrix()
{
	return getTransform();
}

void Engine::Light::setLightFrustum(std::vector<XMVECTOR> points, XMMATRIX * projMatrix)
{
	XMFLOAT3 frustumCenter;
	XMVECTOR tempVecSum = XMVectorSet(0,0,0,0);

	XMFLOAT3 vMin, vMax, vec;
	XMMATRIX view = getViewMatrix();

	
	for (unsigned int i = 1; i<points.size(); i++)
	{
		XMStoreFloat3(&vec, XMVector3Transform(points.at(i), view));

		//compute min and max per component
		vMin = XMFLOAT3(fminf(vMin.x, vec.x), fminf(vMin.y, vec.y), fminf(vMin.z, vec.z));
		vMax = XMFLOAT3(fmaxf(vMax.x, vec.x), fmaxf(vMax.y, vec.y), fmaxf(vMax.z, vec.z));

		tempVecSum += points.at(i);
	}
	XMStoreFloat3(&frustumCenter, tempVecSum * (1 / 8)); //the avarage of all points is the center

	mFar = mNear + (vMax.z - vMin.z); //compute the far plane

	setPosition(XMVectorSet((vMax.x + vMin.x) / 2.0f,			//set shadow camera position using heuristic
		(vMax.y + vMin.y) / 2.0f, vMin.z - mNear, 1.0f));

	width = vMax.x - vMin.x;

	aspectRatio = width / (vMax.y - vMin.y);

	projMatrix = &XMMatrixOrthographicRH(width, width / aspectRatio, mNear, mFar);

}

