#define HLSL

#include "shader_defines.h"

#ifdef BLOOM
static const float PI = 3.14159265f;
static const float sigma = 0.2f;
static const float mu = 0;
static const float c0 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((0 - mu), 2) / pow(sigma, 2)));
static const float c1 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((1 - mu), 2) / pow(sigma, 2)));
static const float c2 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((2 - mu), 2) / pow(sigma, 2)));
static const float c3 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((3 - mu), 2) / pow(sigma, 2)));
static const float c4 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((4 - mu), 2) / pow(sigma, 2)));
static const float c5 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((5 - mu), 2) / pow(sigma, 2)));
static const float c6 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((6 - mu), 2) / pow(sigma, 2)));
static const float c7 = 1 / (sigma * sqrt(2 * PI)) * exp(-0.5 * (pow((7 - mu), 2) / pow(sigma, 2)));
#endif

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
#ifdef BLOOM
    float2 tex0 : TEXCOORD1;
    float2 tex1 : TEXCOORD2;
    float2 tex2 : TEXCOORD3;
    float2 tex3 : TEXCOORD4;
    float2 tex4 : TEXCOORD5;
    float2 tex5 : TEXCOORD6;
    float2 tex6 : TEXCOORD7;
    float2 tex7 : TEXCOORD8;
#endif
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

#ifdef BLOOM
    OUT.tex0 = IN.texCoord + c0;
    OUT.tex1 = IN.texCoord + c1;
    OUT.tex2 = IN.texCoord + c2;
    OUT.tex3 = IN.texCoord + c3;
    OUT.tex4 = IN.texCoord + c4;
    OUT.tex5 = IN.texCoord + c5;
    OUT.tex6 = IN.texCoord + c6;
    OUT.tex7 = IN.texCoord + c7;
#endif
    
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;
    
#ifdef DEFAULT
    result = model.color;
#endif
    
#ifdef BLOOM
    float radius = 1.0f;
    float4 r0 = sceneTexture.Sample(Sampler, IN.tex0) * radius;
    float4 r1 = sceneTexture.Sample(Sampler, IN.tex1) * radius;
    r0 *= c0;
    r0 += r1 * c1;
    r1 = sceneTexture.Sample(Sampler, IN.tex2) * radius;
    float4 r2 = sceneTexture.Sample(Sampler, IN.tex3) * radius;
    r0 += r1 * c2;
    r0 += r2 * c3;
    r1 = sceneTexture.Sample(Sampler, IN.tex4) * radius;
    r2 = sceneTexture.Sample(Sampler, IN.tex5) * radius;
    r0 += r1 * c4;
    r0 += r2 * c5;
    r1 = sceneTexture.Sample(Sampler, IN.tex6) * radius;
    r2 = sceneTexture.Sample(Sampler, IN.tex7) * radius;
    r0 += r1 * c6;
    r0 += r2 * c7;
    float intensity = 1.0f;
    result = r0 * intensity;
    return result;
#endif
    
#ifdef SCENE
    result = bloomTexture.Sample(Sampler, IN.texCoord)
           + effectTexture.Sample(Sampler, IN.texCoord);
#endif
    
    return result;
}


