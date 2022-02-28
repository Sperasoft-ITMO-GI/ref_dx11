#include <EffectsQuad.h>

VertexBuffer EffectsQuad::vertex_buffer;
IndexBuffer  EffectsQuad::index_buffer;

EffectsQuad::EffectsQuad() : constant_buffer()
{
}

EffectsQuad::EffectsQuad(MODEL& cbe)
	: constant_buffer(cbe) {
}

EffectsQuad::~EffectsQuad() {
}

void EffectsQuad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void EffectsQuad::CreateCB(const MODEL& cbe)
{
	constant_buffer.Create(cbe);
}

void EffectsQuad::UpdateCB(MODEL& cbe) {
	constant_buffer.Update(cbe);
}

