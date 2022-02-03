#include <Quad.h>

VertexBuffer Quad::vb;
IndexBuffer  Quad::ib;

Quad::Quad(ConstantBuffer<ConstantBufferQuad>& cb, int flags, int tex_index, int lm_index) 
	: Primitive(flags, tex_index, lm_index, true), cb(cb) {
}

Quad::Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib, 
	       int flags, int tex_index, int lm_index)
	: Primitive(vb, ib, flags, tex_index, lm_index, true), cb(cb) {
}

Quad::Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, 
	       int flags, int tex_index, int lm_index) 
	: Primitive(vb, ib, flags, tex_index, lm_index, false), cb(cb) {
}

int Quad::GetFlags() {
	return flags;
}

void Quad::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->Bind(texture_index);

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}

void Quad::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->Bind(texture_index);

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void Quad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->Bind(texture_index);

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}
