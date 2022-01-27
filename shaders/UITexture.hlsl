Texture2D Text : register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix position_transform;
    matrix color_transform;
    matrix texture_transform;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 col : Color;
    float2 texCoord : TEXCOORD;
};

struct VSIn
{
    float2 pos : Position;
    float4 col : Color;
    float2 texCoord : TexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), position_transform);
    OUT.col = mul(IN.col, color_transform);
    OUT.texCoord = mul(float4(IN.texCoord.x, IN.texCoord.y, 0.0f, 1.0f), texture_transform).xy;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = IN.col;
    
#ifdef COLORED
    result;
#endif

#ifdef FADE 
    result.w = 0.8f;
#endif

#ifdef TEXTURED
    result = Text.Sample(Sampler, IN.texCoord);
#endif
    
    return result;
}

