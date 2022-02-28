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

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;
    
#ifdef DEFAULT
    result = model.color;
#endif
    
#ifdef SCENE
    result = colorTexture.Sample(Sampler, IN.texCoord);
#endif
    
    return result;
}

