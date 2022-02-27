#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
};

struct VSIn
{
    float2 pos : Position;
    float2 texCoord : TexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), ui_buffer.position_transform);
    OUT.texCoord = mul(float4(IN.texCoord.x, IN.texCoord.y, 0.0f, 1.0f), ui_buffer.texture_transform).xy;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;
    
#ifdef COLORED
    result = ui_buffer.color;
#endif

#ifdef TEXTURED
    result = colorTexture.Sample(Sampler, IN.texCoord) * ui_buffer.color;
#endif
    
    return result;
}

