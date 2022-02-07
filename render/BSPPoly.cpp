#include <BSPPoly.h>

VertexBuffer BSPPoly::vb;
IndexBuffer  BSPPoly::ib;

BSPPoly::BSPPoly() : Primitive(true), cb()
{
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb)
	: Primitive(true), cb(cb) {
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

BSPPoly::~BSPPoly() {

}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}

void BSPPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(dynamic_ib.GetCount(), 0u, 0u);
}

void BSPPoly::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void BSPPoly::CreateCB(const ConstantBufferPolygon& cbp) {
	cb.Create(cbp);
}

void BSPPoly::CreateDynamicVB(UINT size)
{
	dynamic_vb.CreateDynamic<BSPVertex>(size);
}

void BSPPoly::CreateDynamicIB(UINT size)
{
	dynamic_ib.CreateDynamic(size);
}

void BSPPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	cb.Update(cbp);
}

void BSPPoly::UpdateDynamicVB(std::vector<BSPVertex> vertexes)
{
	dynamic_vb.Update(vertexes);
}

void BSPPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	dynamic_ib.Update(indexes);
}
