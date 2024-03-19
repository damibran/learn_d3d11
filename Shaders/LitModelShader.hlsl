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
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct ViewData
{
    matrix viewProj;
    float3 world_pos;
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

struct MaterialProp
{
    float4 base_color;
};

cbuffer MaterialPropCB : register(b2)
{
    MaterialProp matProp;
};

struct Lights
{
    float3 dir;
    float3 point_pos;
};

cbuffer LightsCB : register(b3)
{
    Lights lights;
};

vs_out VSMain(vs_in input)
{
    vs_out output = (vs_out) 0; // zero the memory first
    output.worldPos = mul(float4(input.pos, 1.), modelMat.model);
    output.pos = mul(float4(output.worldPos, 1.), view.viewProj);
    output.normal = mul(input.normal, modelMat.normModel);
    output.texCoord = input.texCoord;
    return output;
}

Texture2D diff : register(t0);
SamplerState sampl : register(s0);

// add to material
//const float shininess = 32;

// add light color
float3 CalcDirLight(float3 surf_col, float3 dir, float3 normal, float3 viewDir)
{
    float3 lightDir = normalize(-dir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    return 0.0 * (diff + spec) * surf_col;
}

//https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
float attenuate_cusp(float distance, float radius, float max_intensity, float falloff)
{
    float s = distance / radius;

    if (s >= 1.0)
        return 0.0;

    float s2 = sqrt(s);

    return max_intensity * sqrt(1 - s2) / (1 + falloff * s);
}

float3 CalcPointLight(float3 surf_col, float3 lightPos, float3 normal, float3 fragPos, float3 viewDir)
{
    float3 lightDir = normalize(lightPos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance = length(lightPos - fragPos);
    // combine results
    return attenuate_cusp(distance, 20, 3, 1) * (diff + spec) * surf_col;
}

float4 PSMain(vs_out input) : SV_TARGET
{
    const float3 ambient = 0.1 * float3(1, 1, 1);

    float4 surf_col = matProp.base_color * diff.Sample(sampl, input.texCoord);

    float3 viewDir = normalize(view.world_pos - input.worldPos);

    float3 lit =
    ambient +
    CalcDirLight(surf_col, lights.dir, input.normal, viewDir) +
    CalcPointLight(surf_col, lights.point_pos, input.normal, input.worldPos, viewDir);

    return float4(lit, 1); // must return an RGBA colour
}