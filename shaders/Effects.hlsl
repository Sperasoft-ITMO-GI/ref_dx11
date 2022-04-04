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

#ifdef TAA
static const uint kNeighborsCount = 9;
static const uint neighborCount = 5;

static const float2 kOffsets3x3[9] =
{
    float2(-1, -1), //upper left
	float2(0, -1), //up
	float2(1, -1), //upper right
	float2(-1, 0), //left
	float2(0, 0), // K
	float2(1, 0), //right
	float2(-1, 1), //lower left
	float2(0, 1), //down
	float2(1, 1), //lower right
}; //k is index 4

static const float2 kOffsets2x2[5] =
{
	float2(-1, 0), //left
	float2(0, -1), //up
	float2( 0,  0), // K
	float2(1, 0), //right
	float2(0, 1) //down
}; //k is index 3

float2 GetClosestUV(float2 texCoord)
{
    float2 deltaRes = float2(1.0f / camera.resolution.x, 1.0f / camera.resolution.y);
    float2 closestUV = texCoord;
    float closestDepth = 1.0f;
    
    for (uint iter = 0; iter < kNeighborsCount; iter++)
    {
        float2 newUV = texCoord.xy + (kOffsets3x3[iter] * deltaRes);
        float depth = depthTexture.Sample(Sampler, newUV).x;
        if (depth < closestDepth)
        {
            closestDepth = depth;
            closestUV = newUV;
        }
    }

    return closestUV;
}

float2 MinMaxDepths(in float neighborDepths[kNeighborsCount])
{
    float minDepth = neighborDepths[0];
    float maxDepth = neighborDepths[0];

    for (int iter = 1; iter < kNeighborsCount; iter++)
    {
        minDepth = min(minDepth, neighborDepths[iter]);
        maxDepth = max(maxDepth, neighborDepths[iter]);
    }

    return float2(minDepth, maxDepth);
}

float4 MinColors(in float4 neighborColors[neighborCount])
{
    float4 minColor = neighborColors[0];

    for (int iter = 1; iter < neighborCount; iter++)
    {
        minColor = min(minColor, neighborColors[iter]);
    }

    return minColor;
}

float4 MaxColors(in float4 neighborColors[neighborCount])
{
    float4 maxColor = neighborColors[0];

    for (int iter = 1; iter < neighborCount; iter++)
    {
        maxColor = max(maxColor, neighborColors[iter]);
    }

    return maxColor;
}

float4 MinColors2(in float4 neighborColors[kNeighborsCount])
{
    float4 minColor = neighborColors[0];

    for (int iter = 1; iter < neighborCount; iter++)
    {
        minColor = min(minColor, neighborColors[iter]);
    }

    return minColor;
}

float4 MaxColors2(in float4 neighborColors[kNeighborsCount])
{
    float4 maxColor = neighborColors[0];

    for (int iter = 1; iter < neighborCount; iter++)
    {
        maxColor = max(maxColor, neighborColors[iter]);
    }

    return maxColor;
}

float4 ConstrainHistory(float4 neighborColors[neighborCount])
{
    float4 previousColorMin = MinColors(neighborColors);
    float4 previousColorMax = MaxColors(neighborColors);

	//float4 constrainedHistory = float4(0);
    return clamp(neighborColors[2], previousColorMin, previousColorMax);
}

// note: clips towards aabb center + p.w
float4 clip_aabb(float3 colorMin, float3 colorMax, float4 currentColor, float4 previousColor)
{
    float3 p_clip = 0.5 * (colorMax + colorMin);
    float3 e_clip = 0.5 * (colorMax - colorMin);
    float4 v_clip = previousColor - float4(p_clip, currentColor.a);
    float3 v_unit = v_clip.rgb / e_clip;
    float3 a_unit = abs(v_unit);
    float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));
    if (ma_unit > 1.0)
    {
        return float4(p_clip, currentColor.a) + v_clip / ma_unit;
    }
    else
    {
        return previousColor; // point inside aabb
    }
}

