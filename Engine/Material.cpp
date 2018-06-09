#include "Material.h"

using namespace Engine;
using namespace DirectX;

Material::Material(): 
	Ka(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
	Kd(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
	Ks(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
	Ns(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

Material::Material(XMFLOAT4 Ka, XMFLOAT4 Kd, XMFLOAT4 Ks, XMFLOAT4 Ns) : 
	Ka(Ka),
	Kd(Kd),
	Ks(Ks),
	Ns(Ns)
{
}


Material::~Material()
{
}
