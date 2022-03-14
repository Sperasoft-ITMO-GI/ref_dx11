#ifdef __cplusplus
#pragma once

#include <DirectXMath.h>

#define float4x4 DirectX::XMMATRIX
#define float4 DirectX::XMFLOAT4

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
	float4x4 weaponProj;
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

STRUCTURE(0, CAMERA, camera)
STRUCTURE(1, UI_BUFFER, ui_buffer)
STRUCTURE(1, MODEL, model)
TEXTURE_2D(0, float4, colorTexture)
TEXTURE_2D(1, float4, lightmapTexture)
TEXTURE_2D(2, float4, bloomTexture)
TEXTURE_2D(3, float4, effectTexture)
TEXTURE_2D(4, float4, fxaaTexture)
TEXTURE_2D(5, float, maskTexture)
TEXTURE_CUBE(0, float4, skyboxTexture)
SAMPLER(0, Sampler)
