#include <BSPPoly.h>

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, int flags, int tex_index, int lm_index)
	: cb(cb), vb(vb), flags(flags), texture_index(tex_index), lightmap_index(lm_index) {
}

int BSPPoly::GetFlags() {
	return flags;
}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	// ����� ����� �� ���� ������� ������ �������� � ��������
	renderer->Bind(texture_index);

	vb.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(vb.GetCount(), 0u);
}
