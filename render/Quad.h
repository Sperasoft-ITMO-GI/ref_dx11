#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.h>
#include <UIVertex.h>

class Quad {
public:
	Quad(VertexBuffer vb, IndexBuffer ib, ConstantBuffer<ConstantBufferQuad> cb, int flags, int index);

	int GetFlags();

	void Draw();
private:
	int flags;
	int texture_index;
	VertexBuffer vb;
	IndexBuffer ib;
	ConstantBuffer<ConstantBufferQuad> cb;
	//Texture???
};

