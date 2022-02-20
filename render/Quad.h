#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class Quad : public Primitive {
public:

	Quad(ConstantBufferQuad& cbq);
	Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb, IndexBuffer& ib);	
	Quad(ConstantBuffer<ConstantBufferQuad>& cb, VertexBuffer& vb);

	~Quad();

	void Draw();

	void DrawIndexed();

	void DrawStatic();

	void CreateCB(const ConstantBufferQuad& cbq);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);
	void UpdateCB(ConstantBufferQuad& cbq);
	void UpdateDynamicVB(std::vector<SkyVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferQuad> cb;
};

