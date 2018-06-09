#pragma once
#include <DirectXMath.h>
#include "Shader.h"

namespace Engine {

	using namespace DirectX;

	class Material
	{
	public:
		Material(); //use this if you want to make a non uniform material (with maps) and then use the specific setter methods for setting the map
		Material(XMFLOAT4 Ka, XMFLOAT4 Kd, XMFLOAT4 Ks, XMFLOAT4 Ns); // use this for uniform value materials
		~Material();

		void setShader(Shader* shader) { this->shader = shader; }; // this is purposely not flyweight. More memore intensive, but faster

		Shader * getShader() { return shader; };

		XMFLOAT4 Ka, Kd, Ks, Ns;
		//some texture here

	private:

		Shader * shader;
	};
}