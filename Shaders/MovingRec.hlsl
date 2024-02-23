/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
    float4 position_local : SV_POSITION;
    float4 color : COLOR;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
    float4 position_clip : SV_POSITION; // required output of VS
    float4 color : COLOR;
};

struct ModelMat
{
    matrix model;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    ModelMat modelMat;
};

vs_out VSMain(vs_in input)
{
    vs_out output = (vs_out) 0; // zero the memory first
    output.position_clip = mul(input.position_local,modelMat.model);
    output.color = input.color;
    return output;
}

float4 PSMain(vs_out input) : SV_TARGET
{
    return input.color; // must return an RGBA colour
}