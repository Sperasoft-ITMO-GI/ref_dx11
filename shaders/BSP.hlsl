Texture2D diffuseText : register(t0);
Texture2D lightMapText : register(t1);
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
    float2 lightmapCoord : LMTEXCOORD;
	float4 col : Color;
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
    OUT.pos = mul(position_transform, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
	OUT.col = color;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;

#ifdef SOLID
	result = diffuseText.Sample(Sampler, IN.texCoord) * IN.col;
#endif

#ifdef ALPHA
	result = diffuseText.Sample(Sampler, IN.texCoord) * IN.col;
#endif

#ifdef WATER
	result = diffuseText.Sample(Sampler, IN.texCoord) * IN.col;
#endif

#ifdef LIGHTMAP
	result = diffuseText.Sample(Sampler, IN.texCoord) * IN.col;
#endif

#ifdef LIGHTMAPPEDPOLY
	result = diffuseText.Sample(Sampler, IN.texCoord);
	result *= lightMapText.Sample(Sampler, IN.lightmapCoord);
	result *= IN.col;
#endif

    return result;
}