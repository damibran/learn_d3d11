/* vertex attributes go here to input to the vertex shader */
struct vs_in 
{
    float4 position_local : POS;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out 
{
    float4 position_clip : SV_POSITION; // required output of VS
};

vs_out VSMain(vs_in input) 
{
  vs_out output = (vs_out)0; // zero the memory first
  output.position_clip = input.position_local;
  return output;
}

float4 PSMain(vs_out input) : SV_TARGET {
  return float4( 1.0, 0.0, 1.0, 1.0 ); // must return an RGBA colour
}