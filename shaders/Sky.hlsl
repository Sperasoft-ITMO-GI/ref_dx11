TextureCube tex: register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix position_transform;
};

struct VSOut
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
};

struct VSIn
{
    float3 pos : Position;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.worldPos = IN.pos;
    OUT.pos = mul(position_transform, float4(IN.pos, 0.0f) );
    OUT.pos.z = OUT.pos.w;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    return tex.Sample(Sampler, IN.worldPos);
}

