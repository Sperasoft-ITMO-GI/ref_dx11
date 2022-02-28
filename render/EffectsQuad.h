#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class EffectsQuad {
public:
	EffectsQuad();
	EffectsQuad(MODEL& cbe);

	~EffectsQuad();

	void DrawStatic();

	void CreateCB(const MODEL& cbe);
	void UpdateCB(MODEL& cbe);

public:
	static VertexBuffer vertex_buffer;
	static IndexBuffer  index_buffer;

private:
	ConstantBuffer<MODEL> constant_buffer;
};

