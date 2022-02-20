cbuffer Cbuf
{
    matrix position_transform;
    float4 color;
};

struct VSOut
{
    float4 pos : SV_Position;
};

struct VSIn
{
    float3 pos : Position;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(position_transform, float4(IN.pos, 1.0f));
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    return float4(1, 0, 0, 1);
}

