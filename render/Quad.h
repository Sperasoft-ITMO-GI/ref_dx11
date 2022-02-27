#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.hpp>

#include <shaders/shader_defines.h>

class Quad  {
public:
	Quad();
	Quad(UI_BUFFER& cbq);

	~Quad();

	void DrawStatic();

	void CreateCB(const UI_BUFFER& cbq);

	void UpdateCB(UI_BUFFER& cbq);

public:
	static VertexBuffer vertex_buffer;
	static IndexBuffer  index_buffer;

private:
	ConstantBuffer<UI_BUFFER> constant_buffer;
};

