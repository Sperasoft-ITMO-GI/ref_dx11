#include <Quad.h>

VertexBuffer Quad::vertex_buffer;
IndexBuffer  Quad::index_buffer;

Quad::Quad() : constant_buffer()
{
}

Quad::Quad(UI_BUFFER& cbq)
	: constant_buffer(cbq) {
}

Quad::~Quad() {
}

void Quad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<UI_BUFFER>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void Quad::CreateCB(const UI_BUFFER& cbq)
{
	constant_buffer.Create(cbq);
}

void Quad::UpdateCB(UI_BUFFER& cbq) {
	constant_buffer.Update(cbq);
}

