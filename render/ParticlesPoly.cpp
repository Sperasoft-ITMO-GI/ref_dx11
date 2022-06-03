#include <ParticlesPoly.h>

ParticlesPoly::ParticlesPoly() : constant_buffer()
{
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<MODEL>& cbp)
	: constant_buffer(cbp) {
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb)
	: constant_buffer(cbp), vertex_buffer(vb) {
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib)
	: constant_buffer(cbp), vertex_buffer(vb), index_buffer(ib) {
}

ParticlesPoly::~ParticlesPoly() {

}

void ParticlesPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void ParticlesPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void ParticlesPoly::CreateCB(const MODEL& cbp) {
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

void ParticlesPoly::UpdateCB(const MODEL& cbp)
{
	constant_buffer.Update(cbp);
}

void ParticlesPoly::UpdateDynamicVB(std::vector<ParticlesVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void ParticlesPoly::UpdateDynamicIB(std::vector<UINT> indexes)
{
	index_buffer.Update(indexes);
}
