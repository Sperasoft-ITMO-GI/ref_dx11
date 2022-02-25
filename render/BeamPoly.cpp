#include <BeamPoly.h>

BeamPoly::BeamPoly() : constant_buffer()
{
}

BeamPoly::BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cbp)
	: constant_buffer(cbp) {
}

BeamPoly::BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb, IndexBuffer& ib)
	: constant_buffer(cbp), vertex_buffer(vb), index_buffer(ib) {
}

BeamPoly::BeamPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb) 
	: constant_buffer(cbp), vertex_buffer(vb) {
}

BeamPoly::~BeamPoly()
{
}

void BeamPoly::Draw()
{
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void BeamPoly::DrawIndexed()
{
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void BeamPoly::CreateCB(const ConstantBufferPolygon& cbp)
{
	constant_buffer.Create(cbp);
}

void BeamPoly::CreateDynamicVB(UINT size)
{
	vertex_buffer.CreateDynamic<BeamVertex>(size);
}

void BeamPoly::CreateDynamicIB(UINT size)
{
	index_buffer.CreateDynamic(size);
}

void BeamPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	constant_buffer.Update(cbp);
}

void BeamPoly::UpdateDynamicVB(std::vector<BeamVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void BeamPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	index_buffer.Update(indexes);
}
