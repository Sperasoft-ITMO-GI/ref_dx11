#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class BSPPoly : public Primitive {
public:
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, int flags, int tex_index = -1, int lm_index = -1);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib,
		int flags, int tex_index = -1, int lm_index = -1);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb,
		int flags, int tex_index = -1, int lm_index = -1);

	int GetFlags();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

private:
	static VertexBuffer vb;
	static IndexBuffer ib;

private:
	ConstantBuffer<ConstantBufferPolygon> cb;
};