#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class SkyPoly : public Primitive {
public:
	SkyPoly();
	SkyPoly(ConstantBufferSkyBox& cbsb);
	SkyPoly(ConstantBuffer<ConstantBufferSkyBox>& cb, VertexBuffer& vb, IndexBuffer& ib);
	SkyPoly(ConstantBuffer<ConstantBufferSkyBox>& cb, VertexBuffer& vb);

	~SkyPoly();

	void Draw();

	void DrawIndexed();

	void DrawStatic();


	void CreateCB(const ConstantBufferSkyBox& cbsb);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);
	void UpdateCB(ConstantBufferSkyBox& cbsb);
	void UpdateDynamicVB(std::vector<SkyVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferSkyBox> cb;
};

