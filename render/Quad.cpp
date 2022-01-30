#include <Quad.h>

VertexBuffer Quad::vb;
IndexBuffer  Quad::ib;

Quad::Quad(ConstantBuffer<ConstantBufferQuad>& cb, int flags, int index) 
	: cb(cb), flags(flags), texture_index(index) {
}

int Quad::GetFlags() {
	return flags;
}

void Quad::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->Bind(texture_index);

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(6u, 0u, 0u);
}