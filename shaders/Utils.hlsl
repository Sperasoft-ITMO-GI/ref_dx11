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
    float2 texCoord : TEXCOORD;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
#ifdef QUAD
    OUT.pos = float4((IN.texCoord.x-0.5f)*2,-(IN.texCoord.y-0.5f)*2,0,1);
#else
	float4x4 proj = mul(camera.perspective, camera.view);
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
#endif
    OUT.texCoord = IN.texCoord;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = 0;
#ifdef QUAD
    if (IN.pos.x < 32.0f && IN.pos.y < 32.0f)
        result = float4(0.8f, 0.0f, 0.0f, 1.0f);
    else if (IN.pos.x < 64.0f && IN.pos.y < 32.0f)
        result = float4(0.0f, 0.8f, 0.0f, 1.0f);
    else if (IN.pos.x < 96.0f && IN.pos.y < 32.0f)
        result = float4(0.0f, 0.0f, 0.8f, 1.0f);
    else if (IN.pos.x < 128.0f && IN.pos.y < 32.0f)
        result = float4(0.8f, 0.8f, 0.8f, 1.0f);    
    else if (IN.pos.x < 32.0f && IN.pos.y < 64.0f)
        result = float4(0.8f, 0.8f, 0.0f, 1.0f);
    else if (IN.pos.x < 64.0f && IN.pos.y < 64.0f)
        result = float4(0.0f, 0.8f, 0.8f, 1.0f);
    else if (IN.pos.x < 96.0f && IN.pos.y < 64.0f)
        result = float4(0.8f, 0.0f, 0.8f, 1.0f);
    else if (IN.pos.x < 128.0f && IN.pos.y < 64.0f)
        result = float4(0.5f, 0.8f, 0.5f, 1.0f);    
    else if (IN.pos.x < 32.0f && IN.pos.y < 96.0f)
        result = float4(0.8f, 0.5f, 0.5f, 1.0f);
    else if (IN.pos.x < 64.0f && IN.pos.y < 96.0f)
        result = float4(0.5f, 0.5f, 0.8f, 1.0f);
    else if (IN.pos.x < 96.0f && IN.pos.y < 96.0f)
        result = float4(0.5f, 0.5f, 0.5f, 1.0f);
    else if (IN.pos.x < 128.0f && IN.pos.y < 96.0f)
        result = float4(0.0f, 0.0f, 0.0f, 1.0f);    
    else if (IN.pos.x < 32.0f && IN.pos.y < 128.0f)
        result = float4(0.2f, 0.6f, 0.2f, 1.0f);
    else if (IN.pos.x < 64.0f && IN.pos.y < 128.0f)
        result = float4(0.4f, 0.2f, 0.1f, 1.0f);
    else if (IN.pos.x < 96.0f && IN.pos.y < 128.0f)
        result = float4(0.3f, 0.8f, 0.9f, 1.0f);
    else if (IN.pos.x < 128.0f && IN.pos.y < 128.0f)
        result = float4(0.9f, 0.8f, 0.0f, 1.0f);

#else
    result = float4(1.0f, 1.0f, 0.0f, 0.0f);
#endif
    return result;
}

