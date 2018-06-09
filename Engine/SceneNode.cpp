#include "SceneNode.h"

using namespace Engine;
using namespace DirectX;

SceneNode::SceneNode()
{
}


SceneNode::~SceneNode()
{
}

void SceneNode::setPosition(XMVECTOR pos)
{
	XMStoreFloat3(&mPosition, pos);
}

XMVECTOR SceneNode::getPosition()
{
	return XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 1.0f);
}

void SceneNode::setRotation(XMVECTOR rot)
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

XMMATRIX SceneNode::getRotationMatrix()
{

	//return  XMLoadFloat4x4(&wm);
	return XMLoadFloat3x3(&mOrientation);
}

void SceneNode::setScalingVector(XMVECTOR scale)
{
	XMStoreFloat3(&mScaling, scale);
}

XMVECTOR SceneNode::getScalingVector()
{
	return XMVectorSet(mScaling.x, mScaling.y, mScaling.z, 1.0f);
}

XMMATRIX SceneNode::getScalingMatrix()
{
	return XMMatrixScaling(mScaling.x, mScaling.y, mScaling.z);
}

XMMATRIX SceneNode::getTransform()
{
	return  getRotationMatrix() * getScalingMatrix() * XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
}

XMVECTOR SceneNode::transformToParentSpace(XMVECTOR vec, TransformSpace ts_of_value)
{
	return XMVECTOR();
}

