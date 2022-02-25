#include <BSPPoly.h>

BSPPoly::BSPPoly() : constant_buffer()
{
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cbp)
	: constant_buffer(cbp) {
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vertex_buffer, IndexBuffer& ib)
	: constant_buffer(cbp) {
}

BSPPoly::BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cbp, VertexBuffer& vb) 
	: constant_buffer(cbp), vertex_buffer(vb) {
}

BSPPoly::~BSPPoly() {

}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void BSPPoly::DrawIndexed() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<ConstantBufferPolygon>();

	renderer->GetContext()->DrawIndexed(index_buffer.GetCount(), 0u, 0u);
}

void BSPPoly::CreateCB(const ConstantBufferPolygon& cbp) {
	constant_buffer.Create(cbp);
}

void BSPPoly::CreateDynamicVB(UINT size)
{
	vertex_buffer.CreateDynamic<BSPVertex>(size);
}

void BSPPoly::CreateDynamicIB(UINT size)
{
	index_buffer.CreateDynamic(size);
}

void BSPPoly::UpdateCB(const ConstantBufferPolygon& cbp)
{
	constant_buffer.Update(cbp);
}

void BSPPoly::UpdateDynamicVB(std::vector<BSPVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void BSPPoly::UpdateDynamicIB(std::vector<uint16_t> indexes)
{
	index_buffer.Update(indexes);
}
