#include <ParticlesPoly.h>

ParticlesPoly::ParticlesPoly() : constant_buffer()
{
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cbp)
	: constant_buffer(cbp) {
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<ConstantBufferParticles>& cbp, VertexBuffer& vb, IndexBuffer& ib)
	: constant_buffer(cbp), vertex_buffer(vb), index_buffer(ib) {
}

ParticlesPoly::~ParticlesPoly() {

}

void ParticlesPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<ConstantBufferParticles>();

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void ParticlesPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferParticles>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void ParticlesPoly::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void ParticlesPoly::CreateCB(const ConstantBufferParticles& cbp) {
	constant_buffer.Create(cbp);
}

void ParticlesPoly::CreateDynamicVB(UINT size)
{
	vertex_buffer.CreateDynamic<ParticlesVertex>(size);
}

void ParticlesPoly::CreateDynamicIB(UINT size)
{
	index_buffer.CreateDynamic(size);
}

void ParticlesPoly::UpdateCB(const ConstantBufferParticles& cbp)
{
	constant_buffer.Update(cbp);
}

void ParticlesPoly::UpdateDynamicVB(std::vector<ParticlesVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void ParticlesPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	index_buffer.Update(indexes);
}
