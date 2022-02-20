#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class EffectsQuad : public Primitive {
public:
	EffectsQuad();
	EffectsQuad(ConstantBufferEffects& cbsb);
	EffectsQuad(ConstantBuffer<ConstantBufferEffects>& cb, VertexBuffer& vb, IndexBuffer& ib);
	EffectsQuad(ConstantBuffer<ConstantBufferEffects>& cb, VertexBuffer& vb);

	~EffectsQuad();

	void Draw();

	void DrawIndexed();

	void DrawStatic();


	void CreateCB(const ConstantBufferEffects& cbsb);
	void CreateDynamicVB(UINT size);
	void CreateDynamicIB(UINT size);
	void UpdateCB(ConstantBufferEffects& cbsb);
	void UpdateDynamicVB(std::vector<EffectsVertex> vertexes);
	void UpdateDynamicIB(std::vector<uint16_t> indexes);

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferEffects> cb;
};

