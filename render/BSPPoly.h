#pragma once 

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class BSPPoly {
public:
	BSPPoly();
	BSPPoly(ConstantBuffer<MODEL>& cbp);
	BSPPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb);
	BSPPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~BSPPoly();

	void Draw();
	void DrawIndexed(UINT indexCount = 0, const UINT startIndexLocation = 0U, const INT baseVertexLocation = 0);

	void CreateCB(const MODEL& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const MODEL& cbp);
	void UpdateDynamicVB(std::vector<BSPVertex> vertexes);
	void UpdateDynamicIB(std::vector<UINT> indexes);

	void Bind();

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;
	ConstantBuffer<MODEL> constant_buffer;
};