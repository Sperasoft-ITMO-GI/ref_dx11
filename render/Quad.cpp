#include <Quad.h>

VertexBuffer Quad::vb;
IndexBuffer  Quad::ib;

Quad::Quad(ConstantBufferQuad& cbq) 
	: cb(cbq) {
}

Quad::~Quad() {
}

void Quad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void Quad::CreateCB(const ConstantBufferQuad& cbq)
{
	cb.Create(cbq);
}

void Quad::UpdateCB(ConstantBufferQuad& cbq) {
	cb.Update(cbq);
}

