#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.h>
#include <ModelVertex.h>

class BSPPoly {
public:
	BSPPoly(ConstantBuffer<ConstantBufferPolygon> cb, int flags, int tex_index, int lm_index);

	int GetFlags();

	void Draw();
private:
	int flags;
	int texture_index;
	int lightmap_index;

public:
	static VertexBuffer vb;
	static IndexBuffer  ib;

private:
	ConstantBuffer<ConstantBufferPolygon> cb;
};