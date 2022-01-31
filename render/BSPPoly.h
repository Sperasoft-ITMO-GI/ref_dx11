#pragma once 

#include <array>
#include <utility>

#include <WndDxIncludes.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <ConstantBuffer.h>
#include <BSPVertex.h>

class BSPPoly {
public:
	BSPPoly(ConstantBuffer<ConstantBufferPolygon>& cb, VertexBuffer& vb, IndexBuffer& ib,
		    int flags, int tex_index, int lm_index = -1);

	int GetFlags();

	void Draw();
private:
	int flags;
	int texture_index;
	int lightmap_index;

private:
	VertexBuffer vb;
	IndexBuffer ib;
	ConstantBuffer<ConstantBufferPolygon> cb;
};