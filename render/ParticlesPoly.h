#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class ParticlesPoly {
public:
	ParticlesPoly();
	ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cb);
	ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cb, VertexBuffer& vb, IndexBuffer& ib);
	ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cb, VertexBuffer& vb);

	~ParticlesPoly();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

	void CreateCB(const ConstantBufferParticles& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);
	void UpdateCB(const ConstantBufferParticles& cbp);
	void UpdateDynamicVB(std::vector<ParticlesVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	VertexBuffer vertex_buffer;
	IndexBuffer  index_buffer;

private:
	ConstantBuffer<ConstantBufferParticles> constant_buffer;
};