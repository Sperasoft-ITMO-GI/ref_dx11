#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>
#include <Primitive.h>

class SkyCube {
public:
	SkyCube();

	SkyCube(ConstantBufferSkyBox& cbsb);

	~SkyCube();

	void DrawStatic();

	void CreateCB(const ConstantBufferSkyBox& cbsb);

	void UpdateCB(ConstantBufferSkyBox& cbsb);

public:
	static VertexBuffer vertex_buffer;
	static IndexBuffer  index_buffer;

private:
	ConstantBuffer<ConstantBufferSkyBox> constant_buffer;
};

