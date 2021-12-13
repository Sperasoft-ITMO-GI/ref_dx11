Texture2D text : register(t0);
sampler Sampler : register(s0);

struct PSin
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD;
};

float4 main(PSin IN) : SV_Target
{
	return text.Sample(Sampler, IN.texCoord);
}