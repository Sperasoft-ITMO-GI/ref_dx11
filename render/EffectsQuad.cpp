#include <EffectsQuad.h>

VertexBuffer EffectsQuad::vertex_buffer;
IndexBuffer  EffectsQuad::index_buffer;

EffectsQuad::EffectsQuad() : constant_buffer()
{
}

EffectsQuad::EffectsQuad(ConstantBufferEffects& cbe)
	: constant_buffer(cbe) {
}

EffectsQuad::~EffectsQuad() {
}

void EffectsQuad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferEffects>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void EffectsQuad::CreateCB(const ConstantBufferEffects& cbe)
{
	constant_buffer.Create(cbe);
}

void EffectsQuad::UpdateCB(ConstantBufferEffects& cbe) {
	constant_buffer.Update(cbe);
}

