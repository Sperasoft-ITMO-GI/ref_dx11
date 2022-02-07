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
	BSPPoly();
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb);

	~BSPPoly();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

	void CreateCB(const ConstantBufferPolygon& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);
	void UpdateCB(const ConstantBufferPolygon& cbp);
	void UpdateDynamicVB(std::vector<BSPVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	static VertexBuffer vb;
	static IndexBuffer ib;

private:
	ConstantBuffer<ConstantBufferPolygon> cb;
};