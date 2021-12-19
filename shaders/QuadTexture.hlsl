Texture2D Text : register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix orthogonal;
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

VSOut VSmain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), orthogonal);
    OUT.texCoord = IN.texCoord;
    OUT.col = IN.col;
    return OUT;
}

float4 PSmain(VSOut IN) : SV_Target
{
    return Text.Sample(Sampler, IN.texCoord);
}