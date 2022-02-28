#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float3 texCoord : TEXCOORD;
};

struct VSIn
{
    float3 pos : Position;
};

VSOut VSMain(VSIn IN)
{
	float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
	
    VSOut OUT;
    OUT.texCoord = IN.pos;
    OUT.pos = mul(proj, float4(IN.pos, 0.0f));
    OUT.pos.z = OUT.pos.w;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    return skyboxTexture.Sample(Sampler, float3(IN.texCoord.x, IN.texCoord.z, IN.texCoord.y));
}

