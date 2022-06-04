#define HLSL

#include "shader_defines.h"

#define IDENTITY_MATRIX float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

float2 CreateHaltonNumber(unsigned int index, int2 base)
{
    float f = 1;
    float2 r = 0;
    int current = index;
    do
    {
        f = f / base.x;
        r.x = r.x + f * (current % base.x);
        current = floor(current / base.x);
    } while (current);
    
    current = index;
    do
    {
        f = f / base.y;
        r.y = r.y + f * (current % base.y);
        current = floor(current / base.y);
    } while (current);
    
    return r;
}

struct VSOut
{ 
#ifdef TEX
    float4 pos : SV_Position;
    float4 worldPos : POSITION0;
    float3 norm : NORMAL;
#endif
    
#ifdef LIGHTMAP
    float4 pos : SV_Position;
    float2 lightmapCoord : LMTEXCOORD;
#endif
    
#if defined LIGHTMAPPEDPOLY | defined ALPHA | defined WATER
    float4 pos : SV_Position;
    float4 prevPos : POSITION0;
    float4 newPos : POSITION1;
    float4 worldPos : POSITION2;
    float2 lightmapCoord : LMTEXCOORD;
    float3 norm : NORMAL;  
#endif
    
    float2 texCoord : TEXCOORD0;
};

struct VSIn
{
    float3 pos : Position;
    float2 texCoord : TexCoord;
    float3 norm : Normal;
    float2 lightmapCoord : LmTexCoord;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    
#ifdef TEX
    OUT.pos = mul(transpose(buffer.orthogonal), float4(IN.lightmapCoord.xy, 0, 1));
    OUT.worldPos = float4(IN.pos, 1); //mul(camera.view, float4(IN.pos, 1));
    //if (IN.norm.x > 0)
    //    IN.norm.x *= -1;
    //if (IN.norm.y > 0)
        //IN.norm.y *= -1;  
    //if (IN.norm.z > 0)
    //    IN.norm.z *= -1;

    OUT.norm = IN.norm;
   
#endif
    
#ifdef LIGHTMAP
    OUT.pos = mul(transpose(buffer.orthogonal), float4(IN.lightmapCoord.xy, 0, 1));
    OUT.lightmapCoord = IN.lightmapCoord;
#endif

#if defined LIGHTMAPPEDPOLY | defined ALPHA | defined WATER
    float4x4 proj = mul(mul(camera.perspective, camera.view), model.mod);
    float4x4 prevProj = mul(mul(camera.perspective, camera.prev_view), model.mod);
    OUT.pos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.prevPos = float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f);
    OUT.newPos = mul(proj, float4(IN.pos.x, IN.pos.y, IN.pos.z, 1.0f));
    OUT.lightmapCoord = IN.lightmapCoord;
    OUT.norm = IN.norm.xyz;// mul(camera.perspective, float4(IN.norm.xyz, 0));
    OUT.worldPos = float4(IN.pos, 1);
#endif

    OUT.texCoord = IN.texCoord;
    
    return OUT;
}

struct PSOut
{

#if defined LIGHTMAPPEDPOLY | defined ALPHA | defined WATER
	float4 color       : SV_Target0;
	float4 mask        : SV_Target1;
    float2 velosity    : SV_Target2;
    float4 lightmap    : SV_Target3;
    float4 albedo      : SV_Target4;
    float4 positions   : SV_Target5;
    float4 normals     : SV_Target6;
    float4 albedo3d    : SV_Target7;
#endif
    
#ifdef TEX 
    float4 positions   : SV_Target0;
    float4 normals     : SV_Target1;
    float4 albedo      : SV_Target2;
#endif
    
#ifdef LIGHTMAP
    float4 light   : SV_Target0;
#endif
   
};

