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
	Quad(ConstantBufferQuad& cbq, int flags, int tex_index = -1, int lm_index = -1);
	Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib,
		 int flags, int tex_index = -1, int lm_index = -1);	
	Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb,
		 int flags, int tex_index = -1, int lm_index = -1);

	~Quad();

	int GetFlags();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferQuad> cb;
};

