#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class ModPoly : public Primitive {
public:
	ModPoly();
	ModPoly(ConstantBuffer<ConstantBufferPolygon>& cb);
	ModPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib);
	ModPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb);

	~ModPoly();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

	void CreateCB(const ConstantBufferPolygon& cbp);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);
	void UpdateCB(const ConstantBufferPolygon& cbp);
	void UpdateDynamicVB(std::vector<ModVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

private:
	static VertexBuffer vb;
	static IndexBuffer ib;

private:
	ConstantBuffer<ConstantBufferPolygon> cb;
};