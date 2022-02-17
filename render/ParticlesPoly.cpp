#include <ParticlesPoly.h>

VertexBuffer ParticlesPoly::vb;
IndexBuffer  ParticlesPoly::ib;

ParticlesPoly::ParticlesPoly() : Primitive(true), cb()
{
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<ConstantBufferPolygon>& cb)
	: Primitive(true), cb(cb) {
}

ParticlesPoly::ParticlesPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

ParticlesPoly::~ParticlesPoly() {

}

void ParticlesPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferPolygon>();
	ID3D11Buffer* buffer = cb.GetBuffer();
	renderer->GetContext()->GSSetConstantBuffers(0u, 1u, &buffer);

	renderer->GetContext()->Draw(dynamic_vb.GetCount(), 0u);
}

void ParticlesPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(dynamic_ib.GetCount(), 0u, 0u);
}

void ParticlesPoly::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void ParticlesPoly::CreateCB(const ConstantBufferPolygon& cbp) {
	cb.Create(cbp);
}

void ParticlesPoly::CreateDynamicVB(UINT size)
{
	dynamic_vb.CreateDynamic<ParticlesVertex>(size);
}

void ParticlesPoly::CreateDynamicIB(UINT size)
{
	dynamic_ib.CreateDynamic(size);
}

void ParticlesPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	cb.Update(cbp);
}

void ParticlesPoly::UpdateDynamicVB(std::vector<ParticlesVertex> vertexes)
{
	dynamic_vb.Update(vertexes);
}

void ParticlesPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	dynamic_ib.Update(indexes);
}
