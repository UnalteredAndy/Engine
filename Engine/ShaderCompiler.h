#pragma once
#define _WIN32_WINNT 0x600
#include <stdio.h>
#include"Window.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

class ShaderCompiler
{
public:
	//ShaderCompiler();
	//~ShaderCompiler();

	//compiles hlsl shaders from the disc. 
	static HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
};

