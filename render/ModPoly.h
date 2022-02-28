#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class ModPoly {
public:
	ModPoly();
	ModPoly(ConstantBuffer<MODEL>& cbp);
	ModPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb);
	ModPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~ModPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const MODEL& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const MODEL& cbp);
	void UpdateDynamicVB(std::vector<ModVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;
	ConstantBuffer<MODEL> constant_buffer;
};