#include <EffectsQuad.h>

VertexBuffer EffectsQuad::vb;
IndexBuffer  EffectsQuad::ib;

EffectsQuad::EffectsQuad() : Primitive(true), cb()
{
}

EffectsQuad::EffectsQuad(ConstantBufferEffects& cbq)
	: Primitive(true), cb(cbq) {
}

EffectsQuad::EffectsQuad(ConstantBuffer<ConstantBufferEffects>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

EffectsQuad::EffectsQuad(ConstantBuffer<ConstantBufferEffects>& cb, VertexBuffer& vb)
	: Primitive(vb, ib, false), cb(cb) {
}

EffectsQuad::~EffectsQuad() {
	//cb.~ConstantBuffer();
}

void EffectsQuad::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->Draw(dynamic_vb.GetCount(), 0u);
}

void EffectsQuad::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferEffects>();

	renderer->GetContext()->DrawIndexed(dynamic_ib.GetCount(), 0u, 0u);
}

void EffectsQuad::DrawStatic() {
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferEffects>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void EffectsQuad::CreateCB(const ConstantBufferEffects& cbq)
{
	cb.Create(cbq);
}

void EffectsQuad::CreateDynamicVB(UINT size)
{
	dynamic_vb.CreateDynamic<EffectsVertex>(size);
}

void EffectsQuad::CreateDynamicIB(UINT size)
{
	dynamic_ib.CreateDynamic(size);
}

void EffectsQuad::UpdateCB(ConstantBufferEffects& cbq) {
	cb.Update(cbq);
}

void EffectsQuad::UpdateDynamicVB(std::vector<EffectsVertex> vertexes)
{
	dynamic_vb.Update(vertexes);
}

void EffectsQuad::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	dynamic_ib.Update(indexes);
}
