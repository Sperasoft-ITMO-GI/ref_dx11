#include <Quad.h>

VertexBuffer Quad::vb;
IndexBuffer  Quad::ib;

Quad::Quad(ConstantBufferQuad& cbq) 
	: Primitive(true), cb(cbq) {
}

Quad::Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

Quad::Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb) 
	: Primitive(vb, ib, false), cb(cb) {
}

Quad::~Quad() {
	//cb.~ConstantBuffer();
}

void Quad::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}

void Quad::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void Quad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void Quad::UpdateCB(ConstantBufferQuad& cbq) {
	cb.Update(cbq);
}
