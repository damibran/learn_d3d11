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
};

struct ViewData
{
    matrix viewProj;
};

cbuffer ViewDataCB : register(b0)
{
    ViewData view;
};

struct ModelMat
{
    matrix model;
    matrix normModel;
};

cbuffer ModelMatCB : register(b1)
{
    ModelMat modelMat;
};

vs_out VSMain(vs_in input)
{
    vs_out output = (vs_out) 0; // zero the memory first
    output.pos = mul(mul(float4(input.pos, 1.), modelMat.model), view.viewProj);
    return output;
}


float4 PSMain(vs_out input) : SV_TARGET
{
    return (0, 0, 0, 1);
}