#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
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
	float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
	OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
    
    return OUT;
}

struct PSOut
{
	float4 color : SV_Target0;
	float4  mask  : SV_Target1;
};

PSOut PSMain(VSOut IN)
{
	PSOut result;
	
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
	
    float luma = dot(texColor.rgb, float3(0.3, 0.59, 0.11));
	float mask = saturate(luma * 4 - 3) - 1.0f;
	float3 glow = texColor.rgb * mask;

#ifdef LIGHTMAPPEDPOLY
	texColor *= lightmapTexture.Sample(Sampler, IN.lightmapCoord);
#endif

    float4 mainColor = texColor * model.color;;
	result.color = mainColor;
	result.mask = float4(glow, 1.0f);
    
    return result;
}