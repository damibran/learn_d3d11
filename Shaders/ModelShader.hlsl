/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
    float3 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct ViewMat
{
    matrix view;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    ViewMat viewMat;
};

struct ModelMat
{
    matrix model;
    matrix normModel;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    ModelMat modelMat;
};

vs_out VSMain(vs_in input)
{
    vs_out output = (vs_out) 0; // zero the memory first
    output.pos = mul(mul(float4(input.pos, 1.), modelMat.model), viewMat.view);
    output.normal = mul(input.normal, modelMat.normModel);
    output.texCoord = input.texCoord;
    return output;
}

Texture2D diff : register(t0);
SamplerState sampl : register(s0);

float4 PSMain(vs_out input) : SV_TARGET
{
    return diff.Sample(sampl, input.texCoord); // must return an RGBA colour
}