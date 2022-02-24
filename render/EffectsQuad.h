#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class EffectsQuad {
public:
	EffectsQuad();
	EffectsQuad(ConstantBufferEffects& cbe);

	~EffectsQuad();

	void DrawStatic();

	void CreateCB(const ConstantBufferEffects& cbe);

	void UpdateCB(ConstantBufferEffects& cbe);

public:
	static VertexBuffer vertex_buffer;
	static IndexBuffer  index_buffer;

private:
	ConstantBuffer<ConstantBufferEffects> constant_buffer;
};

