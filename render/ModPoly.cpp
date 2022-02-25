#include <ModPoly.h>

ModPoly::ModPoly() 
	: constant_buffer() {
}

ModPoly::ModPoly(ConstantBuffer<ConstantBufferPolygon>& cbp)
	: constant_buffer(cbp) {
}

ModPoly::ModPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb) 
	: constant_buffer(cbp), vertex_buffer(vb) {
}

ModPoly::ModPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb, IndexBuffer& ib)
	: constant_buffer(cbp), vertex_buffer(vb), index_buffer(ib) {
}

ModPoly::~ModPoly() {
}

void ModPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void ModPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void ModPoly::CreateCB(const ConstantBufferPolygon& cbp) {
	constant_buffer.Create(cbp);
}

void ModPoly::CreateDynamicVB(UINT size)
{
	vertex_buffer.CreateDynamic<ModVertex>(size);
}

void ModPoly::CreateDynamicIB(UINT size)
{
	index_buffer.CreateDynamic(size);
}

void ModPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	constant_buffer.Update(cbp);
}

void ModPoly::UpdateDynamicVB(std::vector<ModVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void ModPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	index_buffer.Update(indexes);
}
