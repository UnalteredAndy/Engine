/////////////////////////////
// colorPS.hlsl 
//
// pixel shader that outputs the interpolated vertex color for that pixel
/////////////////////////////

#include "ShaderStructs.hlsl"

//////GLOBALS
cbuffer PSLightBuffer: register(b1)
{
    DirectionalLight dirLight;
};

//////INPUT TYPE
struct PixelInputType
{
    float4 posH : SV_Position;
    float3 posW : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 color : COLOR;
    float4 shadowCoord : SHADOWPOS;
    float2 texCoord : TEXCOORD;
};

Texture2D diffuseMap : register(t0);
Texture2D NormalsMap : register(t1);
Texture2D shadowMap : register(t2);

sampler m_sampler;

float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
    //normals
    float3 normal;

    float3 bump = 2 * (NormalsMap.Sample(m_sampler, input.texCoord) - (0.5, 0.5, 0.5));
    normal = input.normal + (bump.x * input.tangent + bump.y * input.bitangent);
    normal = normalize(normal);

    //we use bumpMap normal instead
    //normal = normalize(input.normal);

    //shadows
    input.shadowCoord.xyz /= input.shadowCoord.w;

    if (input.shadowCoord.x < -1.0f || input.shadowCoord.x > 1.0f ||
        input.shadowCoord.y < -1.0f || input.shadowCoord.y > 1.0f ||
        input.shadowCoord.z <  0.0f || input.shadowCoord.z > 1.0f)
        return 0;
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    input.shadowCoord.x = input.shadowCoord.x / 2 + 0.5;
    input.shadowCoord.y = input.shadowCoord.y / -2 + 0.5;

    //sample shadow map - point sampler
    float shadowMapDepth = shadowMap.Sample(m_sampler, input.shadowCoord.xy).z;
 
    //if clip space z value greater than shadow map value then pixel is in shadow
    if (shadowMapDepth < input.shadowCoord.z) return 0;
 
    //shadows end


    //float3 lightDirection = dirLight.dir;
    float n = max(dot(dirLight.dir, normal), 0.0f);

    [flatten]
    if(n > 0.0f)
    {
      
        float4 outputColor =dirLight.color * diffuseMap.Sample(m_sampler, input.texCoord) * dirLight.intensity * n;
     
        return outputColor;
    }
    else
    {
        //float4 color = { 0.15f, 0.34f, 0.51f, 0.0f };
       // return color; //debug color
        return (0.0);
    }
    
    
};