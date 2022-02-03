Texture2D Text_0 : register(t0);
sampler Sampler : register(s0);

cbuffer Cbuf
{
    matrix position_transform;
};

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
    float2 lightmapCoord : LMTEXCOORD;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
	//IN.texCoord = normalize(IN.texCoord);
	//IN.pos = normalize(IN.pos);
    OUT.pos = mul(position_transform, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
	//OUT.pos = float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f);
    OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = float4(0.5f, 0.0f, 0.5f, 0.0f);

#ifdef SOLID
	result = Text_0.Sample(Sampler, IN.texCoord);
#endif
    return result;
}