#include <SkyPoly.h>

VertexBuffer SkyPoly::vb;
IndexBuffer  SkyPoly::ib;

SkyPoly::SkyPoly() : Primitive(true), cb()
{
}

SkyPoly::SkyPoly(ConstantBufferSkyBox& cbq)
	: Primitive(true), cb(cbq) {
}

SkyPoly::SkyPoly(ConstantBuffer<ConstantBufferSkyBox>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

SkyPoly::SkyPoly(ConstantBuffer<ConstantBufferSkyBox>& cb, VertexBuffer& vb)
	: Primitive(vb, ib, false), cb(cb) {
}

SkyPoly::~SkyPoly() {
	//cb.~ConstantBuffer();
}

void SkyPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}

void SkyPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void SkyPoly::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void SkyPoly::CreateCB(const ConstantBufferSkyBox& cbq)
{
	cb.Create(cbq);
}

void SkyPoly::CreateDynamicVB(UINT size)
{
	dynamic_vb.CreateDynamic<SkyVertex>(size);
}

void SkyPoly::CreateDynamicIB(UINT size)
{
	dynamic_ib.CreateDynamic(size);
}

void SkyPoly::UpdateCB(ConstantBufferSkyBox& cbq) {
	cb.Update(cbq);
}

void SkyPoly::UpdateDynamicVB(std::vector<SkyVertex> vertexes)
{
	dynamic_vb.Update(vertexes);
}

void SkyPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	dynamic_ib.Update(indexes);
}
