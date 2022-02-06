#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class Quad : public Primitive {
public:

	Quad(ConstantBufferQuad& cbq);
	Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib);	
	Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb);

	~Quad();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

	void UpdateCB(ConstantBufferQuad& cbq);

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferQuad> cb;
};

