#include <SkyQuad.h>

SkyQuad::SkyQuad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib,
	             int flags, int index)
	: cb(cb), vb(vb), ib(ib), flags(flags), texture_index(index) {
}

int SkyQuad::GetFlags() {
	return flags;
}

void SkyQuad::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->Bind(texture_index);

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(6u, 0u, 0u);
}