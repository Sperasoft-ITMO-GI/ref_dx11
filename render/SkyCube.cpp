#include <SkyCube.h>

VertexBuffer SkyCube::vertex_buffer;
IndexBuffer  SkyCube::index_buffer;

SkyCube::SkyCube() : constant_buffer()
{
}

SkyCube::SkyCube(ConstantBufferSkyBox& cbq)
	: constant_buffer(cbq) {
}

SkyCube::~SkyCube() {
}

void SkyCube::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void SkyCube::CreateCB(const ConstantBufferSkyBox& cbq)
{
	constant_buffer.Create(cbq);
}

void SkyCube::UpdateCB(ConstantBufferSkyBox& cbq) {
	constant_buffer.Update(cbq);
}
