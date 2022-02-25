#include <ModPoly.h>

VertexBuffer ModPoly::vb;
IndexBuffer  ModPoly::ib;

ModPoly::ModPoly() : Primitive(true), cb()
{
}

ModPoly::ModPoly(ConstantBuffer<ConstantBufferPolygon>& cb)
	: Primitive(true), cb(cb) {
}

ModPoly::ModPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

ModPoly::~ModPoly() {

}

void ModPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(dynamic_vb.GetCount(), 0u);
}

void ModPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(dynamic_ib.GetCount(), 0u, 0u);
}

void ModPoly::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void ModPoly::CreateCB(const ConstantBufferPolygon& cbp) {
	cb.Create(cbp);
}

void ModPoly::CreateDynamicVB(UINT size)
{
	dynamic_vb.CreateDynamic<ModVertex>(size);
}

void ModPoly::CreateDynamicIB(UINT size)
{
	dynamic_ib.CreateDynamic(size);
}

void ModPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	cb.Update(cbp);
}

void ModPoly::UpdateDynamicVB(std::vector<ModVertex> vertexes)
{
	dynamic_vb.Update(vertexes);
}

void ModPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	dynamic_ib.Update(indexes);
}
