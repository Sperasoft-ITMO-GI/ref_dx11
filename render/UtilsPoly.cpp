#include <UtilsPoly.h>

UtilsPoly::UtilsPoly() : constant_buffer()
{
}

UtilsPoly::UtilsPoly(ConstantBuffer<MODEL>& cbp)
	: constant_buffer(cbp) {
}

UtilsPoly::UtilsPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib)
	: constant_buffer(cbp), vertex_buffer(vb), index_buffer(ib) {
}

UtilsPoly::UtilsPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb)
	: constant_buffer(cbp), vertex_buffer(vb) {
}

UtilsPoly::~UtilsPoly()
{
}

void UtilsPoly::Draw()
{
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void UtilsPoly::DrawIndexed()
{
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void UtilsPoly::CreateCB(const MODEL& cbp)
{
	constant_buffer.Create(cbp);
}

void UtilsPoly::CreateDynamicVB(UINT size)
{
	vertex_buffer.CreateDynamic<UtilsVertex>(size);
}

void UtilsPoly::CreateDynamicIB(UINT size)
{
	index_buffer.CreateDynamic(size);
}

void UtilsPoly::UpdateCB(const MODEL& cbp)
{
	constant_buffer.Update(cbp);
}

void UtilsPoly::UpdateDynamicVB(std::vector<UtilsVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void UtilsPoly::UpdateDynamicIB(std::vector<UINT> indexes)
{
	index_buffer.Update(indexes);
}
