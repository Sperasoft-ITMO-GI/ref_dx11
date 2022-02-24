#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class Quad  {
public:

	Quad(ConstantBufferQuad& cbq);

	~Quad();

	void DrawStatic();

	void CreateCB(const ConstantBufferQuad& cbq);

	void UpdateCB(ConstantBufferQuad& cbq);

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferQuad> cb;
};

