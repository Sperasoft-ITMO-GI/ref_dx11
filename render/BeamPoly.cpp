#include <BeamPoly.h>

VertexBuffer BeamPoly::vb;
IndexBuffer  BeamPoly::ib;

BeamPoly::BeamPoly() : Primitive(true), cb()
{
}

BeamPoly::BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cb)
	: Primitive(true), cb(cb) {
}

BeamPoly::BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib)
	: Primitive(vb, ib, true), cb(cb) {
}

BeamPoly::~BeamPoly()
{
}

void BeamPoly::Draw()
{
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(dynamic_vb.GetCount(), 0u);
}

void BeamPoly::DrawIndexed()
{
	Renderer* renderer = Renderer::GetInstance();

	dynamic_vb.Bind();
	dynamic_ib.Bind();
	cb.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(dynamic_ib.GetCount(), 0u, 0u);
}

void BeamPoly::DrawStatic()
{
	Renderer* renderer = Renderer::GetInstance();

	vb.Bind();
	ib.Bind();
	cb.Bind<ConstantBufferQuad>();

	renderer->GetContext()->DrawIndexed(ib.GetCount(), 0u, 0u);
}

void BeamPoly::CreateCB(const ConstantBufferPolygon& cbp)
{
	cb.Create(cbp);
}

void BeamPoly::CreateDynamicVB(UINT size)
{
	dynamic_vb.CreateDynamic<BeamVertex>(size);
}

void BeamPoly::CreateDynamicIB(UINT size)
{
	dynamic_ib.CreateDynamic(size);
}

void BeamPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	cb.Update(cbp);
}

void BeamPoly::UpdateDynamicVB(std::vector<BeamVertex> vertexes)
{
	dynamic_vb.Update(vertexes);
}

void BeamPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	dynamic_ib.Update(indexes);
}
