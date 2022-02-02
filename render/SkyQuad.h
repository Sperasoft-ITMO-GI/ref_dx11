#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.h>

class SkyQuad {
public:
	SkyQuad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib,
		     int flags, int index);

	int GetFlags();

	void Draw();

private:
	ConstantBuffer<ConstantBufferQuad> cb;
	VertexBuffer vb;
	IndexBuffer  ib;

private:
	int flags;
	int texture_index;
};

