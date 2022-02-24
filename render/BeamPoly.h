#pragma once 

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class BeamPoly {
public:
	BeamPoly();
	BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cb);
	BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib);
	BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb);

	~BeamPoly();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

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