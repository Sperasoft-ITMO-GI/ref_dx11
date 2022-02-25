#pragma once 

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class BeamPoly {
public:
	BeamPoly();
	BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cbp);
	BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb);
	BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~BeamPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const ConstantBufferPolygon& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const ConstantBufferPolygon& cbp);
	void UpdateDynamicVB(std::vector<BeamVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

public:
	VertexBuffer vertex_buffer;
	IndexBuffer index_buffer;

private:
	ConstantBuffer<ConstantBufferPolygon> constant_buffer;
};