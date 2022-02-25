#pragma once 

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class BSPPoly {
public:
	BSPPoly();
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cbp);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb);
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~BSPPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const ConstantBufferPolygon& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const ConstantBufferPolygon& cbp);
	void UpdateDynamicVB(std::vector<BSPVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;
	ConstantBuffer<ConstantBufferPolygon> constant_buffer;
};