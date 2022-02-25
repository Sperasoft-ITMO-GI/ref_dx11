#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class ParticlesPoly {
public:
	ParticlesPoly();
	ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cbp);
	ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cbp, VertexBuffer& vb);
	ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cbp, VertexBuffer& vb, IndexBuffer& ib);

	~ParticlesPoly();

	void Draw();
	void DrawIndexed();

	void CreateCB(const ConstantBufferParticles& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);

	void UpdateCB(const ConstantBufferParticles& cbp);
	void UpdateDynamicVB(std::vector<ParticlesVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;
	ConstantBuffer<ConstantBufferParticles> constant_buffer;
};