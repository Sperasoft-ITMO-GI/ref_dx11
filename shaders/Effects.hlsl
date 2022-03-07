#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
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

#ifdef DEFAULT
float4 PSMain(VSOut IN) : SV_Target1
{
    float4 result;
    result = model.color;
	return result;
}
#endif
    
#ifdef SCENE
float4 PSMain(VSOut IN) : SV_Target0
{
    float4 result;
    result = colorTexture.Sample(Sampler, IN.texCoord);
	//result.w = 0.5f;
	return result;
}
#endif

