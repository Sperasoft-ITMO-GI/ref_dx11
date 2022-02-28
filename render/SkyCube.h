#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

class SkyCube {
public:
	SkyCube();
	SkyCube(MODEL& cbsb);

	~SkyCube();

	void DrawStatic();

	void CreateCB(const MODEL& cbsb);

	void UpdateCB(MODEL& cbsb);

public:
	static VertexBuffer vertex_buffer;
	static IndexBuffer  index_buffer;

private:
	ConstantBuffer<MODEL> constant_buffer;
};

