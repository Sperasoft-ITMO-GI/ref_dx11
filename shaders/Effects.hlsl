#define HLSL

#include "shader_defines.h"

#ifdef BLOOM
static const float PI = 3.14159265f;
static const float sigma = 20.2f;
static const float mu = 0;

#define gauss_constant 0.3989422804014327

float gauss_weight(int sampleDist)
{
	float g = 1.0f / sqrt(2.0f * PI * sigma * sigma);
	return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

#endif

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
};

VSOut VSMain(VSIn IN)
{
	float4x4 proj = mul(transpose(camera.orthogonal), model.mod);
	
    VSOut OUT;
    OUT.pos = mul(proj, float4(IN.pos, 1.0f));
    OUT.texCoord = IN.texCoord;
    
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;
    
#ifdef DEFAULT
    result = model.color;
#endif

#ifdef BLOOM
    uint width, height;
    sceneTexture.GetDimensions(width, height);
    float2 texelSize = float2(1.0f/width, 1.0f/height);

    uint radius = 15;
    
    result = sceneTexture.Sample(Sampler, IN.texCoord) * gauss_weight(0);
    for (uint i = 1; i < radius; ++i)
    {
        result += sceneTexture.Sample(Sampler, IN.texCoord + float2(texelSize.x * i / width, 0.0f)) * gauss_weight(i);
        result += sceneTexture.Sample(Sampler, IN.texCoord - float2(texelSize.x * i / width, 0.0f)) * gauss_weight(i);
    }    
    
    for (i = 1; i < radius; ++i)
    {
        result += sceneTexture.Sample(Sampler, IN.texCoord + float2(0.0f, texelSize.y * i / height)) * gauss_weight(i);
        result += sceneTexture.Sample(Sampler, IN.texCoord - float2(0.0f, texelSize.y * i / height)) * gauss_weight(i);
    }
    

    float intensity = 1.0f;
    result *= intensity;
    return result;
#endif
    
#ifdef SCENE
    result = bloomTexture.Sample(Sampler, IN.texCoord)
           + effectTexture.Sample(Sampler, IN.texCoord);
#endif
    
    return result;
}


