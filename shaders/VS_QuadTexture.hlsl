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

cbuffer Cbuf
{
    matrix transform;
};

VSOut main(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, 0.0f, 1.0f), transform);
    OUT.texCoord = IN.texCoord;
    return OUT;
}