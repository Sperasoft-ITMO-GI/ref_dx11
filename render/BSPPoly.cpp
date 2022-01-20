#include <BSPPoly.h>

VertexBuffer BSPPoly::vb;
IndexBuffer  BSPPoly::ib;

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon> cb, int flags, int tex_index, int lm_index)
	: cb(cb), flags(flags), texture_index(tex_index), lightmap_index(lm_index) {
}

int BSPPoly::GetFlags() {
	return flags;
}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	// ����� ����� �� ���� ������� ������ �������� � ��������
	//renderer->Bind()

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(3u, 0u, 0u);
}
