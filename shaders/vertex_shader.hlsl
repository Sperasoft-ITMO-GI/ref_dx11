struct VSOut
{
    float4 pos : SV_Position;
    float3 color : Color;
};

struct VSIn
{
    float2 pos : Position;
    float3 color : Color;
};

cbuffer Cbuf
{
    matrix transform;
};

VSOut main(VSIn vsin)
{
    VSOut vso;
    vso.pos = mul(float4(vsin.pos.x, vsin.pos.y, 0.0f, 1.0f), transform);
    vso.color = vsin.color;
    return vso;
}