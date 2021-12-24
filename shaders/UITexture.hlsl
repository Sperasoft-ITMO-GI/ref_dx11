Texture2D Text : register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix transform;
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
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), transform);
    OUT.col = IN.col;
    OUT.texCoord = IN.texCoord;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    
//#ifdef COLORED
//    res = IN.col;
//#endif
    
//#ifdef TEXTURED
//    res = Text.Sample(Sampler, IN.texCoord);
//#endif

    return mul(Text.Sample(Sampler, IN.texCoord), IN.col);
}

