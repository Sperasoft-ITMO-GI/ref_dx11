#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
    float2 lightmapCoord : LMTEXCOORD;
	float4 col : Color;
};

struct VSIn
{
    float3 pos : Position;
	float4 col : Color_In;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

VSOut VSMain(VSIn IN)
{
	#ifdef WEAPON
	float4x4 proj = mul(mul(camera.weaponProj, camera.view), model.mod);
	#else
	float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
	#endif
	
	//float4x4 proj = mul(mul(model.mod, camera.view), camera.perspective);
	
    VSOut OUT;
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
	OUT.col = IN.col;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = (colorTexture.Sample(Sampler, IN.texCoord) * IN.col) * model.color;

    return result;
}