float4 Inside2Resolve(float2 velocity, float2 texCoord)
{
    float2 deltaRes = float2(1.0 / camera.resolution.x, 1.0 / camera.resolution.y);

    float4 current3x3Colors[kNeighborsCount];
    float4 previous3x3Colors[kNeighborsCount];

    for (uint iter = 0; iter < kNeighborsCount; iter++)
    {
        float2 newUV = texCoord.xy + (kOffsets3x3[iter] * deltaRes);

        current3x3Colors[iter] = colorTexture.Sample(Sampler, newUV);

        previous3x3Colors[iter] = prevcolorTexture.Sample(Sampler, newUV + velocity);
    }

    float4 rounded3x3Min = MinColors2(current3x3Colors);
    float4 rounded3x3Max = MaxColors2(previous3x3Colors);

///////////////////////////////////////////////////////////////////////////////
    float4 current2x2Colors[neighborCount];
    float4 previous2x2Colors[neighborCount];
    for (iter = 0; iter < neighborCount; iter++)
    {
        float2 newUV = texCoord.xy + (kOffsets2x2[iter] * deltaRes);

        current2x2Colors[iter] = colorTexture.Sample(Sampler, newUV);

        previous3x3Colors[iter] = prevcolorTexture.Sample(Sampler, newUV + velocity);
    }

    float4 min2 = MinColors(current2x2Colors);
    float4 max2 = MaxColors(previous2x2Colors);

	//mix the 3x3 and 2x2 min maxes together
    float4 mixedMin = lerp(rounded3x3Min, min2, 0.5);
    float4 mixedMax = lerp(rounded3x3Max, max2, 0.5);

    float feedbackFactor = 0.5f;
    float velocityScale = 0.5f;
    float testVel = feedbackFactor - (length(velocity) * velocityScale);
    return lerp(current2x2Colors[2], clip_aabb(mixedMin.rgb, mixedMax.rgb, current2x2Colors[2], ConstrainHistory(previous2x2Colors)), testVel);
}

float4 Custom2Resolve(in float preNeighborDepths[kNeighborsCount], in float curNeighborDepths[kNeighborsCount], float2 velocity, float2 texCoord)
{
	//use the closest depth instead?
    float2 preMinMaxDepths = MinMaxDepths(preNeighborDepths);
    float2 curMinMaxDepths = MinMaxDepths(curNeighborDepths);

    float highestDepth = min(preMinMaxDepths.x, curMinMaxDepths.x); //get the furthest
    float lowestDepth = max(preMinMaxDepths.x, curMinMaxDepths.x); //get the closest

    float depthFalloff = abs(highestDepth - lowestDepth);

    float4 res = 0;

	//float testVel = feedbackFactor - (length(velocity) * velocityScale);
    float4 taa = Inside2Resolve(velocity, texCoord);

    float averageDepth = 0;
    for (uint iter = 0; iter < kNeighborsCount; iter++)
    {
        averageDepth += curNeighborDepths[iter];
    }

    averageDepth /= kNeighborsCount;

	//for dithered edges, detect if the adge has been dithered? 
	//use a 3x3 grid to see if anyhting around it has high enough depth?
    float maxDepthFalloff = 0.5f;
    if (averageDepth < depthFalloff)
    {
        res = taa; //float4(1, 0, 0, 1);
    }
    else
    {
        res = colorTexture.Sample(Sampler, texCoord);
    }

    return res;
}

float3 rgb_to_ycbcr(float3 rgb) {
    float y = 0.299 * rgb.x + 0.587 * rgb.y + 0.114 * rgb.z;
    float cb = (rgb.z - y) * 0.565;
    float cr = (rgb.x - y) * 0.713;

    return float3(y, cb, cr);
}

float3 ycbcr_to_rgb(float3 yuv) {
    return float3(
        yuv.x + 1.403 * yuv.z,
        yuv.x - 0.344 * yuv.y - 0.714 * yuv.z,
        yuv.x + 1.770 * yuv.y
    );
}

float3 lerp(float alpha, float3 left, float3 right)
{
    return left*(1-alpha)+alpha*right;
}


float3 ClipAABB(float3 aabbMin, float3 aabbMax, float3 prevSample, float3 avg)
{
#if 1
	// note: only clips towards aabb center (but fast!)
	float3 p_clip = 0.5 * (aabbMax + aabbMin);
	float3 e_clip = 0.5 * (aabbMax - aabbMin);

	float3 v_clip = prevSample - p_clip;
	float3 v_unit = v_clip.xyz / e_clip;
	float3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0)
		return p_clip + v_clip / ma_unit;
	else
		return prevSample;// point inside aabb
