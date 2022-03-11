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
static const float PI = 3.14159265f;
static const float sigma = 20.2f;

#define gauss_constant 0.3989422804014327

float gauss_weight(int sampleDist)
{
	float g = 1.0f / sqrt(2.0f * PI * sigma * sigma);
	return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

#endif

struct VSOut
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
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
    float threshold = 0.7f;
    float lum = bloomMaskTexture.Sample(Sampler, IN.texCoord);
	if (lum > threshold)
		return sceneTexture.Sample(Sampler, IN.texCoord) * lum;
	else
		return 0;
#endif

#ifdef BLOOM
    uint width, height;
    sceneTexture.GetDimensions(width, height);
    float2 texelSize = float2(1.0f/width, 1.0f/height);

    uint radius = 50;
    
    result = sceneTexture.Sample(Sampler, IN.texCoord) * gauss_weight(0);
    for (uint i = 1; i <= radius; ++i)
    {
        result += sceneTexture.Sample(Sampler, IN.texCoord + float2(texelSize.x * i / width, 0.0f)) * gauss_weight(i);
        result += sceneTexture.Sample(Sampler, IN.texCoord - float2(texelSize.x * i / width, 0.0f)) * gauss_weight(i);
    }    
    
    for (i = 1; i <= radius; ++i)
    {
        result += sceneTexture.Sample(Sampler, IN.texCoord + float2(0.0f, texelSize.y * i / height)) * gauss_weight(i);
        result += sceneTexture.Sample(Sampler, IN.texCoord - float2(0.0f, texelSize.y * i / height)) * gauss_weight(i);
    }
    

    float intensity = 1.0f;
    result *= intensity;
    return result;
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

    result = FxaaPixelShader(
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
    );

#endif
    
#ifdef SCENE
    result = bloomTexture.Sample(Sampler, IN.texCoord)
           + fxaaTexture.Sample(Sampler, IN.texCoord)
           + effectTexture.Sample(Sampler, IN.texCoord);
#endif
    
    return result;
}


