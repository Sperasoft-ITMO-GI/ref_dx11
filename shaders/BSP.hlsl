#define HLSL

#include "shader_defines.h"

#define IDENTITY_MATRIX float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

float2 CreateHaltonNumber(unsigned int index, int2 base)
{
    float f = 1;
    float2 r = 0;
    int current = index;
    do
    {
        f = f / base.x;
        r.x = r.x + f * (current % base.x);
        current = floor(current / base.x);
    } while (current);
    
    current = index;
    do
    {
        f = f / base.y;
        r.y = r.y + f * (current % base.y);
        current = floor(current / base.y);
    } while (current);
    
    return r;
}

struct VSOut
{
#ifndef TEX
    float4 pos : SV_Position;
    float4 prevPos : POSITION0;
    float4 newPos : POSITION1;
    float2 lightmapCoord : LMTEXCOORD;
#else
    float4 pos : SV_Position;
#endif
    float3 norm : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct VSIn
{
    float3 pos : Position;
    float3 norm : Normal;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    
#ifdef TEX
    OUT.pos = mul(float4(IN.texCoord, 0, 1),buffer.orthogonal);
    //OUT.pos.x += 0.1f;
    //OUT.pos.y -= 0.3f;
    OUT.norm = IN.norm;//mul(float4(IN.norm.xyz, 0), buffer.orthogonal);
#else
    float deltaWidth = 1.0f / camera.resolution.x;
    float deltaHeight = 1.0f / camera.resolution.y;
    uint numSamples = 10;
    uint index = camera.total_frames % numSamples;
    float2 halton = CreateHaltonNumber(index + 1, float2(2, 3));
    float2 jitter = float2(halton.x * deltaWidth, halton.y * deltaHeight);
    
    float haltonScale = 1.0f;
    matrix jitterMat = IDENTITY_MATRIX;
    jitterMat[3][0] += jitter.x * haltonScale;
    jitterMat[3][1] += jitter.y * haltonScale;
    
    float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
    float4x4 prevProj = mul(mul(camera.perspective, camera.prev_view), model.mod);
    OUT.pos = mul(jitterMat, mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f)));
    OUT.prevPos = mul(prevProj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.newPos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.lightmapCoord = IN.lightmapCoord;
    OUT.norm = mul(camera.perspective, float4(IN.norm.xyz, 0));
#endif
  
    OUT.texCoord = IN.texCoord;
    
    return OUT;
}

struct PSOut
{
#ifndef TEX
	float4 color       : SV_Target0;
	float4 mask        : SV_Target1;
    float2 velosity    : SV_Target2;
    float4 lightmap    : SV_Target3;
    float4 albedo      : SV_Target4;
#else
    float4 positions   : SV_Target2;
    float4 normals     : SV_Target1;
    float4 albedo      : SV_Target0;
#endif
};

//static const float4 lightColor = float4(0.96862745098, 0.36862745098, 0.14509803921, 0.0f);
//static const float3 lightDir =   float3(1, 0.5, 0.45);

PSOut PSMain(VSOut IN)
{
	PSOut result;
    
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
    float4 lightmap = 0;
	
    float luma = dot(texColor.rgb, float3(0.3, 0.59, 0.11));
	float mask = saturate(luma * 4 - 3);
	float3 glow = texColor.rgb * mask;
    
    result.albedo = texColor * model.color;
    
#ifdef LIGHTMAPPEDPOLY
    lightmap = lightmapTexture.Sample(Sampler, IN.lightmapCoord);
    result.lightmap = lightmap;
    texColor *= lightmap;
#endif

#ifndef TEX
    
    #ifdef LMTEX
        result.color = lightmapTexture.Sample(Sampler, IN.lightmapCoord);
    #else
    result.color = texColor * model.color;
    result.color += saturate(dot(light.direction, IN.norm) * light.color) * light.intensity;
    result.mask = float4(glow, 1.0f);
    
    float2 a = (IN.newPos.xy / IN.newPos.w) * 0.5 + 0.5;
    float2 b = (IN.prevPos.xy / IN.prevPos.w) * 0.5 + 0.5;
    result.velosity = a - b;
    #endif
#else
    result.positions = float4(frac(IN.pos.xyz / 64), 1);
    result.normals = float4(IN.norm, 1);
#endif 
    
    return result;
}