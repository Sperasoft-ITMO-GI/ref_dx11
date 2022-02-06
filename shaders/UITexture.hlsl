Texture2D Text : register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix position_transform;
    matrix texture_transform;
	float4 color;
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
    float2 texCoord : TexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), position_transform);
    OUT.col = color;
    OUT.texCoord = mul(float4(IN.texCoord.x, IN.texCoord.y, 0.0f, 1.0f), texture_transform).xy;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;
    
#ifdef COLORED
    result = IN.col;
#endif

#ifdef TEXTURED
    result = Text.Sample(Sampler, IN.texCoord) * IN.col;
#endif
    
    return result;
}

