#pragma once 

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class BeamPoly {
public:
	BeamPoly();
	BeamPoly(ConstantBuffer<MODEL>& cbp);
	BeamPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb);
	BeamPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~BeamPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const MODEL& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const MODEL& cbp);
	void UpdateDynamicVB(std::vector<BeamVertex> vertexes);
	void UpdateDynamicIB(std::vector<UINT> indexes);

public:
	VertexBuffer vertex_buffer;
	IndexBuffer index_buffer;

private:
	ConstantBuffer<MODEL> constant_buffer;
};