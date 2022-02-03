#include <BSPPoly.h>

VertexBuffer BSPPoly::vb;
IndexBuffer  BSPPoly::ib;

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, int flags, int tex_index, int lm_index)
	: Primitive(flags, tex_index, lm_index, true), cb(cb) {
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib,
	int flags, int tex_index, int lm_index)
	: Primitive(vb, ib, flags, tex_index, lm_index, true), cb(cb) {
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb,
	int flags, int tex_index, int lm_index)
	: Primitive(vb, ib, flags, tex_index, lm_index, false), cb(cb) {
}

int BSPPoly::GetFlags() {
	return flags;
}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	// Здесь нужно по идее биндить индекс текстуры и лайтмапы
	renderer->Bind(texture_index);

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}

void BSPPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	// Здесь нужно по идее биндить индекс текстуры и лайтмапы
	renderer->Bind(texture_index);

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(dynamic_ib.GetCount(), 0u, 0u);
}

void BSPPoly::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->Bind(texture_index);

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}