float4 CalculatePointLight(float3 lightPos, float3 pos, float3 normal)
{
    float3 lightDir;
    float3 diffuse;
    float dist;
    float att;
    
    float radius = 150;
    float kc = 1.0f;
    float kl = 1 / radius;
    float kq = 1 / (radius * radius);
    
    lightDir = pos - lightPos;
    dist = length(lightDir);
    att = 1.0f / (kc + kl * dist + kq * dist * dist);
    diffuse = max(dot(normal, normalize(lightDir)), 0.0f) * att;
    return float4(diffuse, 0);
}

PSOut PSMain(VSOut IN)
{
	PSOut result;
    
#if defined LIGHTMAPPEDPOLY | defined ALPHA | defined WATER
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
    float4 lightmap = 0;
	
    float luma = dot(texColor.rgb, float3(0.3, 0.59, 0.11));
	float mask = saturate(luma * 4 - 3);
	float3 glow = texColor.rgb * mask;
    
    result.albedo = texColor * model.color;
    
#ifdef LIGHTMAPPEDPOLY
    lightmap = lightmapTexture.Sample(Sampler, IN.lightmapCoord);
    result.lightmap = lightmap;
    //texColor *= lightmap;
#endif

    result.color = texColor * model.color;
    //result.color += saturate(dot(light.direction, IN.norm) * light.color) * light.intensity;
    result.mask = float4(glow, 1.0f);
    
    float2 a = (IN.newPos.xy / IN.newPos.w) * 0.5 + 0.5;
    float2 b = (IN.prevPos.xy / IN.prevPos.w) * 0.5 + 0.5;
    result.velosity = a - b;
#ifdef LIGHTMAPPEDPOLY
    result.positions = IN.worldPos;//positionTexture.Sample(Sampler, IN.lightmapCoord);
    
    //if (IN.norm.x > 0)
    //    IN.norm.x *= -1;
    //if (IN.norm.y > 0)
    //    IN.norm.y *= -1;  
    //if (IN.norm.z > 0)
    //    IN.norm.z *= -1;
    
    result.normals = float4(normalize(IN.norm), 0);//normalTexture.Sample(Sampler, IN.lightmapCoord);
    result.albedo3d = albedoTexture.Sample(Sampler, IN.lightmapCoord);
#endif
#endif
    
#ifdef TEX
    float4 texColor = colorTexture.Sample(Sampler, IN.texCoord);
    result.positions = float4(IN.worldPos.xyz, 1);//float4(frac(IN.worldPos.xyz / 256), 1);
    result.normals = float4(IN.norm, 1);
    result.albedo = texColor * model.color;
#endif 
    
#ifdef LIGHTMAP
    float4 positions = positionTexture.Sample(Sampler, IN.lightmapCoord);
    float4 normals = normalTexture.Sample(Sampler, IN.lightmapCoord);
    float4 albedo = albedoTexture.Sample(Sampler, IN.lightmapCoord);
    
    result.light = float4(0,0,0,0);
    
    //float4 pl = light_sources_buf.point_light;
    //result.light += CalculatePointLight(pl.xyz, positions.xyz, normals.xyz);
    //result.light += CalculatePointLight(pl.xyz - float3(0, 400, 0), positions.xyz, normals.xyz);
   
    for (int i = 0; i < 110; ++i)
    {
        if (i % 3 == 0)
            result.light += CalculatePointLight(light_sources_buf.source[i].xyz, positions.xyz, normals.xyz) * float4(1, 0, 0, 0);
        if (i % 3 == 1)
            result.light += CalculatePointLight(light_sources_buf.source[i].xyz, positions.xyz, normals.xyz) * float4(0, 1, 0, 0);
        if (i % 3 == 2)
            result.light += CalculatePointLight(light_sources_buf.source[i].xyz, positions.xyz, normals.xyz) * float4(0, 0, 1, 0);
    }
    //diffuse += max(dot(normals.xyz, normalize(light.direction)), 0.0f);
    //diffuse *= albedo.rgb * 5;
    //result.light = float4(diffuse * 0.5, 1);
    
    result.light *= albedo;

#endif

        return result;
}