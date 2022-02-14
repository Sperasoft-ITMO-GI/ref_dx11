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
    float4 CbCol : Color_Cb;
};

struct VSIn
{
    float3 pos : Position;
	float4 col : Color_In;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(position_transform, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
	OUT.col = IN.col;
    OUT.CbCol = color;
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;

#ifdef ALPHA
	result = (diffuseText.Sample(Sampler, IN.texCoord) * IN.col) * IN.CbCol;
#endif

    return result;
}