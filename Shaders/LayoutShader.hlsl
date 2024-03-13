/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
    float3 position_local : SV_POSITION;
    float3 color : NORMAL;
    float2 texCoord : TEXCOORD; 
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
    float4 position_clip : SV_POSITION; // required output of VS
    float4 color : NORMAL;
    float2 texCoord : TEXCOORD; 
};

vs_out VSMain(vs_in input)
{
}

float4 PSMain(vs_out input) : SV_TARGET
{
}