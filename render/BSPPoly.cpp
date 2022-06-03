#include <BSPPoly.h>

BSPPoly::BSPPoly() : constant_buffer()
{
}

BSPPoly::BSPPoly(ConstantBuffer<MODEL>& cbp)
	: constant_buffer(cbp) {
}

BSPPoly::BSPPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vertex_buffer, IndexBuffer& ib)
	: constant_buffer(cbp) {
}

BSPPoly::BSPPoly(ConstantBuffer<MODEL>& cbp, VertexBuffer& vb)
	: constant_buffer(cbp), vertex_buffer(vb) {
}

BSPPoly::~BSPPoly() {

}

void BSPPoly::Draw() {
	Renderer* renderer = Renderer::GetInstance();

	vertex_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->Draw(vertex_buffer.GetCount(), 0u);
}

void BSPPoly::DrawIndexed(UINT indexCount, const UINT startIndexLocation, const INT baseVertexLocation) {
	Renderer* renderer = Renderer::GetInstance();

	if (indexCount == 0)
		indexCount = index_buffer.GetCount();
	vertex_buffer.Bind();
	index_buffer.Bind();
	constant_buffer.Bind<MODEL>(1);

	renderer->GetContext()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void BSPPoly::CreateCB(const MODEL& cbp) {
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

void BSPPoly::UpdateCB(const MODEL& cbp)
{
	constant_buffer.Update(cbp);
}

void BSPPoly::UpdateDynamicVB(std::vector<BSPVertex> vertexes)
{
	vertex_buffer.Update(vertexes);
}

void BSPPoly::UpdateDynamicIB(std::vector<UINT> indexes)
{
	index_buffer.Update(indexes);
}

void BSPPoly::Bind()
{
	vertex_buffer.Bind();
	index_buffer.Bind();
}
