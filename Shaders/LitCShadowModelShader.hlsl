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
    float shininess;
};

cbuffer MaterialPropCB : register(b2)
{
    MaterialProp matProp;
};

struct Ambient
{
    float3 color;
    float intensity;
};

struct Directional
{
    float3 dir;
    float3 color;
    float intensity;
};

struct Point
{
    float3 pos;
    float3 color;
    float radius;
    float max_intensity;
    float falloff;
};

struct Lights
{
    Ambient amb;
    Directional dir;
    Point pont;
};

cbuffer LightsCB : register(b3)
{
    Lights lights;
};

struct CascadeData
{
    float4x4 viewProj[4];
    float4 distances;
};

cbuffer CascBuf : register(b4)
{
    CascadeData cascade_data;
}

vs_out VSMain(vs_in input)
{
    vs_out output = (vs_out) 0; // zero the memory first
    output.worldPos = mul(float4(input.pos, 1.), modelMat.model);
    output.pos = mul(float4(output.worldPos, 1.), view.viewProj);
    output.normal = normalize(mul(float4(input.normal, 0), modelMat.normModel).xyz);
    output.texCoord = input.texCoord;
    return output;
}

Texture2D diff : register(t0);
SamplerState sampl : register(s0);

Texture2DArray shadowmapT : register(t1);
SamplerComparisonState shadowmapS : register(s1);
//SamplerState shadowmapS : register(s1);

// add light color
float3 CalcDirLight(float3 surf_col, float3 dir, float3 normal, float3 viewDir)
{
    float3 lightDir = normalize(-dir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matProp.shininess);

    return lights.dir.intensity * lights.dir.color * (diff + spec) * surf_col;
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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matProp.shininess);
    // attenuation
    float distance = length(lightPos - fragPos);
    // combine results
    return attenuate_cusp(distance, lights.pont.radius, lights.pont.max_intensity, lights.pont.falloff) * lights.pont.color * (diff + spec) * surf_col;
}

float SampleCascade(uint cascadeIndx, float4 pixLightPos)
{
	 // perform perspective divide
    float3 projCoords = pixLightPos.xyz / pixLightPos.w;
    // transform to [0,1] range
    projCoords.x = projCoords.x * 0.5 + 0.5;
    projCoords.y = -projCoords.y * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float3 texCoord;
    texCoord.xy = projCoords.xy;
    texCoord.z = cascadeIndx;
    float sampled = shadowmapT.SampleCmp(shadowmapS, texCoord, projCoords.z);
    //float sampled = shadowmapT.Sample(shadowmapS, projCoords.xy);
    return sampled;
}

float4 PSMain(vs_out input) : SV_TARGET
{
    float4 surf_col = matProp.base_color * diff.Sample(sampl, input.texCoord);

    float3 viewDir = normalize(view.world_pos - input.worldPos);

    uint layer = 3;
    float depthVal = abs((view.world_pos - input.worldPos).z);
    for (int i = 0; i < 4; ++i)
    {
        if (depthVal < cascade_data.distances[i])
        {
            layer = i;
            break;
        }
    }
    float4 lightPos = mul(float4(input.worldPos, 1.), cascade_data.viewProj[layer]);
    float SMs = SampleCascade(layer, lightPos);

    float3 lit =
    lights.amb.intensity * lights.amb.color * surf_col +
    CalcDirLight(surf_col, lights.dir.dir, input.normal, viewDir) * SMs +
    CalcPointLight(surf_col, lights.pont.pos, input.normal, input.worldPos, viewDir);

    return float4(lit, 1); // must return an RGBA colour
}