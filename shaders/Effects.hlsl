#define HLSL

#include "shader_defines.h"

#ifdef FXAA
#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 12
#define FXAA_GREEN_AS_LUMA 0
#include "Fxaa3_11.h"
#endif

#if defined HORIZONTAL_BLUR | defined VERTICAL_BLUR
#define gauss_constant 0.3989422804014327
#define PI 3.14159265f

static const float sigma = 4.7f;

float gauss_weight(int sampleDist)
{
	float g = 1.0f / sqrt(2.0f * PI * sigma * sigma);
	return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

float4 gauss_filter_pass(in float2 texCoord, in float2 direction, int radius)
{
	float4 color = 0;
	float w, h;
	bloomTexture.GetDimensions(w, h);

    color += bloomTexture.Sample(Sampler, texCoord) * gauss_weight(0);
	for (int i = -radius; i <= radius; i++)
	{
		float weight = gauss_weight(i);
		float2 texCoordOffset = texCoord;
		texCoordOffset += (i / float2(w, h)) * direction;

		color += bloomTexture.Sample(Sampler, texCoordOffset) * weight;
	}
	
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
    float4 result = 0;
    
#ifdef DEFAULT
    result = model.color;
#endif
    
#ifdef GLOW
    float threshold = 0.45f;
    float mask = bloomTexture.Sample(Sampler, IN.texCoord).r;
	if (mask > threshold) {
		float4 glow = colorTexture.Sample(Sampler, IN.texCoord) * mask;
        result = glow;
	}
    else
		result = 0;
#endif

#ifdef HORIZONTAL_BLUR
    int radius = 9;
    result = gauss_filter_pass(IN.texCoord, float2(1, 0), radius);
#endif
    
#ifdef VERTICAL_BLUR
    int radius = 9;
    result = gauss_filter_pass(IN.texCoord, float2(0, 1), radius);
#endif
    
#ifdef INTENSITY
    float intensity = 5.0f;
    result = bloomTexture.Sample(Sampler, IN.texCoord) * intensity;
#endif
    
#ifdef FXAA
    float4 unused = float4(0,0,0,0);

    FxaaFloat2 pos = IN.texCoord;
    FxaaFloat4 fxaaConsolePosPos = unused;
    FxaaTex tex;
    tex.smpl = Sampler;
    tex.tex  = colorTexture;
    FxaaTex fxaaConsole360TexExpBiasNegOne = { Sampler, colorTexture };
    FxaaTex fxaaConsole360TexExpBiasNegTwo = { Sampler, colorTexture };

    float w,h;
    colorTexture.GetDimensions(w,h);

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
    float4 sceneColor = colorTexture.Sample(Sampler, IN.texCoord);
    float4 light = float4(lightmapTexture.Sample(Sampler, IN.texCoord).rgb, 1.0f);
    float intensity = 3.0f;
    float4 bloom = bloomTexture.Sample(Sampler, IN.texCoord);
    float4 effect = effectTexture.Sample(Sampler, IN.texCoord);
    float4 fxaa = fxaaTexture.Sample(Sampler, IN.texCoord);
    result = sceneColor * light + bloom * intensity + effect + fxaa;
#endif
    
    return result;
}


