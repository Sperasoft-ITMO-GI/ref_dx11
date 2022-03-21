#define HLSL

#include "shader_defines.h"

#ifdef FXAA
#define FXAA_PC 1
#define FXAA_HLSL_4 1
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
    uint j = 1;
    uint m = radius;
	for (int i = -radius; i <= radius; i++)
	{
		float weight = gauss_weight(j % m);
        ++j;
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
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
    float threshold = 0.45f;
    float mask = bloomTexture.Sample(Sampler, IN.texCoord).r;
	if (mask > threshold) {
		//float3 glow = texColor.rgb * mask;
        //result = float4(glow, 1.0f);
        result = texColor * mask;
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
    float intensity = 7.0f;
    result = bloomTexture.Sample(Sampler, IN.texCoord) * intensity;
#endif
    
#ifdef FXAA
    float4 unused = float4(0,0,0,0);

    FxaaFloat2 pos = IN.texCoord;
    FxaaFloat4 fxaaConsolePosPos = unused;
    FxaaTex tex = { Sampler, colorTexture };
    FxaaTex fxaaConsole360TexExpBiasNegOne = { Sampler, colorTexture };
    FxaaTex fxaaConsole360TexExpBiasNegTwo = { Sampler, colorTexture };

    float w,h;
    colorTexture.GetDimensions(w,h);

    FxaaFloat2 fxaaQualityRcpFrame = float2(1.0/w, 1.0/h);

    FxaaFloat4 fxaaConsoleRcpFrameOpt = unused;
    FxaaFloat4 fxaaConsoleRcpFrameOpt2 = unused;
    FxaaFloat4 fxaaConsole360RcpFrameOpt2 = unused;
    FxaaFloat fxaaQualitySubpix =              0.75f;
    FxaaFloat fxaaQualityEdgeThreshold =       0.166f;
    FxaaFloat fxaaQualityEdgeThresholdMin =    0.0833f;
    FxaaFloat fxaaConsoleEdgeSharpness =       8.0f;
    FxaaFloat fxaaConsoleEdgeThreshold =       0.125f;
    FxaaFloat fxaaConsoleEdgeThresholdMin =    0.05f;
    FxaaFloat4 fxaaConsole360ConstDir = unused;

    return float4(FxaaPixelShader(
        pos,
        fxaaConsolePosPos,
        tex,
        tex,
        fxaaConsole360TexExpBiasNegOne,
        fxaaConsole360TexExpBiasNegTwo,
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

#ifdef MOTION_BLUR
     // Get the depth buffer value at this pixel.    
    float zOverW = depthTexture.Sample(Sampler, IN.texCoord); 
    // H is the viewport position at this pixel in the range -1 to 1.    
    float4 H = float4(IN.texCoord.x * 2 - 1, (1 - IN.texCoord.y) * 2 - 1, zOverW, 1); 
    // Transform by the view-projection inverse.    
    float4 D = mul(H, camera.view_projection_inverse); 
    // Divide by w to get the world position.    
    float4 worldPos = D / D.w; 
    
    // Current viewport position    
    float4 currentPos = H; 
    // Use the world position, and transform by the previous view-projection matrix.    
    float4 previousPos = mul(worldPos, camera.prev_view); 
    // Convert to nonhomogeneous points [-1,1] by dividing by w. 
    previousPos /= previousPos.w; 
    // Use this frame's position and last frame's to compute the pixel velocity.  
    float targetFps = 220.0f;
    float strength = camera.fps / targetFps;
    float2 velocity = ((currentPos - previousPos) / 2.f).xy * strength;
    //velocity.y = -velocity.y;

    
    // Get the initial color at this pixel.    
    float4 color = colorTexture.Sample(Sampler, IN.texCoord); 
    IN.texCoord += velocity;

    int g_numSamples = 10;
    for(int i = 1; i < g_numSamples; ++i, IN.texCoord += velocity) 
    {   // Sample the color buffer along the velocity vector.    
        float4 currentColor = colorTexture.Sample(Sampler, IN.texCoord);   
        // Add the current color to our color sum.   
        color += currentColor; 
    } 
    // Average all of the samples to get the final blur color.    
    result = color / g_numSamples ; 
    
#endif
    
#ifdef SCENE
    float4 sceneColor = colorTexture.Sample(Sampler, IN.texCoord);
    float4 bloom = bloomTexture.Sample(Sampler, IN.texCoord);
    float4 effect = effectTexture.Sample(Sampler, IN.texCoord);
    result = sceneColor + bloom + effect;
#endif
    

    
    return result;
}


