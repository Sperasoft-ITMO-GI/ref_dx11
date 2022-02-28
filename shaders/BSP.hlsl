#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
    float2 lightmapCoord : LMTEXCOORD;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
	float4x4 proj = mul(camera.perspective, camera.view);
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
	
	float luma = dot(texColor.rgb, float3(0.3f, 0.5f, 0.2f));
	float mask = saturate(luma * 4 - 3);
	float3 glow = texColor.rgb * mask;

#ifdef LIGHTMAPPEDPOLY
	texColor *= lightmapTexture.Sample(Sampler, IN.lightmapCoord);
#endif
	
	float4 result = texColor * model.color + float4(glow, 0.0f);

    return result;
}