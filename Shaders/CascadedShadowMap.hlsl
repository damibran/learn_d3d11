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
    float4 pos : POSITION;
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

struct CascadeData
{
    float4x4 viewProj[4];
    float4 distances;
};

cbuffer CascBuf : register(b0)
{
    CascadeData cascade_data;
}

struct GS_OUT
{
    float4 pos : SV_POSITION;
    nointerpolation uint arrInd : SV_RenderTargetArrayIndex;
};


vs_out VSMain(vs_in input)
{
    vs_out output = (vs_out) 0; // zero the memory first
    output.pos = mul(float4(input.pos, 1.), modelMat.model);
    return output;
}

[instance(4)]
[maxvertexcount(3)]
void GSMain(
	triangle vs_out p[3],
	in uint id : SV_GSInstanceID,
	inout TriangleStream<GS_OUT> stream)
{
	[unroll]
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT gs = (GS_OUT) 0;
        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), cascade_data.viewProj[id]);
        gs.arrInd = id;
        stream.Append(gs);
    }
}

float4 PSMain(GS_OUT input) : SV_TARGET
{
    return (0, 0, 0, 1);
}