#include <ModPoly.h>

ModPoly::ModPoly() 
	: constant_buffer() {
}

ModPoly::ModPoly(ConstantBuffer<MODEL>& cbp)
	: constant_buffer(cbp) {
}

ModPoly::ModPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb)
	: constant_buffer(cbp), vertex_buffer(vb) {
}

ModPoly::ModPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb, IndexBuffer& ib)
	: constant_buffer(cbp), vertex_buffer(vb), index_buffer(ib) {
}

ModPoly::~ModPoly() {
}

void ModPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void ModPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void ModPoly::CreateCB(const MODEL& cbp) {
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

void ModPoly::UpdateCB(const MODEL& cbp)
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