#else
	float3 r = prevSample - avg;
	float3 rmax = aabbMax - avg.xyz;
	float3 rmin = aabbMin - avg.xyz;

	const float eps = 0.000001f;

	if (r.x > rmax.x + eps)
		r *= (rmax.x / r.x);
	if (r.y > rmax.y + eps)
		r *= (rmax.y / r.y);
	if (r.z > rmax.z + eps)
		r *= (rmax.z / r.z);

	if (r.x < rmin.x - eps)
		r *= (rmin.x / r.x);
	if (r.y < rmin.y - eps)
		r *= (rmin.y / r.y);
	if (r.z < rmin.z - eps)
		r *= (rmin.z / r.z);

	return avg + r;
#endif
}


static float FilterTriangle(in float x)
{
	return saturate(1.0f - x);
}

float FilterBlackmanHarris(in float x)
{
	x = 1.0f - x;

	const float a0 = 0.35875f;
	const float a1 = 0.48829f;
	const float a2 = 0.14128f;
	const float a3 = 0.01168f;
	const float PI = 3.14159265359f;
	return saturate(a0 - a1 * cos(PI * x) + a2 * cos(2 * PI * x) - a3 * cos(3 * PI * x));
}

float Filter(in float x, in int filterMode)
{
	if (filterMode == 1)
	{
		return FilterTriangle(x);
	}
	else if (filterMode == 2)
	{
		return FilterBlackmanHarris(x);
	}

	return x;
}

float2 LoadVelocity(float2 screenPos)
{
	float2 velocity = 0;

	int DilationMode = 3;
	
	float w,h;
	velocityTexture.GetDimensions(w,h);
	float2 shift = float2(1.0,1.0) / float2(w,h);

	if (DilationMode == 0)
	{
        velocity = velocityTexture.Sample(Sampler, screenPos);
    }
	else if (DilationMode == 1) // nearest depth
	{
		float closestDepth = 10.0f;
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
                float2 neighborVelocity = velocityTexture.Sample(Sampler, screenPos + shift * int2(x, y));
				float neighborDepth = depthTexture.Sample(Sampler, screenPos + shift*int2(x, y));

				if (neighborDepth < closestDepth)
				{
					velocity = neighborVelocity;
					closestDepth = neighborDepth;
				}
			}
		}
	}
	else if (DilationMode == 2) // greatest velocity
	{
		float greatestVelocity = -1.0f;
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				float2 neighborVelocity = velocityTexture.Sample(Sampler, screenPos + shift*int2(x, y));
				float neighborVelocityMag = dot(neighborVelocity, neighborVelocity);
				if (dot(neighborVelocity, neighborVelocity) > greatestVelocity)
				{
					velocity = neighborVelocity;
					greatestVelocity = neighborVelocityMag;
				}
			}
		}
	}

	return velocity;
}
#endif

