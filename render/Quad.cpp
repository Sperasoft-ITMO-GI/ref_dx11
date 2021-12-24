#include <Quad.h>

using std::vector;
using DirectX::XMFLOAT2;
using DirectX::XMFLOAT4;

static vector<uint16_t> indexes{
	0, 2, 3,
	3, 1, 0
};

Quad::Quad(VertexBuffer vb, IndexBuffer ib, ConstantBuffer<ConstantBufferQuad> cb, int flags, int index) 
	: vb(vb), ib(ib), cb(cb), flags(flags), texture_index(index) {
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