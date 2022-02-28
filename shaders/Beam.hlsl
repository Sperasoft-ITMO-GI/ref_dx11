#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
};

struct VSIn
{
    float3 pos : Position;
};

VSOut VSMain(VSIn IN)
{
	float4x4 proj = mul(camera.perspective, camera.view);
    VSOut OUT;
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = model.color;
    return result;
}