float4 PSMain(VSOut IN) : SV_Target
{
    float4 result = 0;
    
#ifdef DEFAULT
    result = model.color;
#endif
    
#ifdef GLOW
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
    float threshold = 0.6f;
    float mask = bloomTexture.Sample(Sampler, IN.texCoord).r;
	if (mask > threshold) {
		//float3 glow = texColor.rgb * mask;
        //result = float4(glow, 1.0f);
        result = float4(texColor.rgb * mask, 1.0f);
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

    int g_numSamples = 5;
    for(int i = 1; i < g_numSamples; ++i, IN.texCoord += velocity) 
    {   // Sample the color buffer along the velocity floattor.    
        float4 currentColor = colorTexture.Sample(Sampler, IN.texCoord);   
        // Add the current color to our color sum.   
        color += currentColor; 
    } 
    // Average all of the samples to get the final blur color.    
    result = color / g_numSamples * 1.5; 
    
#endif
    
#ifdef TAA
    float2 texCords = IN.texCoord;

	float3 clrMin = 99999999.0f;
	float3 clrMax = -99999999.0f;

	float3 m1 = 0.0f;
	float3 m2 = 0.0f;
	float mWeight = 0.0f;

	float depth = depthTexture.Sample(Sampler, texCords);

	float4 clipPos = float4(2 * texCords - 1, depth, 1);
	clipPos.y = -clipPos.y;

	float4 worldPos = mul(camera.view_projection_inverse, clipPos);
	worldPos.xyzw /= worldPos.w;

	float4 clipPosPrev = mul(camera.prev_view, worldPos);
	clipPosPrev.xy /= clipPosPrev.w;

	float2 uvPrev = clipPosPrev.xy * float2(0.5, -0.5) + float2(0.5, 0.5);

	//float2 historyUV1 = uvPrev + JitterOffset;

	//load velocity
	float2 historyUV = texCords - LoadVelocity(texCords);

	//outputRT[screenPos] = float4(10000*abs(historyUV- historyUV1), 0, 1);
	//return;

	//find area min max
	int SampleRadius = 1;
	float w,h;
	colorTexture.GetDimensions(w,h);
	float2 shift = float2(1.0,1.0) / float2(w,h);


	for (int y = -SampleRadius; y <= SampleRadius; ++y)
	{
		for (int x = -SampleRadius; x <= SampleRadius; ++x)
		{
			//float2 samplePos = screenPos + float2(x, y);

			float3 tap = rgb_to_ycbcr(colorTexture.Sample(Sampler, texCords + shift*int2(x, y)));;

			clrMin = min(clrMin, tap);
			clrMax = max(clrMax, tap);

			m1 += tap;
			m2 += tap * tap;
			mWeight += 1.0f;
		}
	}

	float3 current = rgb_to_ycbcr(colorTexture.Sample(Sampler, texCords));
	float3 history;
	
	int ReprojectionMode = 3;

	if (ReprojectionMode == 0)
	{
		history = rgb_to_ycbcr(prevcolorTexture.Sample(Sampler, texCords));
			//historyBuffer[screenPos].rgb;
	}
	else if (ReprojectionMode == 1)
	{
		history = rgb_to_ycbcr(prevcolorTexture.Sample(Sampler, texCords));
	}
	else
	{
		float3 sum = 0.0f;
		float totalWeight = 0.0f;
		int ReprojectionFilter = 1;

		float2 reprojectedPos = historyUV * w * h;
		for (int y = -1; y <= 2; y++)
		{
			for (int x = -1; x <= 2; x++)
			{
				float2 samplePos = floor(reprojectedPos + float2(x, y)) + 0.5f;
				float3 reprojectedSample = rgb_to_ycbcr(prevcolorTexture.Sample(Sampler, texCords + shift*int2(x, y)));

				float2 sampleDist = abs(samplePos - reprojectedPos);
				float filterWeight = Filter(sampleDist.x, ReprojectionFilter) * Filter(sampleDist.y, ReprojectionFilter);

				//float sampleLum = Luminance(reprojectedSample);

				//if (UseExposureFiltering)
				//	sampleLum *= exp2(ManualExposure - ExposureScale + ExposureFilterOffset);

				//if (InverseLuminanceFiltering)
				//	filterWeight *= 1.0f / (1.0f + sampleLum);

				sum += reprojectedSample * filterWeight;
				totalWeight += filterWeight;
			}
		}

		history = max(sum / totalWeight, 0.0f);
	}

    int NeighborhoodClampMode = 3;

	if (NeighborhoodClampMode == 1) // RGB Clamp
	{
		history = clamp(history, clrMin, clrMax);
	}
	else if (NeighborhoodClampMode == 2 ) // RGB Clip
	{
		history = ClipAABB(clrMin, clrMax, history, m1 / mWeight);
	}
	else if (NeighborhoodClampMode == 3 )// Variance Clip
	{
		float VarianceClipGamma = 1;
		float3 mu = m1 / mWeight;
		float3 sigma = sqrt(abs(m2 / mWeight - mu * mu));
		float3 minc = mu - VarianceClipGamma * sigma;
		float3 maxc = mu + VarianceClipGamma * sigma;
		history = ClipAABB(minc, maxc, history, mu);
	}
	
    uint num = 10;
    return float4(ycbcr_to_rgb(lerp(1.0 -  1.0 / num, current, history)), 1.0);
#endif
    
#ifdef SCENE
    float4 sceneColor = colorTexture.Sample(Sampler, IN.texCoord);
    float4 bloom = bloomTexture.Sample(Sampler, IN.texCoord);
    float4 effect = effectTexture.Sample(Sampler, IN.texCoord);
    result = sceneColor + bloom + effect;
#endif
    

    
    return result;
}


