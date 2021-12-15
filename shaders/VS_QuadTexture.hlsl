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

cbuffer Cbuf
{
    matrix orthogonal;
};

VSOut main(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), orthogonal);
    OUT.texCoord = IN.texCoord;
    OUT.col = IN.col;
    return OUT;
}