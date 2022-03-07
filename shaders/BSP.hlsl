#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
    float2 lightmapCoord : LMTEXCOORD;
    float2 tex0 : TEXCOORD1;
    float2 tex1 : TEXCOORD2;
    float2 tex2 : TEXCOORD3;
    float2 tex3 : TEXCOORD4;
    float2 tex4 : TEXCOORD5;
    float2 tex5 : TEXCOORD6;
    float2 tex6 : TEXCOORD7;
    float2 tex7 : TEXCOORD8;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

static const float c0 = 1.0f;
static const float c1 = 0.875f;
static const float c2 = 0.75f;
static const float c3 = 0.625f;
static const float c4 = 0.5;
static const float c5 = 0.375f;
static const float c6 = 0.25f;
static const float c7 = 0.125f;

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
	float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
	OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
    
    OUT.tex0 = IN.texCoord + c0;
    OUT.tex1 = IN.texCoord + c1;
    OUT.tex2 = IN.texCoord + c2;
    OUT.tex3 = IN.texCoord + c3;
    OUT.tex4 = IN.texCoord + c4;
    OUT.tex5 = IN.texCoord + c5;
    OUT.tex6 = IN.texCoord + c6;
    OUT.tex7 = IN.texCoord + c7;
    
    return OUT;
}

struct PSOut
{
	float4 color : SV_Target0;
	float4 mask : SV_Target1;
};

PSOut PSMain(VSOut IN)
{
	PSOut result;
	
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
	
	float luma = dot(texColor.rgb, float3(0.3f, 0.5f, 0.2f));
	float mask = saturate(luma * 4 - 3);
	float3 glow = texColor.rgb * mask;

#ifdef LIGHTMAPPEDPOLY
	texColor *= lightmapTexture.Sample(Sampler, IN.lightmapCoord);
#endif

	//float4 r0 = colorTexture.Sample(Sampler, IN.tex0);
    //float4 r1 = colorTexture.Sample(Sampler, IN.tex1);
    //r0 *= c0;
    //r0 += r1 * c1;
    //r1 = colorTexture.Sample(Sampler, IN.tex2);
    //float4 r2 = colorTexture.Sample(Sampler, IN.tex3);
    //r0 += r1 * c2;
    //r0 += r2 * c3;
    //r1 = colorTexture.Sample(Sampler, IN.tex4);
    //r2 = colorTexture.Sample(Sampler, IN.tex5);
    //r0 += r1 * c4;
    //r0 += r2 * c5;
    //r1 = colorTexture.Sample(Sampler, IN.tex6);
    //r2 = colorTexture.Sample(Sampler, IN.tex7);
    //r0 += r1 * c6;
    //r0 += r2 * c7;
    
    //return result * r0;

    float4 mainColor = texColor * model.color;// + float4(glow, 0.0f);
	result.color = mainColor;
	result.mask = float4(glow, 0.0f);
    
    return result;
}