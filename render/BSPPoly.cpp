#include <BSPPoly.h>

VertexBuffer BSPPoly::vb;
IndexBuffer  BSPPoly::ib;

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb)
	: Primitive(true), cb(cb) {
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
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
