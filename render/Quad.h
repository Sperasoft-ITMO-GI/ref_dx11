#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class Quad  {
public:
	Quad();
	Quad(ConstantBufferQuad& cbq);

	~Quad();

	void DrawStatic();

	void CreateCB(const ConstantBufferQuad& cbq);

	void UpdateCB(ConstantBufferQuad& cbq);

public:
	static VertexBuffer vertex_buffer;
	static IndexBuffer  index_buffer;

private:
	ConstantBuffer<ConstantBufferQuad> constant_buffer;
};

