/////////////////////////////////////////////////////////////////
// 
// vertex shader for shadow map creation
//
////////////////////////////////////////////////////////////////

//GLOBALS
cbuffer MatrixBufferType : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 worldViewProjMatrix;
    float4x4 lightViewProjMatrix;
};


//TYPES
struct VertexInputType
{
    float4 pos : POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT; // or binormal if you want
    float2 uv : TEXCOORD;
};


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return mul(pos, mul(worldMatrix, lightViewProjMatrix));
}