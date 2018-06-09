//////////////////////////////
// colorVS.hlsl  
//
// vertex shader that outputs the color of the vertex
//////////////////////////////


////GLOBALS
cbuffer MatrixBufferType :register(b0)
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

//vertex shader
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;


   //TODO:: take care of premultiplying worldViewProj matrix on CPU
   
    // Transform to homogeneous clip space. This is for the Position in the viewport. Only used by rasterizer
    output.posH = mul(input.pos, worldViewProjMatrix);

    //Vertex World position
    output.posW = mul(input.pos, worldMatrix).xyz;
    
    output.color = input.color;
    output.texCoord = input.uv;

    output.normal = mul(input.normal, worldMatrix).xyz;
    output.tangent = mul(input.tangent, worldMatrix).xyz;
    output.bitangent = mul(input.bitangent, worldMatrix).xyz;

    output.shadowCoord = mul(input.pos, mul(worldMatrix, lightViewProjMatrix));
    return output;
};