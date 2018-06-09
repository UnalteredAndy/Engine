#pragma once
#include<DirectXMath.h>
#include<vector>

namespace Engine {
	class SceneNode
	{
	public:

		enum TransformSpace{TS_WORLD, TS_PARENT, TS_LOCAL};

		SceneNode();
		~SceneNode();

		void setPosition(XMVECTOR pos);
		XMVECTOR getPosition();

		void setRotation(XMVECTOR rot);
		XMMATRIX getRotationMatrix();

		void setScalingVector(XMVECTOR scale);
		XMVECTOR getScalingVector();
		XMMATRIX getScalingMatrix();

		XMMATRIX getTransform();

		XMVECTOR transformToParentSpace(XMVECTOR vec, TransformSpace ts_of_value);//transforms vector from a certain space to parent space. needed to apply orientation or translation changes

		void DrawNode();

	private:
		XMFLOAT3 mPosition;
		XMFLOAT3X3 mOrientation;
		XMFLOAT3 mScaling;
	};
}


