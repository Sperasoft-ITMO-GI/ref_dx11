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
    float4 pos : SV_Position;
    float4 prevPos : POSITION0;
    float4 newPos  : POSITION1;
    float2 texCoord : TEXCOORD0;
    float2 lightmapCoord : LMTEXCOORD;
    float3 norm : NORMAL;
    float2 worldPos : TEXCOORD1;
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
	OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
    
    OUT.norm = mul(camera.perspective, float4(IN.norm.xyz, 0));
    OUT.worldPos = mul(camera.orthogonal, float4(IN.pos, 1));
    
    return OUT;
}

struct PSOut
{
	float4 color       : SV_Target0;
	float4 mask        : SV_Target1;
    float2 velosity    : SV_Target2;
    float4 lightmap    : SV_Target3;
    float4 albedo      : SV_Target4;
    float2 positions   : SV_Target5;
    float4 normals     : SV_Target6;
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
    
    result.color = (texColor * model.color);
    result.positions = float2(frac(IN.worldPos.xy / 64));

    result.normals = float4(IN.norm, 0);
    
    result.color += saturate(dot(light.direction, IN.norm) * light.color) * light.intensity;
    result.mask = float4(glow, 1.0f);
    
    float2 a = (IN.newPos.xy / IN.newPos.w) * 0.5 + 0.5;
    float2 b = (IN.prevPos.xy / IN.prevPos.w) * 0.5 + 0.5;
    result.velosity = a - b;
    
    return result;
}