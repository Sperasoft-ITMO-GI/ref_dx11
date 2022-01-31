#include <BSPPoly.h>

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib,
	             int flags, int tex_index, int lm_index)
	: cb(cb), vb(vb), ib(ib), flags(flags), texture_index(tex_index), lightmap_index(lm_index) {
}

int BSPPoly::GetFlags() {
	return flags;
}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	// Здесь нужно по идее биндить индекс текстуры и лайтмапы
	renderer->Bind(texture_index);

	vb.Bind();
	//ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	//renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}
