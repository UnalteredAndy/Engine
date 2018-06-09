#include "Camera.h"

using namespace Engine;
using namespace DirectX;

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane):
	mFov(fov),
	mAspect(aspect),
	mNear(nearPlane),
	mFar(farPlane),
	mPosition(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
	mRotation(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f))
{
}

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane, XMVECTOR& pos, XMVECTOR& rot) :
	mFov(fov),
	mAspect(aspect),
	mNear(nearPlane),
	mFar(farPlane),
	mPosition(pos),
	mRotation(rot)
{
}


Camera::~Camera()
{
}

XMMATRIX Engine::Camera::getOrientation()
{
	return XMMATRIX();
}

void Engine::Camera::setPosition(XMVECTOR pos)
{
	mPosition = pos;
}

XMVECTOR Engine::Camera::getPosition()
{
	return mPosition;
}

void Engine::Camera::setRotation(XMVECTOR rot)
{
	mRotation = rot;
}

XMVECTOR Engine::Camera::getRotation()
{
	return mRotation;
}

XMMATRIX Engine::Camera::getProjectionMatrix()
{
	return XMMatrixPerspectiveFovRH(mFov, mAspect, mNear, mFar);
}

XMMATRIX Engine::Camera::getViewMatrix()
{

	XMVECTOR up, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	XMFLOAT3 tempRot;
	XMStoreFloat3(&tempRot, mRotation);

	// Setup the vector that points upwards.
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup where the camera is looking by default.
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = tempRot.x * 0.0174532925f;
	yaw = tempRot.y * 0.0174532925f;
	roll = tempRot.z * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = mPosition + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtRH(mPosition, lookAt, up);
	
	//m_viewMatrix = XMMatrixTranslationFromVector(getPosition());

	return m_viewMatrix;

	//this only works in this flat hiarchy. If we used a node tree based system then this calculation is the parent space transform and we would need to do this recursively up to the root to get the world transform.
	//XMMATRIX world = getScaling() * getOrientation() * XMMatrixTranslationFromVector(getPosition());
	

	//XMVECTOR det = XMMatrixDeterminant(world);
	//return XMMatrixInverse(&det, world);

	//return XMMatrixIdentity(); //just have the camera be at the scene origin for now
}

std::vector<XMVECTOR> Engine::Camera::getFrustumPoints()
{
	
	//the base cuboid for the frustum
	std::vector<XMVECTOR> frustumPoints;/* = {
		XMVectorSet(-1.0f, 1.0f, 0.0f, 1.0f),
		XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f),
		XMVectorSet( 1.0f,-1.0f, 0.0f, 1.0f),
		XMVectorSet(-1.0f,-1.0f, 0.0f, 1.0f),
		XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f),
		XMVectorSet( 1.0f, 1.0f, 1.0f, 1.0f),
		XMVectorSet( 1.0f,-1.0f, 1.0f, 1.0f),
		XMVectorSet(-1.0f,-1.0f, 1.0f, 1.0f)
	};

	
	XMMATRIX invViewProjMatrix = XMMatrixInverse(nullptr, XMMatrixTranspose(getProjectionMatrix() * getViewMatrix()));
	
	//transform the frustum points to their correct location
	for (XMVECTOR vec : frustumPoints)
	{
		vec = XMVector3Transform(vec, invViewProjMatrix) ;
	}*/

	float at = atanf(mFov / 2.0f);
	XMMATRIX world = getViewMatrix();

	float fFar = mFar; //min( mFar, 1000.0f );
	float fNear = mNear;

	XMFLOAT3 point;

	frustumPoints.push_back(XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), world));


	frustumPoints.push_back(XMVector4Transform(XMVectorSet(-mAspect *  fFar * at, +fFar * at, fFar, 1.0f), world));

	frustumPoints.push_back(XMVector4Transform(XMVectorSet(+mAspect *  fFar * at, +fFar * at, fFar, 1.0f), world));
	
	frustumPoints.push_back(XMVector4Transform(XMVectorSet(-mAspect *  fFar * at, -fFar * at, fFar, 1.0f), world));
	
	frustumPoints.push_back(XMVector4Transform(XMVectorSet(+mAspect *  fFar * at, -fFar * at, fFar, 1.0f), world));

	return frustumPoints;
}

void Engine::Camera::setAspectRatio(float newAspectRatio)
{
	/*std::wostringstream outs;
	outs.precision(6);
	outs << L"Setting new Aspect Ratio!: "<< newAspectRatio;
	MessageBox(Window::getInstance()->getMainWindow(), outs.str().c_str(), L"warning", 0);*/
	mAspect = newAspectRatio;
}
