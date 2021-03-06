#ifdef __cplusplus
#pragma once

#include <DirectXMath.h>

#define float4x4 DirectX::XMMATRIX
#define float4 DirectX::XMFLOAT4
#define float3 DirectX::XMFLOAT3
#define float2 DirectX::XMFLOAT2

struct CONST_HANDLE
{
	CONST_HANDLE(int slot) { this->slot = slot; }
	int slot;
};

struct TEXTURE_HANDLE
{
	TEXTURE_HANDLE(int slot) { this->slot = slot; }
	int slot;
};

struct SAMPLER_HANDLE
{
	SAMPLER_HANDLE(int slot) { this->slot = slot; }
	int slot;
};

struct TEXTURE_CUBE_HANDLE
{
	TEXTURE_CUBE_HANDLE(int slot) { this->slot = slot; }
	int slot;
};

#endif

#ifdef HLSL
#define STRUCTURE(slot, typeName, name) cbuffer __buffer##slot : register(b##slot) { typeName name : packoffset(c0); }
#else
#define STRUCTURE(slot, typeName, name) const CONST_HANDLE name(slot);
#endif

#ifdef HLSL
#define TEXTURE_2D(slot, type, name) Texture2D<type> name : register(t##slot);
#else
#define TEXTURE_2D(slot, type, name) const TEXTURE_HANDLE name(slot);
#endif

#ifdef HLSL
#define SAMPLER(slot, name) sampler name : register(s##slot);
#else
#define SAMPLER(slot, name) const SAMPLER_HANDLE name(slot);
#endif

#ifdef HLSL
// <type> need in this case???
#define TEXTURE_CUBE(slot, type, name) TextureCube name : register(t##slot);
#else
#define TEXTURE_CUBE(slot, type, name) const TEXTURE_CUBE_HANDLE name(slot);
#endif

struct CAMERA
{
	float4x4 perspective;
	float4x4 orthogonal;
	float4x4 view;
	float4x4 prev_view;
	float4x4 view_projection_inverse;
	float fps;
	unsigned int total_frames;
	float2 resolution;
};

// TEMPORABLE
struct UI_BUFFER
{
	float4x4 position_transform;
	float4x4 texture_transform;
	float4 color;
};

struct MODEL
{
	float4x4 mod;
	float4 color;
};

struct DirectionalLight
{
	float4 color;
	float3 direction;
	float  intensity;
};

struct LightSources
{
	//float4 point_light;
	//unsigned int sources_count;
	float4 source[128];
	//unsigned int dummy[2];
	//float4 color;
};

struct MatrixBuffer
{
	float4x4 orthogonal;
};

STRUCTURE(0, CAMERA, camera)
STRUCTURE(1, UI_BUFFER, ui_buffer)
STRUCTURE(1, MODEL, model)
STRUCTURE(2, DirectionalLight, light)
STRUCTURE(3, MatrixBuffer, buffer)
STRUCTURE(4, LightSources, light_sources_buf)
TEXTURE_2D(0, float4, colorTexture)
TEXTURE_2D(1, float4, lightmapTexture)
TEXTURE_2D(2, float4, positionTexture)
TEXTURE_2D(3, float4, normalTexture)
TEXTURE_2D(4, float4, albedoTexture)
TEXTURE_2D(2, float4, bloomTexture)
TEXTURE_2D(3, float4, effectTexture)
TEXTURE_2D(4, float4, fxaaTexture)
TEXTURE_2D(5, float, depthTexture)
TEXTURE_2D(6, float, prevdepthTexture)
TEXTURE_2D(7, float2, velocityTexture)
TEXTURE_2D(8, float2, prevvelocityTexture)
TEXTURE_2D(9, float4, prevcolorTexture)
TEXTURE_CUBE(0, float4, skyboxTexture)
SAMPLER(0, Sampler)
