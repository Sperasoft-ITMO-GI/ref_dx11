cbuffer Cbuf
{
    matrix position_transform;
    matrix texture_transform;
    matrix lightmap_transform;
};

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
    float2 lightmapCoord : TEXCOORD;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : TexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f), position_transform);
    OUT.texCoord = mul(float4(IN.texCoord.x, IN.texCoord.y, 0.0f, 1.0f), texture_transform).xy;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    return (0.0f, 1.0f, 0.0f, 1.0f);
}