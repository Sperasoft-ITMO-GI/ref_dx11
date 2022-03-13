#define HLSL

#include "shader_defines.h"

#ifdef FXAA
#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 12
#define FXAA_GREEN_AS_LUMA 0
#include "Fxaa3_11.h"
#endif

#ifdef BLOOM
#define gauss_constant 0.3989422804014327
#define PI 3.14159265f

static const float sigma = 5.0f;

float gauss_weight(int sampleDist)
{
	float g = 1.0f / sqrt(2.0f * PI * sigma * sigma);
	return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

float4 gauss(in float2 input, in float2 tex_scale, int radius)
{
	float4 color = 0;

	float w,h;
	bloomTexture.GetDimensions(w,h);

	for (int i = 1; i <= radius; i++)
	{
		float weight = gauss_weight(i);

		// compute tap tc
		float2 tc1 = input;
		float2 tc2 = input;
		tc1 += (i / float2(w,h)) * tex_scale;
		tc2 -= (i / float2(w,h)) * tex_scale;

		color += (bloomTexture.Sample(Sampler, tc1)  + bloomTexture.Sample(Sampler, tc2)) * weight;
	}
	color += bloomTexture.Sample(Sampler, input) * gauss_weight(0);
	return color;
}

#endif

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
	float4x4 proj = mul(transpose(camera.orthogonal), model.mod);
	
    VSOut OUT;
    OUT.pos = mul(proj, float4(IN.pos, 1.0f));
    OUT.texCoord = IN.texCoord;
    
    return OUT;
}

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result;
    
#ifdef DEFAULT
    result = model.color;
#endif
    
#ifdef BLOOM_MASK
    float threshold = 0.5f;
    float mask = bloomTexture.Sample(Sampler, IN.texCoord).r;
	if (mask > threshold) {
		float4 glow = sceneTexture.Sample(Sampler, IN.texCoord) * mask;
        result = glow;
	}
    else
		result = 0;
#endif

#ifdef BLOOM
    int radius = 9;
    result = gauss(IN.texCoord, float2(1, 0), radius);
    //result += gauss(IN.texCoord, float2(0, 1), radius);
#endif
    
#ifdef FXAA
    float4 unused = float4(0,0,0,0);

    FxaaFloat2 pos = IN.texCoord;
    FxaaFloat4 fxaaConsolePosPos = unused;
    FxaaTex tex;
    tex.smpl = Sampler;
    tex.tex  = sceneTexture;
    FxaaTex fxaaConsole360TexExpBiasNegOne = { Sampler, sceneTexture };
    FxaaTex fxaaConsole360TexExpBiasNegTwo = { Sampler, sceneTexture };

    float w,h;
    sceneTexture.GetDimensions(w,h);

    FxaaFloat2 fxaaQualityRcpFrame = float2(1.0/w, 1.0/h);

    FxaaFloat4 fxaaConsoleRcpFrameOpt = unused;
    FxaaFloat4 fxaaConsoleRcpFrameOpt2 = unused;
    FxaaFloat4 fxaaConsole360RcpFrameOpt2 = unused;
    FxaaFloat fxaaQualitySubpix =              1.0f;
    FxaaFloat fxaaQualityEdgeThreshold =       0.063f;
    FxaaFloat fxaaQualityEdgeThresholdMin =    0.0312f;
    FxaaFloat fxaaConsoleEdgeSharpness =       4.0f;
    FxaaFloat fxaaConsoleEdgeThreshold =       0.25f;
    FxaaFloat fxaaConsoleEdgeThresholdMin =    0.04f;
    FxaaFloat4 fxaaConsole360ConstDir = unused;

    result = float4(FxaaPixelShader(
        pos,
        fxaaConsolePosPos,
        tex,
        tex,
        tex,
        tex,
        fxaaQualityRcpFrame,
        fxaaConsoleRcpFrameOpt,
        fxaaConsoleRcpFrameOpt2,
        fxaaConsole360RcpFrameOpt2,
        fxaaQualitySubpix,
        fxaaQualityEdgeThreshold,
        fxaaQualityEdgeThresholdMin,
        fxaaConsoleEdgeSharpness,
        fxaaConsoleEdgeThreshold,
        fxaaConsoleEdgeThresholdMin,
        fxaaConsole360ConstDir
    ).rgb, 1.0f);

#endif
    
#ifdef SCENE
    result = sceneTexture.Sample(Sampler, IN.texCoord);
            //bloomTexture.Sample(Sampler, IN.texCoord);
           //+ fxaaTexture.Sample(Sampler, IN.texCoord)
           //+ effectTexture.Sample(Sampler, IN.texCoord);
#endif
    
    return result;
}


