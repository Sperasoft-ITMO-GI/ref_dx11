#define HLSL

#include "shader_defines.h"

struct VSOut
{
    float4 pos : SV_Position;
    float4 prevPos : Position;
    float2 texCoord : TEXCOORD0;
    float2 lightmapCoord : LMTEXCOORD;
	float4 col : Color;
    float3 norm : NORMAL;
    float2 worldPos : TEXCOORD1;
};

struct VSIn
{
    float3 pos : Position;
    float3 norm : NORMAL;
	float4 col : Color_In;
    float2 texCoord : TexCoord;
    float2 lightmapCoord : LmTexCoord;
};

struct PSOut
{
    float4 color : SV_Target0;
    float4 mask : SV_Target1;
    float2 velosity : SV_Target2;
};

VSOut VSMain(VSIn IN)
{
	#ifdef WEAPON
	// if changes it to cam.perspective nothing changes
	float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
	#else
	float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
	#endif
	
    VSOut OUT;
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.prevPos = mul(camera.prev_view, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.texCoord = IN.texCoord;
	OUT.lightmapCoord = IN.lightmapCoord;
	OUT.col = IN.col;
    OUT.norm = mul(camera.view, float4(IN.norm, 0));
    OUT.worldPos = mul(camera.orthogonal, float4(IN.texCoord, 0, 0));
    return OUT;
}

PSOut PSMain(VSOut IN) 
{
    PSOut result;
    
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
    float luma = dot(texColor.rgb, float3(0.3, 0.59, 0.11));
    float mask = saturate(luma * 4 - 3);
    float3 glow = texColor.rgb * mask;
   
    result.color = texColor * IN.col * model.color;
    
    result.color += saturate(dot(light.direction, IN.norm) * light.color) * light.intensity;
    result.mask = 0;

    float2 a = (IN.pos.xy / IN.pos.w) * 0.5 + 0.5;
    float2 b = (IN.prevPos.xy / IN.prevPos.w) * 0.5 + 0.5;
    result.velosity = a - b;
   
    
    return result;
}