Texture2D Texture : register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix position_transform;
	float4 color;
};

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(position_transform, float4(IN.pos, 1.0f));
    OUT.texCoord = IN.texCoord;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;

#ifdef DEFAULT
	result = (Texture.Sample(Sampler, IN.texCoord) * color);
#endif

    return result;
}