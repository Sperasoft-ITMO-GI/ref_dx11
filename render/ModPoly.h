#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class ModPoly {
public:
	ModPoly();
	ModPoly(ConstantBuffer<ConstantBufferPolygon>& cbp);
	ModPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb);
	ModPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~ModPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const ConstantBufferPolygon& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const ConstantBufferPolygon& cbp);
	void UpdateDynamicVB(std::vector<ModVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;
	ConstantBuffer<ConstantBufferPolygon> constant_buffer;
};