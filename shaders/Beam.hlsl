cbuffer Cbuf
{
    matrix position_transform;
    float4 color;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

struct VSIn
{
    float3 pos : Position;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(position_transform, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.color = color;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = IN.color;
    return result;
}

