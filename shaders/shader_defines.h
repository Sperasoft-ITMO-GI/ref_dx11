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
#define SAMPLER(slot, name) const TEXTURE_HANDLE name(slot);
#endif

struct UI_BUFFER
{
	float4x4 position_transform;
	float4x4 texture_transform;
	float4 color;
};

STRUCTURE(0, UI_BUFFER, ui_buffer)
TEXTURE_2D(0, float4, colorTexture)
SAMPLER(0, Sampler)
