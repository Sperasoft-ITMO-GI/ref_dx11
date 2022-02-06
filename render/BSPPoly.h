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
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb);

	void Draw();

	void DrawIndexed();

	void DrawStatic();

private:
	static VertexBuffer vb;
	static IndexBuffer ib;

private:
	ConstantBuffer<ConstantBufferPolygon> cb;
};