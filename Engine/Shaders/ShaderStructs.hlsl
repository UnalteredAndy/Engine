/////////////////////////////////////////////////////
// ShaderStructs.hlsl 
//
// Defines the light and material structs
///////////////////////////////////////////////////// 

struct PointLight
{
    float3 pos;
    float attenuation;
    float4 color;
    float intensity;
};

struct DirectionalLight
{
    float3 dir;
    float intensity;
    float4 color;
};

//TODO:: add spot light here later

struct Material //this is just the default material. More complex structs need to be defined in the shader itself.
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};