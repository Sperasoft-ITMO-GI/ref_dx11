#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class ParticlesPoly {
public:
	ParticlesPoly();
	ParticlesPoly(ConstantBuffer<MODEL>& cbp);
	ParticlesPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb);
	ParticlesPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~ParticlesPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const MODEL& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const MODEL& cbp);
	void UpdateDynamicVB(std::vector<ParticlesVertex> vertexes);
	void UpdateDynamicIB(std::vector<UINT> indexes);

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;
	ConstantBuffer<MODEL> constant_buffer;
};